#include "NuwroReWeight_MECNorm.h"

#include "NuwroSystUncertainty.h"

#include <iostream>

#include "TFile.h"
#include "TROOT.h"
#include "TMath.h"

//__________________________________________________________________
nuwro::rew::NuwroReWeight_MECNorm::NuwroReWeight_MECNorm()
{
  std::cout << "creating NuwroReWeight_MECNorm" << std::endl;

  Init();
}

//__________________________________________________________________
nuwro::rew::NuwroReWeight_MECNorm::~NuwroReWeight_MECNorm()
{

}

//__________________________________________________________________
void nuwro::rew::NuwroReWeight_MECNorm::Init(void)
{

  fTwkDial_MEC = 0.;
  fNormDef_MEC = 0.;
  fNormCurr_MEC = 0.;

}


//__________________________________________________________________
bool nuwro::rew::NuwroReWeight_MECNorm::SystIsHandled(NuwroSyst_t syst)
{
  bool handle;

  switch(syst) {
    case(kNuwro_MECNorm) :
      handle = true;
      break;

  default:
    handle = false;
    break;
  }

  return handle;

}


//__________________________________________________________________
void nuwro::rew::NuwroReWeight_MECNorm::SetSystematic(NuwroSyst_t syst, double val)
{
  if(syst == kNuwro_MECNorm)   fTwkDial_MEC   = val;

}

//__________________________________________________________________
void nuwro::rew::NuwroReWeight_MECNorm::Reset(void)
{
  fTwkDial_MEC   = 0.;

  this->Reconfigure();
}

//__________________________________________________________________
void nuwro::rew::NuwroReWeight_MECNorm::Reconfigure(void)
{
  // Get the fractional uncertainty on each dial
  NuwroSystUncertainty * fracerr = NuwroSystUncertainty::Instance();

  fNormError_MEC   = fracerr->OneSigmaErr(kNuwro_MECNorm,   fTwkDial_MEC > 0);

  fNormCurr_MEC = fNormDef_MEC * ( 1.0 + fNormError_MEC * fTwkDial_MEC );

}


//__________________________________________________________________
double nuwro::rew::NuwroReWeight_MECNorm::CalcWeight(const event & event)
{

  double weight = 1;
  if (event.flag.mec) weight *= fNormCurr_MEC;

  return weight;

}


//__________________________________________________________________
double nuwro::rew::NuwroReWeight_MECNorm::CalcChisq(void)
{

  double chisq = 0;
  return chisq;

}
