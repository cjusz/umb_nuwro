#include <iostream>

#include "TFile.h"
#include "TROOT.h"
#include "TMath.h"

#include "ff.h"
#include "qel_sigma.h"

#include "dis/alfa.h"
#include "dis/dis_cr_sec.h"
#include "dis/charge.h"
#include "dis/delta.h"

#include "reweight/NuwroSystUncertainty.h"

#include "reweight/NuwroReWeight_MaRES_CA5.h"

using namespace nuwro::rew;
using namespace RooTrackerUtils;

NuwroReWeight_RES_CA5::NuwroReWeight_RES_CA5(){
  fTwkDial_MaRES = 0.;
  fDef_MaRES = 1200.;
  fCurr_MaRES = fDef_MaRES;

  fTwkDial_CA5 = 0.;
  fDef_CA5 = 1200.;
  fCurr_CA5 = fDef_CA5;
}

NuwroReWeight_RES_CA5::~NuwroReWeight_RES_CA5(){}

bool NuwroReWeight_RES_CA5::SystIsHandled(NuwroSyst_t syst){
  switch(syst){
    case kNuwro_MaRES: { return true; }
    case kNuwro_CA5: { return true; }
    default: { return false; }
  }
}

void NuwroReWeight_RES_CA5::SetSystematic(NuwroSyst_t syst, double val){
  if(syst == kNuwro_MaRES){ fTwkDial_MaRES = val; }
  if(syst == kNuwro_CA5){ fTwkDial_CA5 = val; }
}

void NuwroReWeight_RES_CA5::Reset(void){
  fTwkDial_MaRES = 0;
  fTwkDial_CA5 = 0;
  this->Reconfigure();
}

void NuwroReWeight_RES_CA5::Reconfigure(void){

  NuwroSystUncertainty * fracerr = NuwroSystUncertainty::Instance();
  fError_MaRES = fracerr->OneSigmaErr(kNuwro_MaRES,
    (fTwkDial_MaRES > 0)?1:-1);
  std::cout << "[INFO]: NuwroReWeight_RES_CA5::Reconfigure: New MaRES value = "
    << fDef_MaRES << " + (" << fError_MaRES << "  *  " << fTwkDial_MaRES
    << ")" << std::endl;
  fCurr_MaRES = fDef_MaRES + fError_MaRES * fTwkDial_MaRES;

  fError_CA5 = fracerr->OneSigmaErr(kNuwro_CA5,
    (fTwkDial_CA5 > 0)?1:-1);
  std::cout << "[INFO]: NuwroReWeight_RES_CA5::Reconfigure: New CA5 value = "
    << fDef_CA5 << " + (" << fError_CA5 << "  *  " << fTwkDial_CA5
    << ")" << std::endl;
  fCurr_CA5 = fDef_CA5 + fError_CA5 * fTwkDial_CA5;
}


