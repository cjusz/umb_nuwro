#include <cassert>
#include <cmath>
#include <iostream>

#include "TFile.h"
#include "TMath.h"
#include "TROOT.h"

#include "ff.h"
#include "qel_sigma.h"

#include "dis/alfa.h"
#include "dis/charge.h"
#include "dis/delta.h"
#include "dis/dis_cr_sec.h"
#include "dis/resevent2.h"
#include "dis/singlepion.h"

#include "reweight/NuwroSystUncertainty.h"

#include "reweight/NuwroReWeight_SPP.h"

// Apparently externing yourself is better than having it in a header...
// go figure
extern double SPP[2][2][2][3][40];

extern "C" {
void shhpythiaitokay_(void);
void youcanspeaknowpythia_(void);
}

namespace nuwro {
namespace rew {

bool NuwroReWeight_SPP::DoSetupSPP = true;

void SetupSPP(params &param) {
  if (true) {  //! CheckSPPSetup()){ -- No way to know in general
    std::cout << "[INFO]: Setting up singlepion tables..." << std::endl;
    shhpythiaitokay_();
    singlepion(param);
    youcanspeaknowpythia_();
    std::cout << "[INFO]: Set up singlepion tables!" << std::endl;
  }
}
void SetupSPP() {
  std::cout << "[WARN]: Setting up singlepion tables with default parameter "
               "set."
            << std::endl;
  params default_p;
  SetupSPP(default_p);
}

NuwroReWeight_SPP::NuwroReWeight_SPP() {
  fTwkDial_MaRES = 0;
  fDef_MaRES = 0.940;
  fCurr_MaRES = fDef_MaRES;

  fTwkDial_CA5 = 0;
  fDef_CA5 = 1.19;
  fCurr_CA5 = fDef_CA5;

  fTwkDial_SPPBkgScale = 0;
  fDef_SPPBkgScale = 0;
  fCurr_SPPBkgScale = fDef_SPPBkgScale;

  if (DoSetupSPP) {
    SetupSPP();
  }
}

NuwroReWeight_SPP::NuwroReWeight_SPP(params const &param) {
  fTwkDial_MaRES = 0;
  fDef_MaRES = param.pion_axial_mass;
  fCurr_MaRES = fDef_MaRES;

  fTwkDial_CA5 = 0;
  fDef_CA5 = param.pion_C5A;
  fCurr_CA5 = fDef_CA5;

  fTwkDial_SPPBkgScale = 0;
  fDef_SPPBkgScale = param.SPPBkgScale;
  fCurr_SPPBkgScale = fDef_SPPBkgScale;

  params nc_param(param);
  if (DoSetupSPP) {
    SetupSPP(nc_param);
  }
}

void NuwroReWeight_SPP::SetDefaults(params const & param) {
  fTwkDial_MaRES = 0;
  fDef_MaRES = param.pion_axial_mass;
  fCurr_MaRES = fDef_MaRES;

  fTwkDial_CA5 = 0;
  fDef_CA5 = param.pion_C5A;
  fCurr_CA5 = fDef_CA5;

  fTwkDial_SPPBkgScale = 0;
  fDef_SPPBkgScale = param.SPPBkgScale;
  fCurr_SPPBkgScale = fDef_SPPBkgScale;
}

NuwroReWeight_SPP::~NuwroReWeight_SPP() {}

bool NuwroReWeight_SPP::SystIsHandled(NuwroSyst_t syst) {
  switch (syst) {
    case kNuwro_MaRES: {
      return true;
    }
    case kNuwro_CA5: {
      return true;
    }
    case kNuwro_SPPBkgScale: {
      return true;
    }
    default: { return false; }
  }
}

void NuwroReWeight_SPP::SetSystematic(NuwroSyst_t syst, double val) {
  if (syst == kNuwro_MaRES) {
    fTwkDial_MaRES = val;
  }
  if (syst == kNuwro_CA5) {
    fTwkDial_CA5 = val;
  }
  if (syst == kNuwro_SPPBkgScale) {
    fTwkDial_SPPBkgScale = val;
  }
}

double NuwroReWeight_SPP::GetSystematic(NuwroSyst_t syst) {
  switch (syst) {
    case kNuwro_MaRES: {
      return fTwkDial_MaRES;
    }
    case kNuwro_CA5: {
      return fTwkDial_CA5;
    }
    case kNuwro_SPPBkgScale: {
      return fTwkDial_SPPBkgScale;
    }
    default: { throw syst; }
  }
}

double NuwroReWeight_SPP::GetSystematicValue(NuwroSyst_t syst) {
  switch (syst) {
    case kNuwro_MaRES: {
      return fCurr_MaRES;
    }
    case kNuwro_CA5: {
      return fCurr_CA5;
    }
    case kNuwro_SPPBkgScale: {
      return fCurr_SPPBkgScale;
    }
    default: { throw syst; }
  }
}

void NuwroReWeight_SPP::Reset(void) {
  fTwkDial_MaRES = 0;
  fTwkDial_CA5 = 0;
  fTwkDial_SPPBkgScale = 0;
  this->Reconfigure();
}

void NuwroReWeight_SPP::Reconfigure(void) {
  NuwroSystUncertainty *fracerr = NuwroSystUncertainty::Instance();
  fError_MaRES =
      fracerr->OneSigmaErr(kNuwro_MaRES, (fTwkDial_MaRES > 0) ? 1 : -1);

  fCurr_MaRES = fDef_MaRES + fDef_MaRES * fError_MaRES * fTwkDial_MaRES;

  fError_CA5 = fracerr->OneSigmaErr(kNuwro_CA5, (fTwkDial_CA5 > 0) ? 1 : -1);

  fCurr_CA5 = fDef_CA5 + fDef_CA5 * fError_CA5 * fTwkDial_CA5;

  fError_SPPBkgScale = fracerr->OneSigmaErr(
      kNuwro_SPPBkgScale, (fTwkDial_SPPBkgScale > 0) ? 1 : -1);

  fCurr_SPPBkgScale =
      fDef_SPPBkgScale + fDef_SPPBkgScale * fError_SPPBkgScale * fTwkDial_SPPBkgScale;
}

double GetEBind(event &nuwro_event, params const &rwparams) {
  switch (rwparams.nucleus_target) {
    case 0: {
      return 0;
    }  // free
    case 1: {
      return rwparams.nucleus_E_b;
    }  // FG
    case 2: {
      return 0;
    }  // local FG
    case 3: {
      return 0;
    }          // Bodek
    case 4: {  // effective SF
      vec ped = nuwro_event.in[1].p();
      return binen(ped, rwparams.nucleus_p, rwparams.nucleus_n);
    }
    case 5: {  // deuterium
      vec ped = nuwro_event.in[1].p();
      return deuter_binen(ped);
    }
    case 6: {
      return rwparams.nucleus_E_b;
    }  // deuterium like Fermi gas
    default: { return 0; }
  }
}

double GetWghtPropToResXSec(event const &nuwro_event, params const &rwparams) {
  return GetWghtPropToResXSec(SRW::SRWEvent(nuwro_event), rwparams);
}
double GetWghtPropToResXSec(SRW::SRWEvent const &ev, params const &rwparams) {
  // see dis/resevent2.cc to see where this calculation is distilled from

  int const &FFSet = rwparams.delta_FF_set;
  double const &delta_axial_mass = rwparams.pion_axial_mass;
  double const &delta_C5A = rwparams.pion_C5A;
  double const &SPPBkg = rwparams.SPPBkgScale;

  double const &Enu = ev.NeutrinoEnergy;
  double const &HadrMass = ev.DynProp.RES.HadronicMass;
  double const &q0 = ev.DynProp.RES.EnergyTransfer;
  int const &NeutrinoPdg = ev.NeutrinoPDG;
  int const &StruckNucleonPdg = ev.DynProp.RES.StruckNucleonPdg;
  bool const &IsCC = ev.IsCC;
  double const &NucMeff = ev.DynProp.RES.NucleonEffectiveMass;
  char const &NOutPart = ev.DynProp.RES.NOutPart;
  int const &SecondOutPartPDG = ev.DynProp.RES.SecondOutPartPDG;
  int const &ThirdOutPartPDG = ev.DynProp.RES.ThirdOutPartPDG;

#ifdef DEBUG_RES_REWEIGHT
  assert(HadrMass == HadrMass);
#endif

  // Check if we have enough Energy.
  double Meff = std::min(NucMeff, (PDG::mass_proton + PDG::mass_neutron) / 2.0);
  double Meff2 = Meff * Meff;
  double leptonMass = lepton_mass(abs(NeutrinoPdg), IsCC);
  if (Enu < ((1080 + leptonMass) * (1080 + leptonMass) - Meff2) / 2 / Meff) {
    std::cout << "[WARN]: COM Enu, " << Enu
              << ", too low to support SPP is this really a RES event?"
              << std::endl;
#ifdef DEBUG_RES_REWEIGHT
    assert(false);
#endif
    return 0;
  }

  if (HadrMass < 1080) {
    std::cout << "[WARN]: HadrMass, " << HadrMass
              << ", too low to support SPP is this really a RES event?"
              << std::endl;
#ifdef DEBUG_RES_REWEIGHT
    assert(false);
#endif
    return 0;
  }

  int j, k, l;
  j = (NeutrinoPdg > 0) ? 0 : 1;
  k = (IsCC) ? 0 : 1;
  l = (StruckNucleonPdg == 2212) ? 0 : 1;

  int finalcharge = charge(StruckNucleonPdg) + (1 - k) * (1 - 2 * j);  // pdg.cc

  double nonspp = 0;
  double dis0 = 0;
  double dis1 = 0;
  double dis2 = 0;

  double fromdis =
      cr_sec_dis(Enu, HadrMass, q0, NeutrinoPdg, StruckNucleonPdg, IsCC);

  if (fromdis < 0) {
    fromdis = 0;
  }

  if (fromdis != fromdis) {
    std::cout << "[WARN]: Invalid DIS contribution: " << fromdis
              << ", is this really a RES event?" << std::endl;
#ifdef DEBUG_RES_REWEIGHT
    assert(false);
#endif
    return 0;
  }

  // No DIS contribution
  if ((HadrMass < 1210) || (fromdis == 0)) {
    double spp0 = SPP[j][k][l][0][0];
    double spp1 = SPP[j][k][l][1][0];
    double spp2 = SPP[j][k][l][2][0];

#ifdef USE_BETADIS
    dis0 = fromdis * spp0 * betadis(j, k, l, 0, HadrMass, SPPBkg);

    dis1 = fromdis * spp1 * betadis(j, k, l, 1, HadrMass, SPPBkg);

    dis2 = fromdis * spp2 * betadis(j, k, l, 2, HadrMass, SPPBkg);
#else
    dis0 = fromdis * spp0 * alfadis(j, k, l, 0, HadrMass);

    dis1 = fromdis * spp1 * alfadis(j, k, l, 1, HadrMass);

    dis2 = fromdis * spp2 * alfadis(j, k, l, 2, HadrMass);
#endif

    double delta0 = 0, delta1 = 0, delta2 = 0;

    double adel0 = alfadelta(j, k, l, 0, HadrMass);

    double adel1 = alfadelta(j, k, l, 1, HadrMass);

    double adel2 = alfadelta(j, k, l, 2, HadrMass);

    switch (finalcharge) {
      case -1: {
        delta0 = 0;
        delta1 = 0;

        delta2 =
            cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass, q0,
                         NeutrinoPdg, StruckNucleonPdg, 2112, -211, IsCC) *
            adel2;
        break;
      }
      case 0: {
        delta0 = 0;

        delta1 =
            cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass, q0,
                         NeutrinoPdg, StruckNucleonPdg, 2112, 111, IsCC) *
            adel1;

        delta2 =
            cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass, q0,
                         NeutrinoPdg, StruckNucleonPdg, 2212, -211, IsCC) *
            adel2;
        break;
      }
      case 1: {
        delta0 =
            cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass, q0,
                         NeutrinoPdg, StruckNucleonPdg, 2112, 211, IsCC) *
            adel0;

