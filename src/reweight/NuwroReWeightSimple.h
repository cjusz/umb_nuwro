#ifdef DEBUG_SRW
#include <cassert>
#include <stdexcept>
#endif

#include <iomanip>
#include <iostream>
#include <limits>

#include <omp.h>

#include "NuwroReWeight_QEL.h"
#include "NuwroReWeight_SPP.h"

#include "NuwroEventLoaders.h"

//#define DEBUG_SRW
#ifdef _OPENMP
//#define DEBUG_SRW_OMP
#endif

namespace SRW {

inline void SetupSPP() { nuwro::rew::SetupSPP(); }
inline void SetupSPP(params const &p) {
  params p_dummy = p;
  nuwro::rew::SetupSPP(p_dummy);
}

struct SimpleReWeightParams {
  Double_t to_CA5;
  Double_t to_MaRES;
  Double_t to_SPPBkgScale;
  Double_t to_MaQE;
  SimpleReWeightParams(double CA5 = 0xDEADBEEF, double MaRES = 0xDEADBEEF,
                       double SPPBkgScale = 0xDEADBEEF,
                       double MaQE = 0xDEADBEEF)
      : to_CA5(CA5),
        to_MaRES(MaRES),
        to_SPPBkgScale(SPPBkgScale),
        to_MaQE(MaQE) {}
  SimpleReWeightParams(double Params[4])
      : to_CA5(Params[0]),
        to_MaRES(Params[1]),
        to_SPPBkgScale(Params[2]),
        to_MaQE(Params[3]) {}
  bool HasTweaks() const {
    return (to_CA5 != 0xDEADBEEF) || (to_MaRES != 0xDEADBEEF) ||
           (to_SPPBkgScale != 0xDEADBEEF) || (to_MaQE != 0xDEADBEEF);
  }
};

inline std::ostream &operator<<(std::ostream &o,
                                SimpleReWeightParams const &p) {
  return o << "[ CA5: " << p.to_CA5 << ", MaRES: " << p.to_MaRES
           << ", SPPBkgScale: " << p.to_SPPBkgScale << ", MaQE: " << p.to_MaQE << "]"
           << std::flush;
}

inline Double_t GetRESWeight(SRWEvent const &nwEv, params const &NominalParams,
                             Double_t to_CA5 = 0xDEADBEEF,
                             Double_t to_MaRES = 0xDEADBEEF,
                             Double_t to_SPPBkgScale = 0xDEADBEEF,
                             Double_t NominalWeight = 0xDEADBEEF) {
  params rwParams = NominalParams;

  if (NominalWeight == 0xDEADBEEF) {
    NominalWeight = nuwro::rew::GetWghtPropToResXSec(nwEv, rwParams);
  }

  if (to_CA5 != 0xDEADBEEF) {
    rwParams.pion_C5A = to_CA5;
  }
  if (to_MaRES != 0xDEADBEEF) {
    rwParams.pion_axial_mass = to_MaRES;
  }
  if (to_SPPBkgScale != 0xDEADBEEF) {
    rwParams.SPPBkgScale = to_SPPBkgScale;
  }

#ifdef DEBUG_SRW
  Double_t NewWeight = nuwro::rew::GetWghtPropToResXSec(nwEv, rwParams);
  assert(std::isfinite(NominalWeight));
  assert(std::isfinite(NewWeight));
  assert(std::isfinite(NewWeight / NominalWeight));
  return (NewWeight / NominalWeight);
#else
  return (nuwro::rew::GetWghtPropToResXSec(nwEv, rwParams) / NominalWeight);
#endif
}

inline void ReWeightRESEvents(
    std::vector<SRWEvent> const &nwEvs, params const &NominalParams,
    std::vector<Double_t> &OutWeights,
    std::vector<Double_t> const &InWeights = std::vector<Double_t>(),
    Double_t to_CA5 = 0xDEADBEEF, Double_t to_MaRES = 0xDEADBEEF,
    Double_t to_SPPBkgScale = 0xDEADBEEF) {
  if ((to_CA5 == 0xDEADBEEF) && (to_MaRES == 0xDEADBEEF) &&
      (to_SPPBkgScale == 0xDEADBEEF)) {
#ifdef DEBUG_SRW
    std::cout << "[WARN]: found defaults for all reweighted parameter values."
              << std::endl;
#endif
    return;
  }

  bool HaveNomWeights = InWeights.size();

#ifdef DEBUG_SRW
  if (nwEvs.size() != OutWeights.size()) {
    std::cout << "[ERROR](" << __FILE__ << ":" << __LINE__ << "): Recieved "
              << nwEvs.size() << " NuWro events and " << OutWeights.size()
              << " space for weights." << std::endl;
    throw std::exception();
  }
#endif

#ifdef DEBUG_SRW_OMP
  std::vector<Int_t> ThreadEvs;
  ThreadEvs.resize(11);
#endif

#pragma omp parallel for
  for (size_t i = 0; i < nwEvs.size(); ++i) {
    OutWeights[i] =
        GetRESWeight(nwEvs[i], NominalParams, to_CA5, to_MaRES, to_SPPBkgScale,
                     HaveNomWeights ? InWeights[i] : 0xDEADBEEF);
#ifdef DEBUG_SRW_OMP
    if (omp_get_thread_num() < 9) {
      ThreadEvs[omp_get_thread_num()]++;
    } else {
      ThreadEvs[10]++;
    }
#endif
  }

#ifdef DEBUG_SRW_OMP
  for (size_t i = 0; i < (ThreadEvs.size() - 1); ++i) {
    if (!ThreadEvs[i]) {
      continue;
    }
    std::cout << "[INFO]: Reweighting Thread #" << i << ", handled "
              << ThreadEvs[i] << " events." << std::endl;
  }
  if (ThreadEvs[10]) {
    std::cout << "[INFO]: Other threads handled " << ThreadEvs[10] << " events."
              << std::endl;
  }
#endif
}

inline Double_t GetQEWeight(SRWEvent const &nwEv, params const &NominalParams,
                            Double_t to_MaQE = 0xDEADBEEF,
                            Double_t NominalWeight = 0xDEADBEEF) {
  params rwParams = NominalParams;

  if (NominalWeight == 0xDEADBEEF) {
    NominalWeight = nuwro::rew::GetWghtPropToQEXSec(nwEv, rwParams);
  }

  if (to_MaQE != 0xDEADBEEF) {
    rwParams.qel_cc_axial_mass = to_MaQE;
  }

#ifdef DEBUG_SRW
  Double_t NewWeight = nuwro::rew::GetWghtPropToQEXSec(nwEv, rwParams);
  assert(std::isfinite(NominalWeight));
  assert(std::isfinite(NewWeight));
  assert(std::isfinite(NewWeight / NominalWeight));
  return (NewWeight / NominalWeight);
#else
  double NewWeight = nuwro::rew::GetWghtPropToQEXSec(nwEv, rwParams);
  if((NewWeight-NominalWeight) > 1E-8){
    throw 5;
  }
  return ( NewWeight / NominalWeight);
#endif
}

inline void ReWeightQEEvents(
    std::vector<SRWEvent> const &nwEvs, params const &NominalParams,
    std::vector<Double_t> &OutWeights,
    std::vector<Double_t> const &InWeights = std::vector<Double_t>(),
    Double_t to_MaQE = 0xDEADBEEF) {
  if ((to_MaQE == 0xDEADBEEF)) {
#ifdef DEBUG_SRW
    std::cout << "[WARN]: found defaults for all reweighted parameter values."
              << std::endl;
#endif
    return;
  }

  bool HaveNomWeights = InWeights.size();

#ifdef DEBUG_SRW
  if (nwEvs.size() != OutWeights.size()) {
    std::cout << "[ERROR](" << __FILE__ << ":" << __LINE__ << "): Recieved "
              << nwEvs.size() << " NuWro events and " << OutWeights.size()
              << " space for weights." << std::endl;
    throw std::exception();
  }
#endif

#ifdef DEBUG_SRW_OMP
  std::vector<Int_t> ThreadEvs;
  ThreadEvs.resize(11);
#endif

#pragma omp parallel for
  for (size_t i = 0; i < nwEvs.size(); ++i) {
    OutWeights[i] = GetQEWeight(nwEvs[i], NominalParams, to_MaQE,
                                HaveNomWeights ? InWeights[i] : 0xDEADBEEF);
#ifdef DEBUG_SRW_OMP
    if (omp_get_thread_num() < 9) {
      ThreadEvs[omp_get_thread_num()]++;
    } else {
      ThreadEvs[10]++;
    }
#endif
  }

#ifdef DEBUG_SRW_OMP
  for (size_t i = 0; i < (ThreadEvs.size() - 1); ++i) {
    if (!ThreadEvs[i]) {
      continue;
    }
    std::cout << "[INFO]: Reweighting Thread #" << i << ", handled "
              << ThreadEvs[i] << " events." << std::endl;
  }
  if (ThreadEvs[10]) {
    std::cout << "[INFO]: Other threads handled " << ThreadEvs[10] << " events."
              << std::endl;
  }
#endif
}

/// Overall interface

inline void GenerateNominalWeights(std::vector<SRWEvent> const &nwEvs,
                                   params const &NominalParams,
                                   std::vector<Double_t> &NominalWeights) {
#ifdef DEBUG_SRW
  if (nwEvs.size() != NominalWeights.size()) {
    std::cout << "[ERROR] (" << __FILE__ << ":" << __LINE__ << "): Recieved "
              << nwEvs.size() << " NuWro events and " << NominalWeights.size()
              << " space for "
                 "nominal weights."
              << std::endl;
    throw std::exception();
  }
#endif

  std::cout << "Generating Nominal Weights for: CA5: " << NominalParams.pion_C5A
    << ", MaRES: " << NominalParams.pion_axial_mass << std::endl;

#pragma omp parallel for
  for (size_t i = 0; i < nwEvs.size(); ++i) {
    if (nwEvs[i].NuWroDynCode == 0 || nwEvs[i].NuWroDynCode == 1) {
      NominalWeights[i] =
          nuwro::rew::GetWghtPropToQEXSec(nwEvs[i], NominalParams);
    } else if (nwEvs[i].NuWroDynCode == 2 || nwEvs[i].NuWroDynCode == 3) {
      NominalWeights[i] =
          nuwro::rew::GetWghtPropToResXSec(nwEvs[i], NominalParams);
    } else {
      NominalWeights[i] = 1;
    }
  }
}

inline void ReWeightEvents(
    std::vector<SRWEvent> const &nwEvs, params const &NominalParams,
    std::vector<Double_t> &OutWeights,
    std::vector<Double_t> const &InWeights = std::vector<Double_t>(),
    SimpleReWeightParams const &rwp = SimpleReWeightParams()) {
  if (!rwp.HasTweaks()) {
#ifdef DEBUG_SRW
    std::cout << "[WARN]: found defaults for all reweighted parameter values."
              << std::endl;
#endif
    return;
  }

  bool HaveNomWeights = InWeights.size();

#ifdef DEBUG_SRW
  if (nwEvs.size() != OutWeights.size()) {
    std::cout << "[ERROR](" << __FILE__ << ":" << __LINE__ << "): Recieved "
              << nwEvs.size() << " NuWro events and " << OutWeights.size()
              << " space for weights." << std::endl;
    throw std::exception();
  }
#endif

#ifdef DEBUG_SRW_OMP
  std::vector<Int_t> ThreadEvs;
  ThreadEvs.resize(11);
#endif

#pragma omp parallel for
  for (size_t i = 0; i < nwEvs.size(); ++i) {
    if (nwEvs[i].NuWroDynCode == 0 || nwEvs[i].NuWroDynCode == 1) {
      OutWeights[i] = GetQEWeight(nwEvs[i], NominalParams, rwp.to_MaQE,
                                  HaveNomWeights ? InWeights[i] : 0xDEADBEEF);
    } else if (nwEvs[i].NuWroDynCode == 2 || nwEvs[i].NuWroDynCode == 3) {
      OutWeights[i] = GetRESWeight(nwEvs[i], NominalParams, rwp.to_CA5,
                                   rwp.to_MaRES, rwp.to_SPPBkgScale,
                                   HaveNomWeights ? InWeights[i] : 0xDEADBEEF);
    } else {
      OutWeights[i] = 1.0;
    }

#ifdef DEBUG_SRW_OMP
    if (omp_get_thread_num() < 9) {
      ThreadEvs[omp_get_thread_num()]++;
    } else {
      ThreadEvs[10]++;
    }
#endif
  }

#ifdef DEBUG_SRW_OMP
  for (size_t i = 0; i < (ThreadEvs.size() - 1); ++i) {
    if (!ThreadEvs[i]) {
      continue;
    }
    std::cout << "[INFO]: Reweighting Thread #" << i << ", handled "
              << ThreadEvs[i] << " events." << std::endl;
  }
  if (ThreadEvs[10]) {
    std::cout << "[INFO]: Other threads handled " << ThreadEvs[10] << " events."
              << std::endl;
  }
#endif
}
}
