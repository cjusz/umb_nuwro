//c++
#include <iostream>
#include <algorithm>

//POSIX
#include <sys/time.h>

//ROOT
#include "TFile.h"
#include "TTree.h"

//nuwro
#include "event1.h"

//nuwro reweighting
#include "NuwroReWeight.h"
#include "NuwroReWeight_QEL.h"
#include "NuwroReWeight_SPP.h"
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

#define DEBUG_MEM
#define PROFILE

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " <Nominal parameters file> <NuWro eventsout.root>" << std::endl;
}

params par;
bool IsSignal(event const &ev) {
  static bool first = true;
  if (first) {
    par = ev.par;
    first = false;
  }
  if (ev.dyn <= 3) {
    return true;
  }
  return false;
}

int main(int argc, char const *argv[]){

  if((argc < 3) || (argc > 4)){
    std::cerr << "[ERROR]: Found " << (argc-1) << " cli args. Expected 2."
      << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }

  params NomParams;
  NomParams.read(argv[1]);

  int argo = 2;
  Long64_t LoadNoMoreThan = std::numeric_limits<Long64_t>::max();
  if (argc == 4) {
    LoadNoMoreThan = atol(argv[argo]);
    LoadNoMoreThan = std::min(LoadNoMoreThan ,std::numeric_limits<Long64_t>::max());
    argo++;
  }

  std::vector<SRW::SRWEvent> SignalSRWs;
  SRW::LoadSignalSRWEventsIntoVector(argv[argo], SignalSRWs, &IsSignal, LoadNoMoreThan);

  std::vector<Double_t> Weights;
  Weights.resize(SignalSRWs.size());

  SRW::SetupSPP();

  double wt1 = get_wall_time();

  size_t NReconfigures = 100;

  SRW::SimpleReWeightParams rwParams;
  rwParams.to_CA5 = 1.0;
  rwParams.to_MaRES = 1.14;

  for(size_t i = 0; i < NReconfigures; i++){
    SRW::ReWeightEvents(SignalSRWs, par, Weights, std::vector<Double_t>(),
      rwParams);
    std::cout << "\r" << int((i+1)*100/NReconfigures) << "\% reconfigured."
      << std::flush;
  } std::cout << std::endl;

  double wt2 = get_wall_time();

  std::cout << "[PROFILE]: " << NReconfigures << " RES reweights took "
    << (wt2-wt1) << " second, with no nominal weight information." << std::endl;

  std::vector<Double_t> NominalWeights;
  NominalWeights.resize(SignalSRWs.size());
  SRW::GenerateNominalWeights(SignalSRWs, par, NominalWeights);

  wt1 = get_wall_time();

  for(size_t i = 0; i < NReconfigures; i++){
    SRW::ReWeightEvents(SignalSRWs, par, Weights, NominalWeights, rwParams);
    std::cout << "\r" << int((i+1)*100/NReconfigures) << "\% reconfigured."
      << std::flush;
  } std::cout << std::endl;

  wt2 = get_wall_time();

  std::cout << "[PROFILE]: " << NReconfigures << " RES reweights took "
    << (wt2-wt1) << " second, with nominal weight information." << std::endl;

}
