#ifdef DEBUG_SRW
#include <stdexcept>
#include <cassert>
#endif

#include <iostream>

#include <omp.h>

#include "NuwroReWeight_MaCCQE.h"

// #define DEBUG_SRW
#ifdef _OPENMP
#define DEBUG_MULTITHREAD_SRW
#endif

namespace SRW {

  inline void SetupSPP(){
    nuwro::rew::SetupSPP();
  }

  inline Double_t GetRESWeight(event const & nwEv,
    Double_t to_CA5,
    Double_t to_MaRES,
    Double_t NominalWeight=0xDEADBEEF){

    params rwParams = nwEv.par;

    if(NominalWeight==0xDEADBEEF){
      NominalWeight = nuwro::rew::GetWghtPropToResXSec(nwEv,rwParams);
    }

    rwParams.pion_C5A = to_CA5;
    rwParams.pion_axial_mass = to_MaRES;

#ifdef DEBUG_SRW
    Double_t NewWeight = nuwro::rew::GetWghtPropToResXSec(nwEv,rwParams);
    assert(std::isfinite(NominalWeight));
    assert(std::isfinite(NewWeight));
    assert(std::isfinite(NewWeight/NominalWeight));
    return (NewWeight/NominalWeight);
#else
    return (nuwro::rew::GetWghtPropToResXSec(nwEv,rwParams)/NominalWeight);
#endif
  }

