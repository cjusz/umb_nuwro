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

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " <params file for nominal> <input rootracker file> "
    "<output weights file name>" << std::endl;
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
  inpEv.InpToMev = 1000;

  Double_t weight = 0;
  TFile* outputFile = TFile::Open(argv[3],"RECREATE");
  if(!outputFile || ! outputFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << argv[2] << " for writing."
      << std::endl;
    return 16;
  }

  TTree* outputTree = new TTree("nRooTracker_weights","");
  outputTree->SetDirectory(outputFile);
  outputTree->Branch("weight",&weight);

  NuwroReWeight WghtGen;
  WghtGen.AdoptWghtCalc("MAQE",new NuwroReWeight_MaCCQE());
  WghtGen.Systematics().Add(kNuwro_MaCCQE, -2, -5, 5, 2);

  for(Long64_t ent = 0; ent < rootracker->GetEntries(); ++ent){
    rootracker->GetEntry(ent);
    event nwev = inpEv.GetNuWroEvent1();
    nwev.par = NomParams;
    std::cout << "Entry[" << ent << "] NuWro event. Dyn: " << nwev.dyn
      << ", ENu: " << nwev.E() << ", Q2: " << nwev.q2()
      << ", Mass(N0): " << nwev.N0().mass() << ", NuPdg: "
      << nwev.nu().pdg << std::endl;

    WghtGen.Reconfigure();
    std::cout << "\tWeight (Dial Val: "
      << WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).CurValue << "): "
      << WghtGen.CalcWeight(&nwev) << std::endl;
    WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).TurnUp();
    WghtGen.Reconfigure();
    std::cout << "\tWeight (Dial Val: "
      << WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).CurValue << "): "
      << WghtGen.CalcWeight(&nwev) << std::endl;
    WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).TurnUp();
    WghtGen.Reconfigure();
    std::cout << "\tWeight (Dial Val: "
      << WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).CurValue << "): "
      << WghtGen.CalcWeight(&nwev) << std::endl;
    WghtGen.Systematics().GetSystInfo(kNuwro_MaCCQE).TurnDown(2);

    outputTree->Fill();
  }
}
