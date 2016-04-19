#include "NuwroReWeight_QEL.h"
#include "NuwroSyst.h"
#include "NuwroSystUncertainty.h"
#include "ff.h"
#include "qel_sigma.h"

#include <iostream>

#include "TFile.h"
#include "TMath.h"
#include "TROOT.h"

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
}

NuwroReWeight_QEL::~NuwroReWeight_QEL() {}

bool NuwroReWeight_QEL::SystIsHandled(NuwroSyst_t syst) {
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
    default: { throw syst; }
  }
  return 0xdeadbeef;
}

void NuwroReWeight_QEL::Reset(void) {
  fTwkDial_MaCCQE = 0;
  this->Reconfigure();
}

void NuwroReWeight_QEL::Reconfigure(void) {
  NuwroSystUncertainty *fracerr = NuwroSystUncertainty::Instance();
  fError_MaCCQE =
      fracerr->OneSigmaErr(kNuwro_Ma_CCQE, (fTwkDial_MaCCQE > 0) ? 1 : -1);
  if (fabs(fTwkDial_MaCCQE) > 1E-8) {
#ifdef DEBUG_QE_REWEIGHT
    std::cout << "[INFO]: NuwroReWeight_QEL::Reconfigure: New MaQE value = "
              << fDef_MaCCQE << " + (" << fError_MaCCQE << "  *  "
              << fTwkDial_MaCCQE << ")" << std::endl;
#endif
  }
  // fCurr_MaCCQE = fDef_MaCCQE * (1 + fError_MaCCQE * fTwkDial_MaCCQE);
  fCurr_MaCCQE = fDef_MaCCQE + fError_MaCCQE * fTwkDial_MaCCQE;

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
}

double GetWghtPropToQEXSec(event const &nuwro_event, params const &rwparams) {
  return GetWghtPropToQEXSec(SRW::SRWEvent(nuwro_event),rwparams);
}
double GetWghtPropToQEXSec(SRW::SRWEvent const &ev, params const &rwparams) {
  ff_configure(rwparams);
  return qel_sigma(ev.NeutrinoEnergy, ev.DynProp.QE.FourMomentumTransfer,
                   ev.DynProp.QE.QELKind, ev.NeutrinoPDG,
                   ev.DynProp.QE.FSLeptonMass, ev.DynProp.QE.NucleonMass);
}

double NuwroReWeight_QEL::CalcWeight(event *nuwro_event) {
  if ((fabs(fTwkDial_MaCCQE) < 1E-8)) {
#ifdef DEBUG_QE_REWEIGHT
    std::cout << "[WARN]: All Dials set very low, short circuiting."
              << std::endl;
#endif

    return 1.0;
  }

  if ((!nuwro_event->flag.qel)) {
#ifdef DEBUG_QE_REWEIGHT
    std::cout << "[WARN]: This is not a QEL event. nwEv.flag.cc: "
              << nuwro_event->flag.cc
              << ", nwEv.flag.qel: " << nuwro_event->flag.qel << std::endl;
#endif
    return 1.0;
  }

  double weight = 1;
  params rwparams = (nuwro_event->par);

  ff_configure(rwparams);

  double oldweight = GetWghtPropToQEXSec((*nuwro_event), rwparams);

  rwparams.qel_cc_axial_mass = fCurr_MaCCQE;
  rwparams.qel_nc_axial_mass = fCurr_MaNCEL;
  rwparams.qel_s_axial_mass = fCurr_MaNCEL_s;
  rwparams.delta_s = fCurr_DeltaS;

  double newweight = GetWghtPropToQEXSec((*nuwro_event), rwparams);

#ifdef DEBUG_QE_REWEIGHT
  std::assert(std::isfinite(oldweight));
  std::assert(std::isfinite(newweight));
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
}
}
