#include "NuwroReWeight_FlagNorm.h"

#include "NuwroSystUncertainty.h"

#include <iostream>

#include "TFile.h"
#include "TROOT.h"
#include "TMath.h"

//__________________________________________________________________
nuwro::rew::NuwroReWeight_FlagNorm::NuwroReWeight_FlagNorm()
{
  std::cout << "creating NuwroReWeight_FlagNorm" << std::endl;

  Init();
}

//__________________________________________________________________
nuwro::rew::NuwroReWeight_FlagNorm::~NuwroReWeight_FlagNorm()
{

}

//__________________________________________________________________
void nuwro::rew::NuwroReWeight_FlagNorm::Init(void)
{

  fTwkDial_QEL = 0.;
  fNormDef_QEL = 1.;
  fNormCurr_QEL = 1.;

  fTwkDial_RES = 0.;
  fNormDef_RES = 1.;
  fNormCurr_RES = 1.;

  fTwkDial_DIS = 0.;
  fNormDef_DIS = 1.;
  fNormCurr_DIS = 1.;

  fTwkDial_COH = 0.;
  fNormDef_COH = 1.;
  fNormCurr_COH = 1.;
  
  fTwkDial_MEC = 0.;
  fNormDef_MEC = 1.;
  fNormCurr_MEC = 1.;

  fTwkDial_NC = 0.;
  fNormDef_NC = 1.;
  fNormCurr_NC = 1.;

  fTwkDial_CC = 0.;
  fNormDef_CC = 1.;
  fNormCurr_CC = 1.;

  fTwkDial_ANTINU = 0.;
  fNormDef_ANTINU = 1.;
  fNormCurr_ANTINU = 1.;

  for (int i = 0; i < 10; i++){
    fTwkDial_DYN[i]  = 0.;
    fNormDef_DYN[i]  = 1.;
    fNormCurr_DYN[i] = 1.;
  }
}


//__________________________________________________________________
bool nuwro::rew::NuwroReWeight_FlagNorm::SystIsHandled(NuwroSyst_t syst)
{
  bool handle;

  switch(syst) {
 
  case(kNuwro_QELNorm) :
  case(kNuwro_RESNorm) :
  case(kNuwro_DISNorm) :
  case(kNuwro_COHNorm) :
  case(kNuwro_MECNorm) :
  case(kNuwro_NCNorm) :
  case(kNuwro_CCNorm) :
  case(kNuwro_AntiNuNorm) :

  case(kNuwro_DYNNorm_0) :
  case(kNuwro_DYNNorm_1) :
  case(kNuwro_DYNNorm_2) :
  case(kNuwro_DYNNorm_3) :
  case(kNuwro_DYNNorm_4) :
  case(kNuwro_DYNNorm_5) :
  case(kNuwro_DYNNorm_6) :
  case(kNuwro_DYNNorm_7) :
  case(kNuwro_DYNNorm_8) :
  case(kNuwro_DYNNorm_9) :

    handle = true;
  break;

  default:
    handle = false;
    break;
  }

  return handle;

}


//__________________________________________________________________
void nuwro::rew::NuwroReWeight_FlagNorm::SetSystematic(NuwroSyst_t syst, double val)
{

  switch(syst){
  case (kNuwro_QELNorm): fTwkDial_QEL = val;  break;
  case (kNuwro_RESNorm): fTwkDial_RES = val;  break;
  case (kNuwro_DISNorm): fTwkDial_DIS = val;  break;
  case (kNuwro_COHNorm): fTwkDial_COH = val;  break;
  case (kNuwro_MECNorm): fTwkDial_MEC = val;  break;
  case (kNuwro_CCNorm):  fTwkDial_CC  = val;  break;
  case (kNuwro_NCNorm):  fTwkDial_NC  = val;  break;
  case (kNuwro_AntiNuNorm): fTwkDial_ANTINU = val; break;

  case(kNuwro_DYNNorm_0) :  fTwkDial_DYN[0]  = val; break;
  case(kNuwro_DYNNorm_1) :  fTwkDial_DYN[1]  = val; break;
  case(kNuwro_DYNNorm_2) :  fTwkDial_DYN[2]  = val; break;
  case(kNuwro_DYNNorm_3) :  fTwkDial_DYN[3]  = val; break;
  case(kNuwro_DYNNorm_4) :  fTwkDial_DYN[4]  = val; break;
  case(kNuwro_DYNNorm_5) :  fTwkDial_DYN[5]  = val; break;
  case(kNuwro_DYNNorm_6) :  fTwkDial_DYN[6]  = val; break;
  case(kNuwro_DYNNorm_7) :  fTwkDial_DYN[7]  = val; break;
  case(kNuwro_DYNNorm_8) :  fTwkDial_DYN[8]  = val; break;
  case(kNuwro_DYNNorm_9) :  fTwkDial_DYN[9]  = val; break;

  default: break;
  }

}

//__________________________________________________________________
void nuwro::rew::NuwroReWeight_FlagNorm::Reset(void)
{
  

  this->Reconfigure();
}