  inline void ReWeightRESEvents(std::vector<event> const &nwEvs,
    Double_t to_CA5,
    Double_t to_MaRES, std::vector<Double_t> &OutWeights,
    std::vector<Double_t> const&InWeights=std::vector<Double_t>()){

    bool HaveNomWeights = InWeights.size();

#ifdef DEBUG_SRW
    if(nwEvs.size()!=OutWeights.size()){
      std::cout << "[ERROR](__FILE__:__LINE__): Recieved " << nwEvs.size()
        << " NuWro events and " << OutWeights.size() << " space for weights."
        << std::endl;
      throw std::exception();
    }
#endif

#ifdef DEBUG_SRW
    std::vector<Int_t> ThreadEvs;
    ThreadEvs.resize(11);
#endif

    # pragma omp parallel for
    for(size_t i = 0; i < nwEvs.size(); ++i){
    #pragma omp critical
      {
      OutWeights[i] = GetRESWeight(nwEvs[i],to_CA5,to_MaRES,
        HaveNomWeights ? InWeights[i] :
                        0xDEADBEEF);
      }
#ifdef DEBUG_SRW
      if(omp_get_thread_num()<9){
        ThreadEvs[omp_get_thread_num()]++;
      } else { ThreadEvs[10]++; }
#endif
    }

#ifdef DEBUG_MULTITHREAD_SRW
    std::vector<Double_t> OutWeights_mtdb;
    OutWeights_mtdb.resize(OutWeights.size());
    for(size_t i = 0; i < nwEvs.size(); ++i){
      OutWeights_mtdb[i] = GetRESWeight(nwEvs[i],to_CA5,to_MaRES,
        HaveNomWeights ? InWeights[i] :
                        0xDEADBEEF);
    }
    for(size_t i = 0; i < nwEvs.size(); ++i){
      if(OutWeights[i] != OutWeights_mtdb[i]){
        std::cout << "[ERROR] (" << __FILE__ << ":" << __LINE__
          << "): Found a difference in weight"
          " calculation when running in OpenMP mode. OutWeights[i] ( = "
          << OutWeights[i] << ") != OutWeights_mtdb[i] ( = "
          << OutWeights_mtdb[i] << ")" << std::endl;
      }
      assert(OutWeights[i] == OutWeights_mtdb[i]);
    }
#endif

#ifdef DEBUG_SRW
    for(size_t i = 0; i < (ThreadEvs.size()-1); ++i){
      if(!ThreadEvs[i]){ continue; }
      std::cout << "[INFO]: Reweighting Thread #" << i << ", handled "
        << ThreadEvs[i] << " events." << std::endl;
    }
    if(ThreadEvs[10]){
      std::cout << "[INFO]: Other threads handled "
              << ThreadEvs[10] << " events." << std::endl;
    }
#endif
  }

inline void GenerateNominalRESWeights(std::vector<event> const &nwEvs,
  std::vector<Double_t> &NominalWeights){
#ifdef DEBUG_SRW
  if(nwEvs.size()!=NominalWeights.size()){
    std::cout << "[ERROR] (" << __FILE__ << ":" << __LINE__
      << "): Recieved " << nwEvs.size()
      << " NuWro events and " << NominalWeights.size() << " space for "
      "nominal weights." << std::endl;
    throw std::exception();
  }
#endif

#ifdef DEBUG_MULTITHREAD_SRW
  std::vector<Double_t> NominalWeights_mtdb2;
  NominalWeights_mtdb2.resize(NominalWeights.size());
  std::vector<Double_t> NominalWeights_mtdb0;
  NominalWeights_mtdb0.resize(NominalWeights.size());
#endif

  # pragma omp parallel for
  for(size_t i = 0; i < nwEvs.size(); ++i){
    NominalWeights_mtdb0[i] = nuwro::rew::GetWghtPropToResXSec(nwEvs[i],nwEvs[i].par);
    #pragma omp critical
    {
      // std::cout << "==["<<i<<"]" << std::endl;
      NominalWeights[i] = nuwro::rew::GetWghtPropToResXSec(nwEvs[i],nwEvs[i].par, false);
      // std::cout << "__["<<i<<"]" << std::endl;
    }
    NominalWeights_mtdb2[i] = nuwro::rew::GetWghtPropToResXSec(nwEvs[i],nwEvs[i].par);
  }

#ifdef DEBUG_MULTITHREAD_SRW
    std::vector<Double_t> NominalWeights_mtdb;
    NominalWeights_mtdb.resize(NominalWeights.size());
    for(size_t i = 0; i < nwEvs.size(); ++i){
      NominalWeights_mtdb[i] =
        nuwro::rew::GetWghtPropToResXSec(nwEvs[i],nwEvs[i].par);
    }
    for(size_t i = 0; i < nwEvs.size(); ++i){
      if(NominalWeights[i] != NominalWeights_mtdb[i]){
        std::cout << "[ERROR] (" << __FILE__ << ":" << __LINE__
          << "): Found a difference in weight"
          " calculation when running in OpenMP mode. NominalWeights[" << i << "] ( = "
          << NominalWeights[i] << ") != NominalWeights_mtdb[" << i << "] ( = "
          << NominalWeights_mtdb[i] << ")" << std::endl;
        std::cout << "[DEBUGGING]: Try0: " << NominalWeights_mtdb0[i] << std::endl;
        std::cout << "[DEBUGGING]: Try2: " << NominalWeights_mtdb2[i] << std::endl;
        std::cout << "++["<<i<<"]" << std::endl;
        std::cout << "[DEBUGGING]: Try3: "
          << nuwro::rew::GetWghtPropToResXSec(nwEvs[i],nwEvs[i].par, false)
          << std::endl;
        std::cout << "--["<<i<<"]" << std::endl;
        # pragma omp parallel for
        for(size_t j = 0; j < nwEvs.size(); ++j){
          NominalWeights_mtdb2[j] = nuwro::rew::GetWghtPropToResXSec(nwEvs[j],nwEvs[j].par);
          if(j == i){
            std::cout << "[DEBUGGING]: Try4: " << NominalWeights_mtdb2[j] << std::endl;
          }
        }
      }
      assert(NominalWeights[i] == NominalWeights_mtdb[i]);
    }
#endif
}

// inline bool LoadSignalEventsIntoVector(TTree* SAF_Tree,
//   TTree* RooTrackerTree, std::vector<>)

}
