#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "event1.h"

#include "reweight/NuwroReWeight.h"
#include "reweight/NuwroReWeight_MaCCQE.h"
#include "reweight/NuwroSyst.h"
#include "reweight/NuwroSystSet.h"
#include "reweight/RooTrackerEvent.h"

using namespace nuwro::rew;
using namespace RooTrackerUtils;

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " <params file for nominal> <input rootracker file> "
    "<output weights file name>" << std::endl;
}

std::string GetDialValStr(Double_t dialval){
  std::stringstream ss;
  if(dialval<0){
    ss << "minus";
  }
  ss << fabs(dialval);
  return ss.str();
}

int main(int argc, char const *argv[]){

  if(argc != 4){
    std::cerr << "[ERROR]: Found " << (argc-1) << " cli args. Expected 3."
      << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }

  params NomParams;
  NomParams.read(argv[1]);

  TFile *inputFile = TFile::Open(argv[2]);
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

  TFile* outputFile = TFile::Open(argv[3],"RECREATE");
  if(!outputFile || ! outputFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << argv[2] << " for writing."
      << std::endl;
    return 16;
  }

  TTree* outputTree = new TTree("nRooTracker_weights","");

  NuwroReWeight WghtGen;
  WghtGen.AdoptWghtCalc("MAQE",new NuwroReWeight_MaCCQE());
  WghtGen.Systematics().Add(kNuwro_MaCCQE, 0, -5, 5, 2);

  std::string nominal = GetDialValStr(WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).CurValue);
  WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).TurnDown();
  std::string d1_val = GetDialValStr(WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).CurValue);
  WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).TurnUp(2);
  std::string d2_val = GetDialValStr(WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).CurValue);

  Double_t weight_d1 = 0;
  outputTree->Branch((std::string("weight_MAQE_")+nominal+"_to_"+d1_val).c_str(),
    &weight_d1);
  Double_t weight_d2 = 0;
  outputTree->Branch((std::string("weight_MAQE_")+nominal+"_to_"+d2_val).c_str(),
    &weight_d2);


  for(Long64_t ent = 0; ent < rootracker->GetEntries(); ++ent){
    rootracker->GetEntry(ent);
    event nwev = GetNuWroEvent1(inpEv, 1000.0);
    nwev.par = NomParams;
    std::cout << "Entry[" << ent << "] NuWro event. Dyn: " << nwev.dyn
      << ", ENu: " << nwev.E() << ", Q2: " << nwev.q2()
      << ", Mass(N0): " << nwev.N0().mass() << ", NuPdg: "
      << nwev.nu().pdg << std::endl;

    WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).TurnDown(2);
    WghtGen.Reconfigure();
    weight_d1 = WghtGen.CalcWeight(&nwev);
    WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).TurnUp(2);
    WghtGen.Reconfigure();
    weight_d2 = WghtGen.CalcWeight(&nwev);

    outputTree->Fill();
  }
  outputTree->Write();
  outputFile->Write();
  outputFile->Close();
}
