#include "NuwroReWeight_QEL.h"
#include "NuwroSyst.h"
#include "NuwroSystUncertainty.h"
#include "ff.h"
#include "qel_sigma.h"

#include <iostream>

#include "TFile.h"
#include "TROOT.h"
#include "TMath.h"

using namespace nuwro::rew;

NuwroReWeight_QEL::NuwroReWeight_QEL(){
  fTwkDial_MaCCQE = 0;
  fDef_MaCCQE = 1200;
  fCurr_MaCCQE = fDef_MaCCQE;

  fTwkDial_MaNCEL = 0;
  fDef_MaNCEL = 1350;
  fCurr_MaNCEL = fDef_MaNCEL;

  fTwkDial_MaNCEL_s = 0;
  fDef_MaNCEL_s = 1200;
  fCurr_MaNCEL_s = fDef_MaNCEL_s;

  fTwkDial_DeltaS = 0;
  fDef_DeltaS = -0.15;
  fCurr_DeltaS = fDef_DeltaS;
}

NuwroReWeight_QEL::NuwroReWeight_QEL(params const & param){
  fTwkDial_MaCCQE = 0;
  fDef_MaCCQE = param.qel_cc_axial_mass;
  fCurr_MaCCQE = fDef_MaCCQE;

  fTwkDial_MaNCEL = 0;
  fDef_MaNCEL = param.qel_nc_axial_mass;
  fCurr_MaNCEL = fDef_MaNCEL;

  fTwkDial_MaNCEL_s = 0;
  fDef_MaNCEL_s = param.qel_s_axial_mass;
  fCurr_MaNCEL_s = fDef_MaNCEL_s;

  fTwkDial_DeltaS = 0;
  fDef_DeltaS = param.delta_s;
  fCurr_DeltaS = fDef_DeltaS;

}

NuwroReWeight_QEL::~NuwroReWeight_QEL(){}

bool NuwroReWeight_QEL::SystIsHandled(NuwroSyst_t syst){

  switch(syst){
  case kNuwro_Ma_CCQE: { return true; }
  case kNuwro_Ma_NCEL: { return true; }
  case kNuwro_SMa_NCEL: { return true; }
  case kNuwro_DeltaS_NCEL: { return true; }
  default: { return false; }
  }
}

void NuwroReWeight_QEL::SetSystematic(NuwroSyst_t syst, double val){
  if(syst == kNuwro_Ma_CCQE)    { fTwkDial_MaCCQE   = val; }
  if(syst == kNuwro_Ma_NCEL)    { fTwkDial_MaNCEL   = val; }
  if(syst == kNuwro_SMa_NCEL)   { fTwkDial_MaNCEL_s = val; }
  if(syst == kNuwro_DeltaS_NCEL){ fTwkDial_DeltaS   = val; }
}

void NuwroReWeight_QEL::Reset(void){
  fTwkDial_MaCCQE = 0;
  this->Reconfigure();
}

void NuwroReWeight_QEL::Reconfigure(void){

  NuwroSystUncertainty * fracerr = NuwroSystUncertainty::Instance();
  fError_MaCCQE = fracerr->OneSigmaErr(kNuwro_Ma_CCQE,
    (fTwkDial_MaCCQE > 0)?1:-1);
  if(fabs(fTwkDial_MaCCQE) > 1E-8){
#ifdef DEBUG_QE_REWEIGHT
    std::cout << "[INFO]: NuwroReWeight_QEL::Reconfigure: New MaQE value = "
      << fDef_MaCCQE << " + (" << fError_MaCCQE << "  *  " << fTwkDial_MaCCQE
      << ")" << std::endl;
#endif
  }
  // fCurr_MaCCQE = fDef_MaCCQE * (1 + fError_MaCCQE * fTwkDial_MaCCQE);
  fCurr_MaCCQE = fDef_MaCCQE + fError_MaCCQE * fTwkDial_MaCCQE;

  // Set NCEL Dials
  fError_MaNCEL = fracerr->OneSigmaErr(kNuwro_Ma_NCEL, (fTwkDial_MaNCEL > 0)?1:-1);
  fCurr_MaNCEL  = fDef_MaNCEL * (1 + fError_MaNCEL * fTwkDial_MaNCEL);

  fError_MaNCEL_s = fracerr->OneSigmaErr(kNuwro_SMa_NCEL, (fTwkDial_MaNCEL_s > 0)?1:-1);
  fCurr_MaNCEL_s = fDef_MaNCEL_s * (1 + fError_MaNCEL_s * fTwkDial_MaNCEL_s);

  fError_DeltaS = fracerr->OneSigmaErr(kNuwro_DeltaS_NCEL, (fTwkDial_DeltaS > 0)?1:-1);
  fCurr_DeltaS = fDef_DeltaS * (1 + fError_DeltaS * fTwkDial_DeltaS);

}


double NuwroReWeight_QEL::CalcWeight(event* nuwro_event){

  if ((fabs(fTwkDial_MaCCQE) < 1E-8)){

#ifdef DEBUG_QE_REWEIGHT
    std::cout << "[WARN]: All Dials set very low, short circuiting."
      << std::endl;
#endif

    return 1.0;
  }

  if ((!nuwro_event->flag.qel)){
#ifdef DEBUG_QE_REWEIGHT
    std::cout << "[WARN]: This is not a QEL event. nwEv.flag.cc: "
      << nuwro_event->flag.cc << ", nwEv.flag.qel: " << nuwro_event->flag.qel
      << std::endl;
#endif
    return 1.0;
  }

  double weight = 1;
  params rwparams = (nuwro_event->par);

  ff_configure(rwparams);

  double E = nuwro_event->E();
  double q2 = nuwro_event->q2();
  double m_lep = nuwro_event->out[0].mass();
  double m_nuc = nuwro_event->N0().mass();
  bool pdg_neut = nuwro_event->nu().pdg < 0;
  double oldweight = qel_sigma( E, q2, 0, pdg_neut, m_lep, m_nuc );

  rwparams.qel_cc_axial_mass = fCurr_MaCCQE;
  rwparams.qel_nc_axial_mass = fCurr_MaNCEL;
  rwparams.qel_s_axial_mass  = fCurr_MaNCEL_s;
  rwparams.delta_s = fCurr_DeltaS;

  ff_configure(rwparams);

  double newweight = qel_sigma( E, q2, 0, pdg_neut, m_lep, m_nuc );

  weight *= newweight/oldweight;
#ifdef DEBUG_QE_REWEIGHT
  std::cout << "Returning event weight of " << weight << std::endl;
#endif

  return weight;
}

double NuwroReWeight_QEL::CalcChisq(void){
  double chisq = 0;
  return chisq;
}
