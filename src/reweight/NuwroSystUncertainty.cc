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

  SetUncertainty(kNuwro_Ma_CCQE,  160, 160); //MeV
  SetUncertainty(kNuwro_Ma_NCEL,  0.16, 0.16); //MeV
  SetUncertainty(kNuwro_SMa_NCEL, 0.16, 0.16); //MeV
  SetUncertainty(kNuwro_DeltaS_NCEL,   0.10, 0.10); //MeV

  // FLAGS Norm
  SetUncertainty(kNuwro_QELNorm, 1.0, 1.0);
  SetUncertainty(kNuwro_RESNorm, 1.0, 1.0);
  SetUncertainty(kNuwro_DISNorm, 1.0, 1.0);
  SetUncertainty(kNuwro_COHNorm, 1.0, 1.0);
  SetUncertainty(kNuwro_MECNorm, 0.25, 0.25);
  SetUncertainty(kNuwro_CCNorm, 1.0, 1.0);
  SetUncertainty(kNuwro_NCNorm, 1.0, 1.0);
  SetUncertainty(kNuwro_AntiNuNorm, 1.0, 1.0);

  // DYN Norm
  SetUncertainty(kNuwro_DYNNorm_0, 1.0, 1.0);
  SetUncertainty(kNuwro_DYNNorm_1, 1.0, 1.0);
  SetUncertainty(kNuwro_DYNNorm_2, 1.0, 1.0);
  SetUncertainty(kNuwro_DYNNorm_3, 1.0, 1.0);
  SetUncertainty(kNuwro_DYNNorm_4, 1.0, 1.0);
  SetUncertainty(kNuwro_DYNNorm_5, 1.0, 1.0);
  SetUncertainty(kNuwro_DYNNorm_6, 1.0, 1.0);
  SetUncertainty(kNuwro_DYNNorm_7, 1.0, 1.0);
  SetUncertainty(kNuwro_DYNNorm_8, 1.0, 1.0);
  SetUncertainty(kNuwro_DYNNorm_9, 1.0, 1.0);

  // BBBA07
  SetUncertainty(kNuwro_BBBA07_AEp1, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEp2, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEp3, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEp4, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEp5, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEp6, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEp7, 0.10, 0.10);

  SetUncertainty(kNuwro_BBBA07_AMp1, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMp2, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMp3, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMp4, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMp5, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMp6, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMp7, 0.10, 0.10);

  SetUncertainty(kNuwro_BBBA07_AEn1, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEn2, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEn3, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEn4, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEn5, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEn6, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AEn7, 0.10, 0.10);

  SetUncertainty(kNuwro_BBBA07_AMn1, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMn2, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMn3, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMn4, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMn5, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMn6, 0.10, 0.10);
  SetUncertainty(kNuwro_BBBA07_AMn7, 0.10, 0.10);

  // RES
  SetUncertainty(kNuwro_MaRES, 0.1, 0.1); // GeV
  SetUncertainty(kNuwro_CA5, 0.1, 0.1);
  SetUncertainty(kNuwro_SPPDISBkgScale, 0.25, 0.25);
}

