#include <cassert>
#include <iostream>

#include "TFile.h"
#include "TMath.h"
#include "TROOT.h"

#include "NuwroReWeight_QEL.h"
#include "NuwroSyst.h"
#include "NuwroSystUncertainty.h"
#include "ff.h"
#include "qel_sigma.h"

namespace nuwro {
namespace rew {

NuwroReWeight_QEL::NuwroReWeight_QEL() {
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

  // 2 Comp Parameters                           
  fTwkDial_axial_2comp_alpha = 0.0;
  fDef_axial_2comp_alpha = 1.0;
  fCurr_axial_2comp_alpha = fTwkDial_axial_2comp_alpha;

  fTwkDial_axial_2comp_gamma = 0.0;
  fDef_axial_2comp_gamma = 0.515;
  fCurr_axial_2comp_gamma = fTwkDial_axial_2comp_gamma;

  // 3 Comp Parameters
  fTwkDial_axial_3comp_theta = 0.0;
  fDef_axial_3comp_theta = 0.15;
  fCurr_axial_3comp_theta = fTwkDial_axial_3comp_theta;

  fTwkDial_axial_3comp_beta = 0.0;
  fDef_axial_3comp_beta = 2.0;
  fCurr_axial_3comp_beta = fTwkDial_axial_3comp_beta;

  // Sort many BBBA07FF dials
  ResetBBBA07();

}

NuwroReWeight_QEL::NuwroReWeight_QEL(params const &param) {
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
  
  // 2 Comp Parameters                                                                                                                                                           
  fTwkDial_axial_2comp_alpha = 0.0;
  fDef_axial_2comp_alpha = param.qel_axial_2comp_alpha;
  fCurr_axial_2comp_alpha = fTwkDial_axial_2comp_alpha;

  fTwkDial_axial_2comp_gamma = 0.0;
  fDef_axial_2comp_gamma = param.qel_axial_2comp_gamma;
  fCurr_axial_2comp_gamma = fTwkDial_axial_2comp_gamma;

  // 3 Comp Parameters 
  fTwkDial_axial_3comp_theta = 0.0;
  fDef_axial_3comp_theta = param.qel_axial_3comp_theta;
  fCurr_axial_3comp_theta = fTwkDial_axial_3comp_theta;

  fTwkDial_axial_3comp_beta = 0.0;
  fDef_axial_3comp_beta = param.qel_axial_3comp_beta;
  fCurr_axial_3comp_beta = fTwkDial_axial_3comp_beta;

}

NuwroReWeight_QEL::~NuwroReWeight_QEL() {}

bool NuwroReWeight_QEL::SystIsHandled(NuwroSyst_t syst) {

  for (int i = 0; i < 7; i++) {
    if (syst == kNuwro_BBBA07_AEp1 + i) return true;
    if (syst == kNuwro_BBBA07_AMp1 + i) return true;
    if (syst == kNuwro_BBBA07_AEn1 + i) return true;
    if (syst == kNuwro_BBBA07_AMn1 + i) return true;
    if (syst == kNuwro_BBBA07_AAx1 + i) return true;
  }

  switch (syst) {
    case kNuwro_Ma_CCQE: {
      return true;
    }
    case kNuwro_Ma_NCEL: {
      return true;
    }
    case kNuwro_SMa_NCEL: {
      return true;
    }
    case kNuwro_DeltaS_NCEL: {
      return true;
    }
    case kNuwro_Axl2comp_alpha:{
      return true;
    }
    case kNuwro_Axl2comp_gamma:{
      return true;
    }
    case kNuwro_Axl3comp_theta:{
      return true;
    }
    case kNuwro_Axl3comp_beta:{
      return true;
    }
    default: { return false; }
  }
}

void NuwroReWeight_QEL::SetSystematic(NuwroSyst_t syst, double val) {
  if (syst == kNuwro_Ma_CCQE) {
    fTwkDial_MaCCQE = val;
  }
  if (syst == kNuwro_Ma_NCEL) {
    fTwkDial_MaNCEL = val;
  }
  if (syst == kNuwro_SMa_NCEL) {
    fTwkDial_MaNCEL_s = val;
  }
  if (syst == kNuwro_DeltaS_NCEL) {
    fTwkDial_DeltaS = val;
  }
  if (syst == kNuwro_Axl2comp_alpha){
    fTwkDial_axial_2comp_alpha = val;
  }
  if (syst == kNuwro_Axl2comp_gamma){
    fTwkDial_axial_2comp_gamma = val;
  }
  if (syst == kNuwro_Axl3comp_theta){
    fTwkDial_axial_3comp_theta = val;
  }
  if (syst == kNuwro_Axl3comp_beta){
    fTwkDial_axial_3comp_beta = val;
  }
  
  
 for (int i = 0; i < 7; i++) {
    if (syst == kNuwro_BBBA07_AEp1 + i) {
      p_AEp_twk[i] = val;
      return;
    }
  }

  for (int i = 0; i < 7; i++) {
    if (syst == kNuwro_BBBA07_AMp1 + i) {
      p_AMp_twk[i] = val;
      return;
    }
  }

  for (int i = 0; i < 7; i++) {
    if (syst == kNuwro_BBBA07_AEn1 + i) {
      p_AEn_twk[i] = val;
      return;
    }
  }

  for (int i = 0; i < 7; i++) {
    if (syst == kNuwro_BBBA07_AMn1 + i) {
      p_AMn_twk[i] = val;
      return;
    }
  }
  
  for (int i = 0; i < 7; i++) {
    if (syst == kNuwro_BBBA07_AAx1 + i) {
      p_AAx_twk[i] = val;
      return;
    }
  }
}

double NuwroReWeight_QEL::GetSystematic(NuwroSyst_t syst) {
  switch (syst) {
    case kNuwro_Ma_CCQE: {
      return fTwkDial_MaCCQE;
    }
    case kNuwro_Ma_NCEL: {
      return fTwkDial_MaNCEL;
    }
    case kNuwro_SMa_NCEL: {
      return fTwkDial_MaNCEL_s;
    }
    case kNuwro_DeltaS_NCEL: {
      return fTwkDial_DeltaS;
    }
    case kNuwro_Axl2comp_alpha: {
      return fTwkDial_axial_2comp_alpha;
    }
    case kNuwro_Axl2comp_gamma: {
      return fTwkDial_axial_2comp_gamma;
    }
    case kNuwro_Axl3comp_beta: {
      return fTwkDial_axial_3comp_beta;
    }
    case kNuwro_Axl3comp_theta: {
      return fTwkDial_axial_3comp_theta;
    }
    default: { throw syst; }
  }
  return 0xdeadbeef;
}

double NuwroReWeight_QEL::GetSystematicValue(NuwroSyst_t syst) {
  switch (syst) {
    case kNuwro_Ma_CCQE: {
      return fCurr_MaCCQE;
    }
    case kNuwro_Ma_NCEL: {
      return fCurr_MaNCEL;
    }
    case kNuwro_SMa_NCEL: {
      return fCurr_MaNCEL_s;
    }
    case kNuwro_DeltaS_NCEL: {
      return fCurr_DeltaS;
    }
    case kNuwro_Axl2comp_alpha: {
      return fCurr_axial_2comp_alpha;
    }
    case kNuwro_Axl2comp_gamma: {
      return fCurr_axial_2comp_gamma;
    }
    case kNuwro_Axl3comp_beta: {
      return fCurr_axial_3comp_beta;
    }
    case kNuwro_Axl3comp_theta: {
      return fCurr_axial_3comp_theta;
    }
    default: { throw syst; }
  }
  return 0xdeadbeef;
}

void NuwroReWeight_QEL::Reset(void) {

  fTwkDial_MaCCQE = 0;
  fTwkDial_MaNCEL = 0;
  fTwkDial_MaNCEL_s = 0;
  fTwkDial_DeltaS = 0;

  fTwkDial_axial_2comp_alpha = 0;
  fTwkDial_axial_2comp_gamma = 0;
  fTwkDial_axial_3comp_beta = 0;
  fTwkDial_axial_3comp_theta = 0;

  this->Reconfigure();
}

void NuwroReWeight_QEL::Reconfigure(void) {
  NuwroSystUncertainty *fracerr = NuwroSystUncertainty::Instance();

  fError_MaCCQE =
      fracerr->OneSigmaErr(kNuwro_Ma_CCQE, (fTwkDial_MaCCQE > 0) ? 1 : -1);
  fCurr_MaCCQE = fDef_MaCCQE * (1.0 + fError_MaCCQE * fTwkDial_MaCCQE);

  // Set NCEL Dials
  fError_MaNCEL =
      fracerr->OneSigmaErr(kNuwro_Ma_NCEL, (fTwkDial_MaNCEL > 0) ? 1 : -1);
  fCurr_MaNCEL = fDef_MaNCEL * (1 + fError_MaNCEL * fTwkDial_MaNCEL);

  fError_MaNCEL_s =
      fracerr->OneSigmaErr(kNuwro_SMa_NCEL, (fTwkDial_MaNCEL_s > 0) ? 1 : -1);
  fCurr_MaNCEL_s = fDef_MaNCEL_s * (1 + fError_MaNCEL_s * fTwkDial_MaNCEL_s);

  fError_DeltaS =
      fracerr->OneSigmaErr(kNuwro_DeltaS_NCEL, (fTwkDial_DeltaS > 0) ? 1 : -1);
  fCurr_DeltaS = fDef_DeltaS * (1 + fError_DeltaS * fTwkDial_DeltaS);


  // Set 2 Comp Dials
  fError_axial_2comp_alpha = fracerr->OneSigmaErr(kNuwro_Axl2comp_alpha, 
						  (fTwkDial_axial_2comp_alpha > 0) ? 1 : -1);
  fCurr_axial_2comp_alpha = fDef_axial_2comp_alpha * 
    (1 + fError_axial_2comp_alpha * fTwkDial_axial_2comp_alpha);

  fError_axial_2comp_gamma = fracerr->OneSigmaErr(kNuwro_Axl2comp_gamma,
						  (fTwkDial_axial_2comp_gamma > 0) ? 1 : -1);
  fCurr_axial_2comp_gamma = fDef_axial_2comp_gamma *
    (1 + fError_axial_2comp_gamma * fTwkDial_axial_2comp_gamma);

  // 3 Comp Dials
  fError_axial_3comp_beta = fracerr->OneSigmaErr(kNuwro_Axl3comp_beta,
						 (fTwkDial_axial_3comp_beta > 0) ? 1 : -1);
  fCurr_axial_3comp_beta = fDef_axial_3comp_beta *
    (1 + fError_axial_3comp_beta * fTwkDial_axial_3comp_beta);

  fError_axial_3comp_theta = fracerr->OneSigmaErr(kNuwro_Axl3comp_theta,
                                                 (fTwkDial_axial_3comp_theta > 0) ? 1 : -1);
  fCurr_axial_3comp_theta = fDef_axial_3comp_theta *
    (1 + fError_axial_3comp_theta * fTwkDial_axial_3comp_theta);
  

#ifdef DEBUG_QE_REWEIGHT
  if ((fabs(fTwkDial_MaCCQE) > 1E-8) || (fabs(fTwkDial_MaNCEL) > 1E-8) ||
      (fabs(fTwkDial_MaNCEL_s) > 1E-8) || (fabs(fTwkDial_DeltaS) > 1E-8)) {
    std::cout << "[INFO]: NuwroReWeight_QEL::Reconfigure:"
              << "\n\tNew MaCCQE value = " << fDef_MaCCQE << " * (1.0 + "
              << fError_MaCCQE << "  *  " << fTwkDial_MaCCQE << ")"
              << "\n\tNew MaNCEL value = " << fDef_MaNCEL << " * (1.0 + "
              << fError_MaNCEL << "  *  " << fTwkDial_MaNCEL << ")"
              << "\n\tNew MaNCEL_s value = " << fDef_MaNCEL_s << " * (1.0 + "
              << fError_MaNCEL_s << "  *  " << fTwkDial_MaNCEL_s << ")"
              << "\n\tNew Delta_s value = " << fDef_DeltaS << " * (1.0 + "
              << fError_DeltaS << "  *  " << fTwkDial_DeltaS << ")"
              << std::endl;
  }
#endif
}

double GetWghtPropToQEXSec(event const &nuwro_event, params const &rwparams) {
  return GetWghtPropToQEXSec(SRW::SRWEvent(nuwro_event), rwparams);
}
double GetWghtPropToQEXSec(SRW::SRWEvent const &ev, params const &rwparams) {
  ff_configure(rwparams);
  return qel_sigma(ev.NeutrinoEnergy, ev.DynProp.QE.FourMomentumTransfer,
                   ev.DynProp.QE.QELKind, (ev.NeutrinoPDG < 0),
                   ev.DynProp.QE.FSLeptonMass, ev.DynProp.QE.NucleonMass);
}

double NuwroReWeight_QEL::CalcWeight(event *nuwro_event) {
  return CalcWeight(SRW::SRWEvent(*nuwro_event), nuwro_event->par);
}

double NuwroReWeight_QEL::CalcWeight(SRW::SRWEvent const &srwev,
                                     params const &par) {
  if ((fabs(fTwkDial_MaCCQE) < 1E-8) && (fabs(fTwkDial_MaNCEL) < 1E-8) &&
      (fabs(fTwkDial_MaNCEL_s) < 1E-8) && (fabs(fTwkDial_DeltaS) < 1E-8) &&
      (fabs(fTwkDial_axial_3comp_theta) < 1E-8) && 
      (fabs(fTwkDial_axial_3comp_beta) < 1E-8) && 
      (fabs(fTwkDial_axial_2comp_alpha) < 1E-8) && 
      (fabs(fTwkDial_axial_2comp_gamma) < 1E-8)) {
#ifdef DEBUG_QE_REWEIGHT
    std::cout << "[WARN]: All Dials set very low, short circuiting."
              << std::endl;
#endif

    return 1.0;
  }

  if (srwev.NuWroDynCode > 1) {
#ifdef DEBUG_QE_REWEIGHT
    std::cout << "[WARN]: This is not a QEL event. srwev.NuWroDynCode: "
              << srwev.NuWroDynCode << std::endl;
#endif
    return 1.0;
  }

  double weight = 1;
  params rwparams = par;

  ff_configure(rwparams);

  double oldweight = GetWghtPropToQEXSec(srwev, rwparams);

#ifdef DEBUG_QE_REWEIGHT
  std::cout << "[INFO]: QEL Param changes: "
            << "qel_cc_axial_mass: " << rwparams.qel_cc_axial_mass << " -> "
            << fCurr_MaCCQE
            << ", qel_nc_axial_mass: " << rwparams.qel_nc_axial_mass << " -> "
            << fCurr_MaNCEL
            << ", qel_s_axial_mass: " << rwparams.qel_s_axial_mass << " -> "
            << fCurr_MaNCEL_s << ", delta_s: " << rwparams.delta_s << " -> "
            << fCurr_DeltaS << std::endl;
#endif

  rwparams.qel_cc_axial_mass = fCurr_MaCCQE;
  rwparams.qel_nc_axial_mass = fCurr_MaNCEL;
  rwparams.qel_s_axial_mass = fCurr_MaNCEL_s;
  rwparams.delta_s = fCurr_DeltaS;

  rwparams.qel_axial_2comp_alpha = fCurr_axial_2comp_alpha;
  rwparams.qel_axial_2comp_gamma = fCurr_axial_2comp_gamma;
  rwparams.qel_axial_3comp_beta  = fCurr_axial_3comp_beta;
  rwparams.qel_axial_3comp_theta = fCurr_axial_3comp_theta;

  double newweight = GetWghtPropToQEXSec(srwev, rwparams);

#ifdef DEBUG_QE_REWEIGHT
  assert(std::isfinite(oldweight));
  assert(std::isfinite(newweight));
#endif

  weight *= newweight / oldweight;
#ifdef DEBUG_QE_REWEIGHT
  std::cout << "Returning event weight of " << weight << std::endl;
#endif

  return weight;
}

double NuwroReWeight_QEL::CalcChisq(void) {
  double chisq = 0;
  return chisq;
}


void NuwroReWeight_QEL::ResetBBBA07(void){

  // Setup Default Params
  params newpars;

  // Set New Dials
  p_AEp_def[0] = newpars.bba07_AEp1;
  p_AEp_def[1] = newpars.bba07_AEp2;
  p_AEp_def[2] = newpars.bba07_AEp3;
  p_AEp_def[3] = newpars.bba07_AEp4;
  p_AEp_def[4] = newpars.bba07_AEp5;
  p_AEp_def[5] = newpars.bba07_AEp6;
  p_AEp_def[6] = newpars.bba07_AEp7;

  p_AMp_def[0] = newpars.bba07_AMp1;
  p_AMp_def[1] = newpars.bba07_AMp2;
  p_AMp_def[2] = newpars.bba07_AMp3;
  p_AMp_def[3] = newpars.bba07_AMp4;
  p_AMp_def[4] = newpars.bba07_AMp5;
  p_AMp_def[5] = newpars.bba07_AMp6;
  p_AMp_def[6] = newpars.bba07_AMp7;

  p_AEn_def[0] = newpars.bba07_AEn1;
  p_AEn_def[1] = newpars.bba07_AEn2;
  p_AEn_def[2] = newpars.bba07_AEn3;
  p_AEn_def[3] = newpars.bba07_AEn4;
  p_AEn_def[4] = newpars.bba07_AEn5;
  p_AEn_def[5] = newpars.bba07_AEn6;
  p_AEn_def[6] = newpars.bba07_AEn7;

  p_AMn_def[0] = newpars.bba07_AMn1;
  p_AMn_def[1] = newpars.bba07_AMn2;
  p_AMn_def[2] = newpars.bba07_AMn3;
  p_AMn_def[3] = newpars.bba07_AMn4;
  p_AMn_def[4] = newpars.bba07_AMn5;
  p_AMn_def[5] = newpars.bba07_AMn6;
  p_AMn_def[6] = newpars.bba07_AMn7;

  p_AAx_def[0] = newpars.bba07_AAx1;
  p_AAx_def[1] = newpars.bba07_AAx2;
  p_AAx_def[2] = newpars.bba07_AAx3;
  p_AAx_def[3] = newpars.bba07_AAx4;
  p_AAx_def[4] = newpars.bba07_AAx5;
  p_AAx_def[5] = newpars.bba07_AAx6;
  p_AAx_def[6] = newpars.bba07_AAx7;

  for (int i = 0; i < 7; i++) {
    p_AEp_twk[i] = 0.0;
    p_AEp[i] = p_AEp_def[i];

    p_AMp_twk[i] = 0.0;
    p_AMp[i] = p_AMp_def[i];

    p_AEn_twk[i] = 0.0;
    p_AEn[i] = p_AEn_def[i];

    p_AMn_twk[i] = 0.0;
    p_AMn[i] = p_AMn_def[i];
    
    p_AAx_twk[i] = 0.0;
    p_AAx[i] = p_AAx_def[i];
  }
  return;
}
}
}