double GetWghtPropToResXSec(int FFSet, double delta_axial_mass,
  double delta_C5A, double Enu, double HadrMass, double q0, int NeutrinoPdg,
  int StruckNucleonPdg, bool IsCC){
/*
  //see dis/resevent2.cc to see where this calculation is distilled from

  int j, k, l, t;
  j = (NeutrinoPdg > 0) ? 0 : 1;
  k = (IsCC) ? 0 : 1;
  l = (StruckNucleonPdg == 2212) ? 0 : 1;
  int finalcharge = charge(StruckNucleonPdg) + (1 - k) * (1 - 2 * j); //pdg.cc

  double nonspp = 0;
  double dis0 = 0;
  double dis1 = 0;
  double dis2 = 0;

  double fromdis = cr_sec_dis(Enu, HadrMass, q0, NeutrinoPdg,
    StruckNucleonPdg, current);

  if(fromdis < 0){
    fromdis = 0;
  }

  //No DIS contribution
  if( (HadrMass < 1210) || (fromdis == 0) ){
    double spp0 = SPP[j][k][l][0][0];
    double spp1 = SPP[j][k][l][1][0];
    double spp2 = SPP[j][k][l][2][0];

    dis0 = fromdis * spp0 * alfadis(j, k, l, 0, HadrMass);
    dis1 = fromdis * spp1 * alfadis(j, k, l, 1, HadrMass);
    dis2 = fromdis * spp2 * alfadis(j, k, l, 2, HadrMass);

    double delta0, delta1, delta2;

    double adel0 = alfadelta(j, k, l, 0, HadrMass);
    double adel1 = alfadelta(j, k, l, 1, HadrMass);
    double adel2 = alfadelta(j, k, l, 2, HadrMass);

    switch(finalcharge){
      case -1: {
        delta0 = 0;
        delta1 = 0;
        delta2 = cr_sec_delta(FFset, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2112, -211, IsCC) * adel2;
        break;
      }
      case 0: {
        delta0 = 0;
        delta1 = cr_sec_delta(FFset, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2112, 111, IsCC) * adel1;
        delta2 = cr_sec_delta(FFset, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2212, -211, IsCC) * adel2;
        break;
      }
      case 1: {
        delta0 = cr_sec_delta(FFset, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2112, 211, IsCC) * adel0;
        delta1 = cr_sec_delta(FFset, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2212, 111, IsCC) * adel1;
        delta2 = 0;
        break;
      }
      case 2: {
        delta0 = cr_sec_delta(FFset, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2212, 211, IsCC) * adel0;
        delta1 = 0;
        delta2 = 0;
        break;
      }
    }

    return (dis0 + dis1 + dis2 + delta0 + delta1 + delta2);
  } else { //(HadrMass > 1210) && (fromdis != 0)
    //With DIS contribution

    //Event was Single Pion Production
    if(IsPythiaSPP){

      int t = 0;
      int pionPdg = 0;
      switch (pionPdg) {
        case -211: { t = 2; break; }
        case 111: { t = 1; break; }
        case 211: { t = 0; break; }
      }

      //DIS contribution
      double dis_spp = fromdis * alfadis(j, k, l, t, HadrMass);

      int StruckNucleonChrg = finalcharge + t - 1;
      int ProducedNucleonPdg = ( (StruckNucleonChrg == 1) ? 2212 : 2112 );

      double SPPFact = SPPF(j, k, l, t, W) * alfadelta(j, k, l, t, W);

      //resonant SPP contribution
      double delta_spp = cr_sec_delta (FFset, delta_axial_mass, delta_C5A,
        Enu, HadrMass, q0, NeutrinoPdg, StruckNucleonPdg, OutNucleonPdg,
            OutPionPdg, IsCC) / SPPFact;

      if ( (p.nucleus_p + p.nucleus_n) > 7 ){
        //Pionless delta decay reduction.
        delta_spp *= pdd_red(Enu);
      }
      return (dis_spp + delta_spp);
    } else { //inelastic pion production and single kaon production
      //Event was all pythia just return DIS xsec.
      return fromdis;
    }
  }
*/ return 1.0;
}

double NuwroReWeight_RES_CA5::CalcWeight(event* nuwro_event){
  return 1.0;
  /*
  if ((fabs(fTwkDial_MaRES) < 1E-8) && (fabs(fTwkDial_CA5) < 1E-8)){
    return 1.0;
  }

  if (!nuwro_event->flag.cc or !nuwro_event->flag.res){
    return 1.0;
  }

  //TODO: for dis contribution not affected by RES params, can insta return 1.0;

  double weight = 1;
  params rwparams = nuwro_event->par;

  int FFSet = rwparams.delta_FF_set;
  double Enu = nuwro_event->E();
  double HadrMass = nuwro_event->W();
  double q0 = nuwro_event->q0();
  int NeutrinoPdg = nuwro_event->nu().pdg;
  int StruckNucleonPdg = nuwro_event->N0().pdg;
  bool IsCC = nuwro_event->flag.cc;

  double oldweight = GetWghtPropToResXSec(FFSet, Enu, HadrMass, q0,
    NeutrinoPdg, StruckNucleonPdg, IsCC);

  std::cout << "[INFO]: Reweighting nominal MaRES: { "
  << rwparams.pion_axial_mass << " -> " << fCurr_MaRES << " } and CA5: {"
  << rwparams.pion_C5A << " -> " << fCurr_CA5 << "}" << std::endl;

  rwparams.pion_axial_mass = fCurr_MaRES;
  rwparams.pion_C5A = fCurr_CA5;

  double newweight = GetWghtPropToResXSec(FFSet, Enu, HadrMass, q0,
    NeutrinoPdg, StruckNucleonPdg, IsCC);

  weight *= newweight/oldweight;

  std::cout << "Returning event weight of " << weight << std::endl;

  return weight;
  */
}

double NuwroReWeight_RES_CA5::CalcChisq(void){
  double chisq = 0;
  return chisq;
}
