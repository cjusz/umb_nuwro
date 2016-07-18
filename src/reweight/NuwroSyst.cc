#include <iostream>
#include <string>
#include "NuwroSyst.h"

using namespace nuwro::rew;

std::string NuwroSyst::AsString(NuwroSyst_t syst){
  switch(syst) {
    case kNullSystematic: { return "kNullSystematic"; }

    case kNuwro_MaRES: { return "kNuwro_MaRES"; }
    case kNuwro_CA5: { return "kNuwro_CA5"; }
    case kNuwro_SPPBkgScale: { return "kNuwro_SPPBkgScale"; }

    case kNuwro_Ma_CCQE: { return "kNuwro_Ma_CCQE"; }
    case kNuwro_Ma_NCEL: { return "kNuwro_Ma_NCEL"; }
    case kNuwro_SMa_NCEL: { return "kNuwro_SMa_NCEL"; }
    case kNuwro_DeltaS_NCEL: { return "kNuwro_DeltaS_NCEL"; }

      // FLAGS  Norm
    case kNuwro_QELNorm: { return "kNuwro_QELNorm"; }
    case kNuwro_RESNorm: { return "kNuwro_RESNorm"; }
    case kNuwro_DISNorm: { return "kNuwro_DISNorm"; }
    case kNuwro_COHNorm: { return "kNuwro_COHNorm"; }
    case kNuwro_MECNorm: { return "kNuwro_MECNorm"; }
    case kNuwro_CCNorm: { return "kNuwro_CCNorm"; }
    case kNuwro_NCNorm: { return "kNuwro_NCNorm"; }

    case kNuwro_AntiNuNorm: { return "kNuwro_AntiNuNorm"; }

      // DYN Norm
    case kNuwro_DYNNorm_0: { return "kNuwro_DYNNorm_0"; }
    case kNuwro_DYNNorm_1: { return "kNuwro_DYNNorm_1"; }
    case kNuwro_DYNNorm_2: { return "kNuwro_DYNNorm_2"; }
    case kNuwro_DYNNorm_3: { return "kNuwro_DYNNorm_3"; }
    case kNuwro_DYNNorm_4: { return "kNuwro_DYNNorm_4"; }
    case kNuwro_DYNNorm_5: { return "kNuwro_DYNNorm_5"; }
    case kNuwro_DYNNorm_6: { return "kNuwro_DYNNorm_6"; }
    case kNuwro_DYNNorm_7: { return "kNuwro_DYNNorm_7"; }
    case kNuwro_DYNNorm_8: { return "kNuwro_DYNNorm_8"; }
    case kNuwro_DYNNorm_9: { return "kNuwro_DYNNorm_9"; }

      // BBBA07 FF
    case kNuwro_BBBA07_AEp1: { return "kNuwro_BBBA07_AEp1"; }
    case kNuwro_BBBA07_AEp2: { return "kNuwro_BBBA07_AEp2"; }
    case kNuwro_BBBA07_AEp3: { return "kNuwro_BBBA07_AEp3"; }
    case kNuwro_BBBA07_AEp4: { return "kNuwro_BBBA07_AEp4"; }
    case kNuwro_BBBA07_AEp5: { return "kNuwro_BBBA07_AEp5"; }
    case kNuwro_BBBA07_AEp6: { return "kNuwro_BBBA07_AEp6"; }
    case kNuwro_BBBA07_AEp7: { return "kNuwro_BBBA07_AEp7"; }

    case kNuwro_BBBA07_AMp1: { return "kNuwro_BBBA07_AMp1"; }
    case kNuwro_BBBA07_AMp2: { return "kNuwro_BBBA07_AMp2"; }
    case kNuwro_BBBA07_AMp3: { return "kNuwro_BBBA07_AMp3"; }
    case kNuwro_BBBA07_AMp4: { return "kNuwro_BBBA07_AMp4"; }
    case kNuwro_BBBA07_AMp5: { return "kNuwro_BBBA07_AMp5"; }
    case kNuwro_BBBA07_AMp6: { return "kNuwro_BBBA07_AMp6"; }
    case kNuwro_BBBA07_AMp7: { return "kNuwro_BBBA07_AMp7"; }

    case kNuwro_BBBA07_AEn1: { return "kNuwro_BBBA07_AEn1"; }
    case kNuwro_BBBA07_AEn2: { return "kNuwro_BBBA07_AEn2"; }
    case kNuwro_BBBA07_AEn3: { return "kNuwro_BBBA07_AEn3"; }
    case kNuwro_BBBA07_AEn4: { return "kNuwro_BBBA07_AEn4"; }
    case kNuwro_BBBA07_AEn5: { return "kNuwro_BBBA07_AEn5"; }
    case kNuwro_BBBA07_AEn6: { return "kNuwro_BBBA07_AEn6"; }
    case kNuwro_BBBA07_AEn7: { return "kNuwro_BBBA07_AEn7"; }

    case kNuwro_BBBA07_AMn1: { return "kNuwro_BBBA07_AMn1"; }
    case kNuwro_BBBA07_AMn2: { return "kNuwro_BBBA07_AMn2"; }
    case kNuwro_BBBA07_AMn3: { return "kNuwro_BBBA07_AMn3"; }
    case kNuwro_BBBA07_AMn4: { return "kNuwro_BBBA07_AMn4"; }
    case kNuwro_BBBA07_AMn5: { return "kNuwro_BBBA07_AMn5"; }
    case kNuwro_BBBA07_AMn6: { return "kNuwro_BBBA07_AMn6"; }
    case kNuwro_BBBA07_AMn7: { return "kNuwro_BBBA07_AMn7"; }

    case kNuwro_BBBA07_AAx1: { return "kNuwro_BBBA07_AAx1"; }
    case kNuwro_BBBA07_AAx2: { return "kNuwro_BBBA07_AAx2"; }
    case kNuwro_BBBA07_AAx3: { return "kNuwro_BBBA07_AAx3"; }
    case kNuwro_BBBA07_AAx4: { return "kNuwro_BBBA07_AAx4"; }
    case kNuwro_BBBA07_AAx5: { return "kNuwro_BBBA07_AAx5"; }
    case kNuwro_BBBA07_AAx6: { return "kNuwro_BBBA07_AAx6"; }
    case kNuwro_BBBA07_AAx7: { return "kNuwro_BBBA07_AAx7"; }

    case kNuwro_Axl2comp_gamma: { return "kNuwro_Axl2comp_gamma"; }
    case kNuwro_Axl2comp_alpha: { return "kNuwro_Axl2comp_alpha"; }
    case kNuwro_Axl3comp_beta: { return "kNuwro_Axl3comp_beta"; }
    case kNuwro_Axl3comp_theta: { return "kNuwro_Axl3comp_theta"; }

    default: { return "-"; }

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
