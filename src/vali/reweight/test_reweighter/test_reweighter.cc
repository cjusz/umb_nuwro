#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "event1.h"

#include "NuwroReWeight.h"
#include "NuwroReWeight_MaCCQE.h"
#include "NuwroReWeight_MaRES_CA5.h"
#include "NuwroSyst.h"
#include "NuwroSystSet.h"
#include "RooTrackerEvent.h"

using namespace nuwro::rew;
using namespace RooTrackerUtils;

NuwroWghtEngineI* GetWghtEngineFromDial(ENuWroSyst sys, params const &param){

  if(sys == kNullSystematic){ return NULL; }

  switch(sys){
    case kNuwro_MaCCQE:{
      return new NuwroReWeight_MaCCQE(param);
    }
    case kNuwro_MaRES:{
      return new NuwroReWeight_MaRES_CA5(param);
    }
    case kNuwro_CA5:{
      return new NuwroReWeight_MaRES_CA5(param);
    }
    default:{
      return NULL;
    }
  }
}

NuwroWghtEngineI* GetWghtEngineFromDialString(std::string DialName,
  params const &param){
  ENuWroSyst sys = NuwroSyst::FromString(DialName);
  return GetWghtEngineFromDial(sys, param);
}

void PrintDialsImplemented(){
  NuwroReWeight_MaRES_CA5::DoSetupSPP = false;
  params dummy;
  std::cout << "[INFO]: Currently implemented dials." << std::endl;
  for(ENuWroSyst sys = kNullSystematic; sys != kNNuWroSystematics;
    NuwroSyst::Adv(sys)){

    NuwroWghtEngineI* eng =
      GetWghtEngineFromDial(sys,dummy);
    if(eng){
      std::cout << "\t" << sys << std::endl;
      delete eng;
    }
  }
}

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " <dial to reweight> <params file for nominal> <input rootracker file> "
    "<output weights file name>" << std::endl;
  PrintDialsImplemented();
}

std::string GetDialValStr(Double_t dialval){
  std::stringstream ss;
  if(dialval < 0){
    ss << "minus";
  }
  ss << fabs(dialval);
  return ss.str();
}

int main(int argc, char const *argv[]){

  if(argc != 5){
    std::cerr << "[ERROR]: Found " << (argc-1) << " cli args. Expected 4."
      << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }

  std::string DialName = argv[1];
  params NomParams;
  NomParams.read(argv[2]);

  TFile *inputFile = TFile::Open(argv[3]);
  if(!inputFile || ! inputFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << argv[1] << " for reading."
      << std::endl;
    return 2;
  }
  TTree *rootracker = dynamic_cast<TTree*>(inputFile->Get("nRooTracker"));
  if(!rootracker){
    std::cerr << "[ERROR]: Couldn't find TTree (\"nRooTracker\") in file "
      << argv[1] << "." << std::endl;
    return 4;
  }

  RooTrackerEvent inpEv;
  if(!inpEv.JackIn(rootracker)){
    std::cerr << "[ERROR]: Couldn't Jack the RooTrackerEvent into the input"
      " tree. Was it malformed?" << std::endl;
      return 8;
  } else {
    std::cout << "[INFO]: StdHepEvent jacked in, here we go." << std::endl;
  }

  TFile* outputFile = TFile::Open(argv[4],"RECREATE");
  if(!outputFile || ! outputFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << argv[2] << " for writing."
      << std::endl;
    return 16;
  }

  TTree* outputTree = new TTree("weights","");

  NuwroReWeight WghtGen;
  NuwroWghtEngineI *wghtCalc = GetWghtEngineFromDialString(DialName,NomParams);
  if(!wghtCalc){
    std::cerr << "[ERROR]: Invalid weight engine, " << DialName
      << ", requested." << std::endl;
    PrintDialsImplemented();
    return 32;
  }
  //I know I could have got it from above...
  ENuWroSyst syst = NuwroSyst::FromString(DialName);

  WghtGen.AdoptWghtCalc(DialName, wghtCalc);

  WghtGen.Systematics().Add(syst, 0, -4, 4, 2);

  NuwroSystInfo & SystInstance =
    WghtGen.Systematics().GetSystInfo(syst);

  std::string nominal = GetDialValStr(SystInstance.CurValue);
  SystInstance.TurnDown();
  std::string d1_val = GetDialValStr(SystInstance.CurValue);
  SystInstance.TurnUp(2);
  std::string d2_val = GetDialValStr(SystInstance.CurValue);

  Double_t weight_d1 = 0;
  outputTree->Branch((std::string("weight_") + DialName + "_" + nominal + "_to_"
    +d1_val).c_str(),
    &weight_d1);
  Double_t weight_d2 = 0;
  outputTree->Branch((std::string("weight_") + DialName + "_" + nominal + "_to_"
    +d2_val).c_str(),
    &weight_d2);

  Long64_t nEntries = rootracker->GetEntries();

  for(Long64_t ent = 0; ent < nEntries; ++ent){
    rootracker->GetEntry(ent);
    event nwev = GetNuWroEvent1(inpEv, 1000.0);
    nwev.par = NomParams;
    SystInstance.TurnDown(2);
    WghtGen.Reconfigure();
    weight_d1 = WghtGen.CalcWeight(&nwev);
    SystInstance.TurnUp(2);
    WghtGen.Reconfigure();
    weight_d2 = WghtGen.CalcWeight(&nwev);
    outputTree->Fill();
    std::cout << "\r" << int((ent+1)*100/nEntries) << "\% done."
      << std::flush;
  }
  std::cout << std::endl << "[INFO]: Wrote " << nEntries << " entries."
    << std::endl;
  outputTree->Write();
  outputFile->Write();
  outputFile->Close();
}
