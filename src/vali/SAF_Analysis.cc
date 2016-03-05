#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "RooTrackerEvent.h"

#include "SimpleAnalysisFormat.h"

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " [-n] <TChain::Add specifer of input event files> "
    "<output ROOT file name>" << std::endl;
  std::cout << "\t -n: if specified input is expected to be NuWro event files,"
    << " otherwise rootracker." << std::endl;
}

std::pair<TFile*,TTree*> MakeOutputFile(SimpleAnalysisFormat &safEv,
  char const *OutputFileName){

  TFile* outputFile = TFile::Open(OutputFileName,"RECREATE");
  if(!outputFile || ! outputFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << OutputFileName << " for writing."
      << std::endl;
    return std::pair<TFile*,TTree*>(NULL,NULL);
  }
  TTree* outputTree = new TTree("nwSAF","");
  safEv.AddBranches(outputTree);
  return std::pair<TFile*,TTree*>(outputFile, outputTree);
}

int ReadRooTracker(char const * RooTrackerFile, char const * OutputFileName){
  TFile *inputFile = TFile::Open(RooTrackerFile);
  if(!inputFile || ! inputFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << RooTrackerFile << " for reading."
      << std::endl;
    return 2;
  }
  TTree *rootracker = dynamic_cast<TTree*>(inputFile->Get("nRooTracker"));
  if(!rootracker){
    std::cerr << "[ERROR]: Couldn't find TTree (\"nRooTracker\") in file "
      << RooTrackerFile << "." << std::endl;
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

  SimpleAnalysisFormat safEv;
  std::pair<TFile*,TTree*> outputters = MakeOutputFile(safEv, OutputFileName);
  if(!outputters.first){
    return 16;
  }

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
    outputters.second->Fill();
    safEv.Reset();
    std::cout << "\r" << int((ent+1)*100/nEntries) << "\% done."
      << std::flush;
  }
  std::cout << std::endl << "[INFO]: Wrote " << nEntries << " entries."
    << std::endl;
  outputters.first->Write();
  outputters.first->Close();
  inputFile->Close();
  return 0;
}

int ReadNuWroEvent(char const * NuWroEvsFile, char const * OutputFileName){
  TFile *inputFile = TFile::Open(NuWroEvsFile);
  if(!inputFile || ! inputFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << NuWroEvsFile << " for reading."
      << std::endl;
    return 2;
  }
  TTree *NuWroEvTree = dynamic_cast<TTree*>(inputFile->Get("treeout"));
  if(!NuWroEvTree){
    std::cerr << "[ERROR]: Couldn't find TTree (\"treeout\") in file "
      << NuWroEvsFile << "." << std::endl;
    return 4;
  }

  event *nwEv = 0; // ROOT is in control of object life cycle
  NuWroEvTree->SetBranchAddress("e", &nwEv);

  SimpleAnalysisFormat safEv;
  std::pair<TFile*,TTree*> outputters = MakeOutputFile(safEv, OutputFileName);
  if(!outputters.first){
    return 16;
  }

  Long64_t nEntries = NuWroEvTree->GetEntries();

  for(Long64_t ent = 0; ent < nEntries; ++ent){
    NuWroEvTree->GetEntry(ent);
    safEv = MakeSimpleAnalysisFormat(*nwEv);
    outputters.second->Fill();
    safEv.Reset();
    std::cout << "\r" << int((ent+1)*100/nEntries) << "\% done."
      << std::flush;
  }
  std::cout << std::endl << "[INFO]: Wrote " << nEntries << " entries."
    << std::endl;
  outputters.first->Write();
  outputters.first->Close();
  inputFile->Close();
  return 0;
}

int main(int argc, char const * argv[]){

  if((argc < 3) || (argc > 4)){
    std::cerr << "[ERROR]: Found " << (argc-1) << " cli args. Expected 2 or 3."
      << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }

  if( (argc == 4) && std::string(argv[1]) == "-n"){
    return ReadNuWroEvent(argv[2], argv[3]);
  } else if (argc == 4){
    std::cout << "[ERROR]: Recieved 3 cli args, but the first was not \"-n\" as"
      " expected." << std::endl;
    return 1;
  }

  return ReadRooTracker(argv[1], argv[2]);

}