        delta1 =
            cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass, q0,
                         NeutrinoPdg, StruckNucleonPdg, 2212, 111, IsCC) *
            adel1;
        delta2 = 0;
        break;
      }
      case 2: {
        delta0 =
            cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass, q0,
                         NeutrinoPdg, StruckNucleonPdg, 2212, 211, IsCC) *
            adel0;
        delta1 = 0;
        delta2 = 0;
        break;
      }
    }
#ifdef DEBUG_RES_REWEIGHT
    double test = (dis0 + dis1 + dis2 + delta0 + delta1 + delta2);
    assert((test == test));
#endif
    return (dis0 + dis1 + dis2 + delta0 + delta1 + delta2);
  } else {  //(HadrMass > 1210) && (fromdis != 0)
    // With DIS contribution
    // This check is effectively the same as for Pythia SPP NPar == 5;
    int OutPionPdg = 0;
    int NPions = 0;
    if ((SecondOutPartPDG == -211) || (SecondOutPartPDG == 111) ||
        (SecondOutPartPDG == 211)) {
      OutPionPdg = SecondOutPartPDG;
      NPions++;
    }
    if ((ThirdOutPartPDG == -211) || (ThirdOutPartPDG == 111) ||
        (ThirdOutPartPDG == 211)) {
      OutPionPdg = ThirdOutPartPDG;
      NPions++;
    }
    bool IsPythiaSPP = ((NOutPart == 3) && OutPionPdg);
    if (IsPythiaSPP && (NPions == 2)) {
      std::cerr << "[ERROR]: Found 2 pions in SPP event::out vector ("
                << "size: " << NOutPart << ")" << std::endl;
      throw std::exception();
    }

    // Event was Single Pion Production
    if (IsPythiaSPP) {
      int t = 0;
      switch (OutPionPdg) {
        case -211: {
          t = 2;
          break;
        }
        case 111: {
          t = 1;
          break;
        }
        case 211: {
          t = 0;
          break;
        }
      }

// DIS contribution
#ifdef USE_BETADIS
      double dis_spp = fromdis * betadis(j, k, l, t, HadrMass, SPPBkg);
#else
      double dis_spp = fromdis * alfadis(j, k, l, t, HadrMass);
#endif
      int StruckNucleonChrg = finalcharge + t - 1;
      int ProducedNucleonPdg = ((StruckNucleonChrg == 1) ? 2212 : 2112);

      double SPPFact = SPPF(j, k, l, t, HadrMass);

      double AlfaFact = alfadelta(j, k, l, t, HadrMass);
#ifdef DEBUG_RES_REWEIGHT
      SPPF(j, k, l, t, HadrMass);
      assert(fabs(SPPFact) > 0);
#endif
      // resonant SPP contribution
      double src_sec_delta_bare = cr_sec_delta(
          FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass, q0, NeutrinoPdg,
          StruckNucleonPdg, ProducedNucleonPdg, OutPionPdg, IsCC);
      double delta_spp = src_sec_delta_bare / SPPFact * AlfaFact;

      if ((rwparams.nucleus_p + rwparams.nucleus_n) > 7) {
        // Pionless delta decay reduction.
        double pdd_red_fact = pdd_red(Enu);
        delta_spp *= pdd_red_fact;
      }
#ifdef DEBUG_RES_REWEIGHT
      double test = (dis_spp + delta_spp);
      if (test != test) {
        std::cout << "[ERROR]: DIS contrib: " << dis_spp
                  << ", Delta contrib:" << delta_spp << std::endl;
      }
      assert((test == test));
#endif
      return (dis_spp + delta_spp);
    } else {  // inelastic pion production and single kaon production
// Event was all pythia just return DIS xsec.
#ifdef DEBUG_RES_REWEIGHT
      assert((fromdis == fromdis));
#endif
      return fromdis;
    }
  }
}

