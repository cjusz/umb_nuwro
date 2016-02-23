#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "RooTrackerEvent.h"

#include "SimpleAnalysisFormat.h"

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " <TChain::Add specifer of NuWro rootracker files> "
    "<output ROOT file name>" << std::endl;
}

int main(int argc, char const * argv[]){

  if(argc != 3){
    std::cerr << "[ERROR]: Found " << (argc-1) << " cli args. Expected 2."
      << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }

  TFile *inputFile = TFile::Open(argv[1]);
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

  RooTrackerUtils::RooTrackerEvent inpEv;
  if(!inpEv.JackIn(rootracker)){
    std::cerr << "[ERROR]: Couldn't Jack the RooTrackerEvent into the input"
      " tree. Was it malformed?" << std::endl;
      return 8;
  } else {
    std::cout << "[INFO]: StdHepEvent jacked in, here we go." << std::endl;
  }

  TFile* outputFile = TFile::Open(argv[2],"RECREATE");
  if(!outputFile || ! outputFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << argv[2] << " for writing."
      << std::endl;
    return 16;
  }
  TTree* outputTree = new TTree("nwSAF","");
  SimpleAnalysisFormat safEv;

  safEv.AddBranches(outputTree);

  Long64_t nEntries = rootracker->GetEntries();

  for(Long64_t ent = 0; ent < nEntries; ++ent){
    rootracker->GetEntry(ent);
    safEv.NeutConventionReactionCode = inpEv.fEvtCode->GetString().Atoi();
    for(Int_t i = 0; i < inpEv.fStdHepN; ++i){
      safEv.HandleStdHepParticle(inpEv.fStdHepPdg[i], inpEv.fStdHepStatus[i],
        inpEv.fStdHepP4[i]);
    }
    safEv.EvtWght = inpEv.fEvtWght;
    safEv.Finalise();
    outputTree->Fill();
    safEv.Reset();
    std::cout << "\r" << int((ent+1)*100/nEntries) << "\% done."
      << std::flush;
  }
  std::cout << std::endl << "[INFO]: Wrote " << nEntries << " entries."
    << std::endl;
  outputFile->Write();
  outputFile->Close();
}
