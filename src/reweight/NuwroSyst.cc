#include <iostream>
#include <string>
#include "NuwroSyst.h"

using namespace nuwro::rew;

std::string NuwroSyst::AsString(NuwroSyst_t syst){
  switch(syst) {
    case kNullSystematic: {
      return "kNullSystematic";
    }
    case kNuwro_MaCCQE: {
      return "kNuwro_MaCCQE";
    }
    case kNuwro_MECNorm: {
      return "kNuwro_MECNorm";
    }
    case kNuwro_MaRES: {
      return "kNuwro_MaRES";
    }
    case kNuwro_CA5: {
      return "kNuwro_CA5";
    }
    default: {
      return "-";
    }
  }
}

NuwroSyst_t NuwroSyst::FromString(std::string syst){
  for(NuwroSyst_t rtnSyst = Next(kNullSystematic);
    rtnSyst != kNullSystematic;
    Adv(rtnSyst) ){
    if(AsString(rtnSyst) == syst) {
      return rtnSyst;
    }
  }
  return kNullSystematic;
};

NuwroSyst_t NuwroSyst::Next(NuwroSyst_t const& syst){
  if(syst != kNNuWroSystematics){
    return static_cast<NuwroSyst_t>(int(syst)+1);
  }
  return kNullSystematic;
}
NuwroSyst_t NuwroSyst::Prev(NuwroSyst_t const& syst){
  if(syst != kNullSystematic){
    return static_cast<NuwroSyst_t>(int(syst)-1);
  }
  return kNullSystematic;
}
void NuwroSyst::Adv(NuwroSyst_t &syst){
  syst = Next(syst);
}
void NuwroSyst::Red(NuwroSyst_t &syst){
  syst = Prev(syst);
}

std::ostream& operator<<(std::ostream &os, ENuWroSyst const & syst){
  return os << NuwroSyst::AsString(syst) << std::flush;
}
