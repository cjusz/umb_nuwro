//c++
#include <iostream>

//POSIX
#include <sys/time.h>

//ROOT
#include "TFile.h"
#include "TTree.h"

//nuwro
#include "event1.h"

//nuwro reweighting
#include "NuwroReWeight.h"
#include "NuwroReWeight_MaCCQE.h"
#include "NuwroReWeight_MaRES_CA5.h"
#include "NuwroSyst.h"
#include "NuwroSystSet.h"
#include "RooTrackerEvent.h"

#include "NuwroReWeightSimple.h"

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

using namespace RooTrackerUtils;

#define DEBUG_MEM
#define PROFILE

static const size_t eSize = sizeof(event);
static const size_t pSize = sizeof(particle);

inline size_t NuwroEvSize(event const & ev){
  return ev.in.size()*pSize + ev.temp.size()*pSize + ev.out.size()*pSize +
  ev.post.size()*pSize + ev.all.size()*pSize + eSize;
}

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " <Nominal parameters file> <TChain::Add specifer of NuWro rootracker files> "
    "<output ROOT file name>" << std::endl;
}

int main(int argc, char const *argv[]){

  if(argc != 4){
    std::cerr << "[ERROR]: Found " << (argc-1) << " cli args. Expected 2."
      << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }

  params NomParams;
  NomParams.read(argv[1]);

  TFile *inputFile = TFile::Open(argv[2]);
  if(!inputFile || ! inputFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << argv[2] << " for reading."
      << std::endl;
    PrintUsage(argv[0]);
    return 2;
  }
  TTree *rootracker = dynamic_cast<TTree*>(inputFile->Get("nRooTracker"));
  if(!rootracker){
    std::cerr << "[ERROR]: Couldn't find TTree (\"nRooTracker\") in file "
      << argv[1] << "." << std::endl;
    PrintUsage(argv[0]);
    return 4;
  }

  RooTrackerEvent inpEv;
  if(!inpEv.JackIn(rootracker)){
    std::cerr << "[ERROR]: Couldn't Jack the RooTrackerEvent into the input"
      " tree. Was it malformed?" << std::endl;
    PrintUsage(argv[0]);
    return 8;
  } else {
    std::cout << "[INFO]: StdHepEvent jacked in, here we go." << std::endl;
  }

  Long64_t nEntries = rootracker->GetEntries();
  Long64_t Filled = 0;

  std::vector<event> SignalEvents;
  std::cout << "[INFO]: Loading the signal..." << std::endl;
#ifdef DEBUG_MEM
  size_t CacheSize = 0;
#endif
  for(Long64_t ent = 0; ent < nEntries; ++ent){
    std::cout << "\r" << int((ent+1)*100/nEntries) << "\% loaded "
#ifdef DEBUG_MEM
      << "(" << (CacheSize/size_t(8E6)) << " Mb)"
#endif
      << std::flush;
    rootracker->GetEntry(ent);
    event nwev = GetNuWroEvent1(inpEv, 1000.0);
    //Look for CCRes
    if( (nwev.dyn != 2) || (!nwev.flag.cc) || (!nwev.flag.anty)){
      continue;
    }
    SignalEvents.push_back(nwev);
    SignalEvents.back().par = NomParams;
#ifdef DEBUG_MEM
    CacheSize += NuwroEvSize(nwev);
#endif
  }
  std::cout << std::endl <<
    "[INFO]: Loaded " << SignalEvents.size() << " signal events into memory."
    << std::endl;

  std::vector<Double_t> Weights;
  Weights.resize(SignalEvents.size());

  SRW::SetupSPP();

  double wt1 = get_wall_time();

  size_t NReconfigures = 100;

  for(size_t i = 0; i < NReconfigures; i++){
    SRW::ReWeightRESEvents(SignalEvents,1.0,1.14, Weights);
    std::cout << "\r" << int((i+1)*100/NReconfigures) << "\% reconfigured."
      << std::flush;
  } std::cout << std::endl;

  double wt2 = get_wall_time();

  std::cout << "[PROFILE]: " << NReconfigures << " reweights took "
    << (wt2-wt1) << " second, with no nominal weight information." << std::endl;

  std::vector<Double_t> NominalWeights;
  NominalWeights.resize(SignalEvents.size());
  SRW::GenerateNominalRESWeights(SignalEvents,NominalWeights);

  wt1 = get_wall_time();

  for(size_t i = 0; i < NReconfigures; i++){
    SRW::ReWeightRESEvents(SignalEvents,1.0,1.14, Weights, NominalWeights);
    std::cout << "\r" << int((i+1)*100/NReconfigures) << "\% reconfigured."
      << std::flush;
  } std::cout << std::endl;

  wt2 = get_wall_time();

  std::cout << "[PROFILE]: " << NReconfigures << " reweights took "
    << (wt2-wt1) << " second, with nominal weight information." << std::endl;



}