//__________________________________________________________________
void nuwro::rew::NuwroReWeight_FlagNorm::Reconfigure(void)
{
  // Get the fractional uncertainty on each dial
  NuwroSystUncertainty * fracerr = NuwroSystUncertainty::Instance();

  // Flags ( Really only the ones being handled should be set.... )
  fNormError_QEL   = fracerr->OneSigmaErr(kNuwro_QELNorm,   fTwkDial_QEL > 0);
  fNormCurr_QEL = fNormDef_QEL * ( 1.0 + fNormError_QEL * fTwkDial_QEL );

  fNormError_RES   = fracerr->OneSigmaErr(kNuwro_RESNorm,   fTwkDial_RES > 0);
  fNormCurr_RES = fNormDef_RES * ( 1.0 + fNormError_RES * fTwkDial_RES );
  
  fNormError_DIS   = fracerr->OneSigmaErr(kNuwro_DISNorm,   fTwkDial_DIS > 0);
  fNormCurr_DIS = fNormDef_DIS * ( 1.0 + fNormError_DIS * fTwkDial_DIS );

  fNormError_COH   = fracerr->OneSigmaErr(kNuwro_COHNorm,   fTwkDial_COH > 0);
  fNormCurr_COH = fNormDef_COH * ( 1.0 + fNormError_COH * fTwkDial_COH );

  fNormError_MEC   = fracerr->OneSigmaErr(kNuwro_MECNorm,   fTwkDial_MEC > 0);
  fNormCurr_MEC = fNormDef_MEC * ( 1.0 + fNormError_MEC * fTwkDial_MEC );

  fNormError_CC   = fracerr->OneSigmaErr(kNuwro_CCNorm,   fTwkDial_CC > 0);
  fNormCurr_CC = fNormDef_CC * ( 1.0 + fNormError_CC * fTwkDial_CC );

  fNormError_NC   = fracerr->OneSigmaErr(kNuwro_NCNorm,   fTwkDial_NC > 0);
  fNormCurr_NC = fNormDef_NC * ( 1.0 + fNormError_NC * fTwkDial_NC );

  fNormError_ANTINU   = fracerr->OneSigmaErr(kNuwro_AntiNuNorm,   fTwkDial_ANTINU > 0);
  fNormCurr_ANTINU = fNormDef_ANTINU * ( 1.0 + fNormError_ANTINU * fTwkDial_ANTINU );

  // Dyn Modes ( should put in loop but CBA... )
  fNormError_DYN[0]  = fracerr->OneSigmaErr(kNuwro_DYNNorm_0,   fTwkDial_DYN[0] > 0);
  fNormError_DYN[1]  = fracerr->OneSigmaErr(kNuwro_DYNNorm_1,   fTwkDial_DYN[1] > 0);
  fNormError_DYN[2]  = fracerr->OneSigmaErr(kNuwro_DYNNorm_2,   fTwkDial_DYN[2] > 0);
  fNormError_DYN[3]  = fracerr->OneSigmaErr(kNuwro_DYNNorm_3,   fTwkDial_DYN[3] > 0);
  fNormError_DYN[4]  = fracerr->OneSigmaErr(kNuwro_DYNNorm_4,   fTwkDial_DYN[4] > 0);
  fNormError_DYN[5]  = fracerr->OneSigmaErr(kNuwro_DYNNorm_5,   fTwkDial_DYN[5] > 0);
  fNormError_DYN[6]  = fracerr->OneSigmaErr(kNuwro_DYNNorm_6,   fTwkDial_DYN[6] > 0);
  fNormError_DYN[7]  = fracerr->OneSigmaErr(kNuwro_DYNNorm_7,   fTwkDial_DYN[7] > 0);
  fNormError_DYN[8]  = fracerr->OneSigmaErr(kNuwro_DYNNorm_8,   fTwkDial_DYN[8] > 0);
  fNormError_DYN[9]  = fracerr->OneSigmaErr(kNuwro_DYNNorm_9,   fTwkDial_DYN[9] > 0);

  // Set DYN Cutt
  for (int i = 0; i < 10; i++){
    fNormCurr_DYN[i]   = fNormDef_DYN[0] * ( 1.0 + fNormError_DYN[i] * fTwkDial_DYN[i] );
  }

}


//__________________________________________________________________
double nuwro::rew::NuwroReWeight_FlagNorm::CalcWeight(event* event)
{

  double weight = 1.;

  // Apply norm scaling for each flag (Can have multiple flags..)

  if (event->flag.qel)  weight *= fNormCurr_QEL;
  if (event->flag.res)  weight *= fNormCurr_RES;
  if (event->flag.dis)  weight *= fNormCurr_DIS;
  if (event->flag.coh)  weight *= fNormCurr_COH;
  if (event->flag.mec)  weight *= fNormCurr_MEC;
  if (event->flag.cc)   weight *= fNormCurr_CC;
  if (event->flag.nc)   weight *= fNormCurr_NC;
  if (event->flag.anty) weight *= fNormCurr_ANTINU;

  // Apply norm scaling for event dyn
  if (fTwkDial_DYN[event->dyn] != 0.0){  weight *= fNormCurr_DYN[event->dyn];  }

  return weight;

}


//__________________________________________________________________
double nuwro::rew::NuwroReWeight_FlagNorm::CalcChisq(void)
{

  double chisq = 0;
  return chisq;

}