double NuwroReWeight_SPP::CalcWeight(event *nuwro_event) {
  return CalcWeight(SRW::SRWEvent(*nuwro_event), nuwro_event->par);
}

double NuwroReWeight_SPP::CalcWeight(SRW::SRWEvent const &srwev, params const &par) {
  if ((fabs(fTwkDial_MaRES) < 1E-8) && (fabs(fTwkDial_CA5) < 1E-8) &&
      (fabs(fTwkDial_SPPBkgScale) < 1E-8)) {
#ifdef DEBUG_RES_REWEIGHT
    std::cout << "[WARN]: All Dials set very low, short circuiting."
              << std::endl;
#endif
    return 1.0;
  }

  if ((srwev.NuWroDynCode != 2) && (srwev.NuWroDynCode != 3)) {
#ifdef DEBUG_RES_REWEIGHT
    std::cout << "[WARN]: This is not a SPP event. srwev.NuWroDynCode: "
              << srwev.NuWroDynCode << std::endl;
#endif
    return 1.0;
  }

  double weight = 1;
  params rwparams = par;

  if(srwev.CacheWeight == 0xdeadbeef){
    srwev.CacheWeight = GetWghtPropToResXSec(srwev, rwparams);
  }

#ifdef DEBUG_RES_REWEIGHT
  std::cout << "[INFO]: Reweighting nominal MaRES: { "
            << rwparams.pion_axial_mass << " -> " << fCurr_MaRES
            << " } and CA5: {" << rwparams.pion_C5A << " -> " << fCurr_CA5
            << " } and SPP DIS Background scale: {" << rwparams.SPPBkgScale
            << " -> " << fCurr_SPPBkgScale << "}" << std::endl;
#endif
  rwparams.pion_axial_mass = fCurr_MaRES;
  rwparams.pion_C5A = fCurr_CA5;
  rwparams.SPPBkgScale = fCurr_SPPBkgScale;

  double newweight = GetWghtPropToResXSec(srwev, rwparams);

  weight *= newweight / srwev.CacheWeight;
#ifdef DEBUG_RES_REWEIGHT
  std::cout << "[INFO]: Returning event weight of " << weight << " = "
            << newweight << "/" << srwev.CacheWeight << std::endl;
  assert(std::isfinite(weight));
#endif

  return weight;
}

double NuwroReWeight_SPP::CalcChisq(void) {
  double chisq = 0;
  return chisq;
}
}
}
