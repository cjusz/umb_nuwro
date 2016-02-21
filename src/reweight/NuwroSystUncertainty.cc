#include <iostream>

#include "NuwroSystUncertainty.h"

using namespace nuwro::rew;

NuwroSystUncertainty *NuwroSystUncertainty::fInstance = NULL;

NuwroSystUncertainty::NuwroSystUncertainty(){
  std::cout << "[INFO]: NuwroSystUncertainty initialization" << std::endl;
  SetDefaults();
}
NuwroSystUncertainty::~NuwroSystUncertainty(){}

NuwroSystUncertainty * NuwroSystUncertainty::Instance(){
  if(!fInstance) {
    fInstance = new NuwroSystUncertainty;
  }
  return fInstance;
}

bool NuwroSystUncertainty::TearDown(void){
  if(fInstance) { delete fInstance; fInstance = NULL; return true; }
  return false;
}

double NuwroSystUncertainty::OneSigmaErr(NuwroSyst_t syst, int sign)
  const {

  if(sign > 0) {
    std::map<nuwro::rew::NuwroSyst_t,double>::const_iterator it =
      fOneSigPlusErrMap.find(syst);
    if(it != fOneSigPlusErrMap.end()){ return it->second; }
    return 0;
  } else if(sign < 0) {
    std::map<nuwro::rew::NuwroSyst_t,double>::const_iterator it =
      fOneSigMnusErrMap.find(syst);
    if(it != fOneSigMnusErrMap.end()){ return it->second; }
    return 0;
  } else { // Assume symmetric error.
    double err = 0.5 * (OneSigmaErr(syst, +1) + OneSigmaErr(syst, -1));
    return err;
  }
}

void NuwroSystUncertainty::SetUncertainty(NuwroSyst_t syst,
  double plus_err, double minus_err) {
  fOneSigPlusErrMap[syst] = plus_err;
  fOneSigMnusErrMap[syst] = minus_err;
}

void NuwroSystUncertainty::SetDefaults(void){
  SetUncertainty(kNuwro_MaCCQE, 100.0, 100.0);
  SetUncertainty(kNuwro_MECNorm, 1.0, 1.0);
}

