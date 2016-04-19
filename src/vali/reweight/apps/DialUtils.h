#ifndef DIALUTILS_H_SEEN
#define DIALUTILS_H_SEEN

#include <iostream>

#include "params.h"

#include "NuwroReWeight.h"
#include "NuwroReWeight_QEL.h"
#include "NuwroReWeight_FlagNorm.h"
#include "NuwroReWeight_SPP.h"
#include "NuwroSyst.h"
#include "NuwroSystSet.h"

inline nuwro::rew::NuwroWghtEngineI* GetWghtEngineFromDial(
  nuwro::rew::ENuWroSyst sys,
  params const &param){

  if(sys == nuwro::rew::kNullSystematic){ return NULL; }

  switch(sys){
    case nuwro::rew::kNuwro_Ma_CCQE: {
      return new nuwro::rew::NuwroReWeight_QEL(param);
    }
    // case nuwro::rew::kNuwro_MECNorm: {
    //   return new nuwro::rew::NuwroReWeight_FlagNorm();
    // }
    case nuwro::rew::kNuwro_DeltaS_NCEL:{
      return new nuwro::rew::NuwroReWeight_QEL(param);
    }
    case nuwro::rew::kNuwro_MaRES:{
      return new nuwro::rew::NuwroReWeight_SPP(param);
    }
    case nuwro::rew::kNuwro_CA5:{
      return new nuwro::rew::NuwroReWeight_SPP(param);
    }
    case nuwro::rew::kNuwro_SPPBkgScale:{
      return new nuwro::rew::NuwroReWeight_SPP(param);
    }
    default:{
      return NULL;
    }
  }
}

inline nuwro::rew::NuwroWghtEngineI* GetWghtEngineFromDialString(
  std::string DialName, params const &param){
  nuwro::rew::ENuWroSyst sys = nuwro::rew::NuwroSyst::FromString(DialName);
  return GetWghtEngineFromDial(sys, param);
}

inline void PrintDialsImplemented(){
  nuwro::rew::NuwroReWeight_SPP::DoSetupSPP = false;
  params dummy;
  std::cout << "[INFO]: Currently implemented dials." << std::endl;
  for(nuwro::rew::ENuWroSyst sys = nuwro::rew::kNullSystematic;
    sys != nuwro::rew::kNNuWroSystematics;
    nuwro::rew::NuwroSyst::Adv(sys)){

    nuwro::rew::NuwroWghtEngineI* eng =
      GetWghtEngineFromDial(sys,dummy);
    if(eng){
      std::cout << "\t" << sys << std::endl;
      delete eng;
    }
  }
}

#endif
