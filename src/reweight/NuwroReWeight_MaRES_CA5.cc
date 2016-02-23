#include <iostream>
#include <cassert>
#include <cmath>

#include "TFile.h"
#include "TROOT.h"
#include "TMath.h"

#include "ff.h"
#include "qel_sigma.h"

#include "dis/alfa.h"
#include "dis/dis_cr_sec.h"
#include "dis/charge.h"
#include "dis/delta.h"
#include "dis/resevent2.h"
#include "dis/singlepion.h"

#include "reweight/NuwroSystUncertainty.h"

#include "reweight/NuwroReWeight_MaRES_CA5.h"

//Apparently externing yourself is better than having it in a header...
//go figure
extern double SPP[2][2][2][3][40];

namespace nuwro {
namespace rew {

bool NuwroReWeight_MaRES_CA5::DoSetupSPP = true;

NuwroReWeight_MaRES_CA5::NuwroReWeight_MaRES_CA5(){
  fTwkDial_MaRES = 0;
  fDef_MaRES = 940;
  fCurr_MaRES = fDef_MaRES;

  fTwkDial_CA5 = 0;
  fDef_CA5 = 1.19;
  fCurr_CA5 = fDef_CA5;
  if(DoSetupSPP){
    SetupSPP();
  }
}

NuwroReWeight_MaRES_CA5::NuwroReWeight_MaRES_CA5(params const & param){
  fTwkDial_MaRES = 0;
  fDef_MaRES = param.pion_axial_mass;
  fCurr_MaRES = fDef_MaRES;

  fTwkDial_CA5 = 0;
  fDef_CA5 = param.pion_C5A;
  fCurr_CA5 = fDef_CA5;
  params nc_param(param);
  if(DoSetupSPP){
    SetupSPP(nc_param);
  }
}

void NuwroReWeight_MaRES_CA5::SetupSPP(){
    std::cout << "[WARN]: Setting up singlepion tables with default parameter "
      "set." << std::endl;
    params default_p;
    SetupSPP(default_p);
}
void NuwroReWeight_MaRES_CA5::SetupSPP(params & param){
  if(true){//!CheckSPPSetup()){ -- No way to know in general
    std::cout << "[INFO]: Setting up singlepion tables..." << std::endl;
    singlepion(param);
    std::cout << "[INFO]: Set up singlepion tables!" << std::endl;
  }
}

NuwroReWeight_MaRES_CA5::~NuwroReWeight_MaRES_CA5(){}

bool NuwroReWeight_MaRES_CA5::SystIsHandled(NuwroSyst_t syst){
  switch(syst){
    case kNuwro_MaRES: { return true; }
    case kNuwro_CA5: { return true; }
    default: { return false; }
  }
}

void NuwroReWeight_MaRES_CA5::SetSystematic(NuwroSyst_t syst, double val){
  if(syst == kNuwro_MaRES){ fTwkDial_MaRES = val; }
  if(syst == kNuwro_CA5){ fTwkDial_CA5 = val; }
}

void NuwroReWeight_MaRES_CA5::Reset(void){
  fTwkDial_MaRES = 0;
  fTwkDial_CA5 = 0;
  this->Reconfigure();
}

void NuwroReWeight_MaRES_CA5::Reconfigure(void){

  NuwroSystUncertainty * fracerr = NuwroSystUncertainty::Instance();
  fError_MaRES = fracerr->OneSigmaErr(kNuwro_MaRES,
    (fTwkDial_MaRES > 0)?1:-1);
  if(fTwkDial_MaRES){
#ifdef DEBUG_RES_REWEIGHT
    std::cout << "[INFO]: NuwroReWeight_MaRES_CA5::Reconfigure: New MaRES value = "
      << fDef_MaRES << " + (" << fError_MaRES << "  *  " << fTwkDial_MaRES
      << ")" << std::endl;
#endif
  }
  fCurr_MaRES = fDef_MaRES + fError_MaRES * fTwkDial_MaRES;

  fError_CA5 = fracerr->OneSigmaErr(kNuwro_CA5,
    (fTwkDial_CA5 > 0)?1:-1);
  if(fTwkDial_CA5){
#ifdef DEBUG_RES_REWEIGHT
    std::cout << "[INFO]: NuwroReWeight_MaRES_CA5::Reconfigure: New CA5 value = "
      << fDef_CA5 << " + (" << fError_CA5 << "  *  " << fTwkDial_CA5
      << ")" << std::endl;
#endif
  }
  fCurr_CA5 = fDef_CA5 + fError_CA5 * fTwkDial_CA5;
}

double GetEBind(event &nuwro_event, params const & rwparams){
  switch(rwparams.nucleus_target){
    case 0:{ return 0; }//free
    case 1:{ return rwparams.nucleus_E_b; }//FG
    case 2:{ return 0; }//local FG
    case 3:{ return 0; }//Bodek
    case 4:{//effective SF
      vec ped = nuwro_event.in[1].p();
      return binen (ped, rwparams.nucleus_p, rwparams.nucleus_n);
    }
    case 5:{//deuterium
      vec ped = nuwro_event.in[1].p();
      return deuter_binen (ped);
    }
    case 6:{ return rwparams.nucleus_E_b; } //deuterium like Fermi gas
    default:{ return 0; }
  }
}

double GetWghtPropToResXSec(event &nuwro_event, params const & rwparams){
  //see dis/resevent2.cc to see where this calculation is distilled from

  int const &FFSet = rwparams.delta_FF_set;
  double const &delta_axial_mass = rwparams.pion_axial_mass;
  double const &delta_C5A = rwparams.pion_C5A;

  //Need to boost to the COM frame to get q0/nu/omega correct
  vect nuc0 = nuwro_event.in[1];
  // I don't think that this ever gets added back on, so probably don't need to
  // re-boost.
  // nuc0.t -= GetEBind(nuwro_event, rwparams);

  vect nu0 = nuwro_event.in[0];
  nu0.boost( -nuc0.v() );

  vect fsLep = nuwro_event.out[0];
  fsLep.boost( -nuc0.v() );

  double Enu = nu0.t;
  double HadrMass = nuwro_event.W();
  double q0 = (nu0.t - fsLep.t);
  int NeutrinoPdg = nuwro_event.in[0].pdg;
  int StruckNucleonPdg = nuwro_event.in[1].pdg;
  bool IsCC = nuwro_event.flag.cc;

  //Check if we have enough Energy.
  double NucMeff = sqrt(nuc0 * nuc0);
  double Meff = std::min(NucMeff, (PDG::mass_proton+PDG::mass_neutron)/2.0);
  double Meff2 = Meff * Meff;
  double leptonMass = lepton_mass(abs(NeutrinoPdg), IsCC);
  if (Enu < ((1080 + leptonMass) * (1080 + leptonMass) - Meff2) / 2 / Meff){
    std::cout << "[WARN]: COM Enu, " << Enu << ", too low to "
      "support SPP is this really a RES event?" << std::endl;
#ifdef DEBUG_RES_REWEIGHT
      assert(false);
#endif
    return 0;
  }

  if (HadrMass < 1080){
    std::cout << "[WARN]: HadrMass, " << HadrMass << ", too low to "
      "support SPP is this really a RES event?" << std::endl;
#ifdef DEBUG_RES_REWEIGHT
      assert(false);
#endif
    return 0;
  }

  int j, k, l;
  j = (NeutrinoPdg > 0) ? 0 : 1;
  k = (IsCC) ? 0 : 1;
  l = (StruckNucleonPdg == 2212) ? 0 : 1;
  int finalcharge = charge(StruckNucleonPdg) + (1 - k) * (1 - 2 * j); //pdg.cc

  double nonspp = 0;
  double dis0 = 0;
  double dis1 = 0;
  double dis2 = 0;

  double fromdis = cr_sec_dis(Enu, HadrMass, q0, NeutrinoPdg,
    StruckNucleonPdg, IsCC);

  if(fromdis < 0){
    fromdis = 0;
  }

  if(fromdis != fromdis){
#ifdef DEBUG_RES_REWEIGHT
    assert(false);
#endif
    return 0;
  }

  //No DIS contribution
  if( (HadrMass < 1210) || (fromdis == 0) ){
    double spp0 = SPP[j][k][l][0][0];
    double spp1 = SPP[j][k][l][1][0];
    double spp2 = SPP[j][k][l][2][0];

    dis0 = fromdis * spp0 * alfadis(j, k, l, 0, HadrMass);
    dis1 = fromdis * spp1 * alfadis(j, k, l, 1, HadrMass);
    dis2 = fromdis * spp2 * alfadis(j, k, l, 2, HadrMass);

    double delta0 = 0, delta1 = 0, delta2 = 0;

    double adel0 = alfadelta(j, k, l, 0, HadrMass);
    double adel1 = alfadelta(j, k, l, 1, HadrMass);
    double adel2 = alfadelta(j, k, l, 2, HadrMass);

    switch(finalcharge){
      case -1: {
        delta0 = 0;
        delta1 = 0;
        delta2 = cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2112, -211, IsCC) * adel2;
        break;
      }
      case 0: {
        delta0 = 0;
        delta1 = cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2112, 111, IsCC) * adel1;
        delta2 = cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2212, -211, IsCC) * adel2;
        break;
      }
      case 1: {
        delta0 = cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2112, 211, IsCC) * adel0;
        delta1 = cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2212, 111, IsCC) * adel1;
        delta2 = 0;
        break;
      }
      case 2: {
        delta0 = cr_sec_delta(FFSet, delta_axial_mass, delta_C5A, Enu, HadrMass,
          q0, NeutrinoPdg, StruckNucleonPdg, 2212, 211, IsCC) * adel0;
        delta1 = 0;
        delta2 = 0;
        break;
      }
    }
#ifdef DEBUG_RES_REWEIGHT
    double test = (dis0 + dis1 + dis2 + delta0 + delta1 + delta2);
    assert((test == test));
    std::cout << "[INFO]: result = " << test << ". Return Low HadrMass: "
      << HadrMass << ", fromdis: " << fromdis
      << " { Enu: " << Enu << ", W: " << HadrMass << ", q0: "
      << q0 << " }" << std::endl;
#endif
    return (dis0 + dis1 + dis2 + delta0 + delta1 + delta2);
  } else { //(HadrMass > 1210) && (fromdis != 0)
    //With DIS contribution
    //This check is effectively the same as for Pythia SPP NPar == 5;
    int OutPionPdg = 0; int NPions = 0;
    if( (nuwro_event.out[1].pdg == -211) ||
        (nuwro_event.out[1].pdg == 111)  ||
        (nuwro_event.out[1].pdg == 211)    ) {
      OutPionPdg = nuwro_event.out[1].pdg;
      NPions++;
    }
    if( (nuwro_event.out[2].pdg == -211) ||
        (nuwro_event.out[2].pdg == 111)  ||
        (nuwro_event.out[2].pdg == 211)    ) {
      OutPionPdg = nuwro_event.out[2].pdg;
      NPions++;
    }
    bool IsPythiaSPP = ((nuwro_event.out.size()==3) && OutPionPdg);
    if(IsPythiaSPP && (NPions==2)) {
      std::cerr << "[ERROR]: Found 2 pions in SPP event::out vector ("
        << "size: " << nuwro_event.out.size() << ")" << std::endl;
      throw std::exception();
    }
#ifdef DEBUG_RES_REWEIGHT
    std::cout << "[INFO]: IsPythiaSPP: " << (IsPythiaSPP?"true":"false")
      << ", event_out size: " << nuwro_event.out.size() << ", Found PionPdg? "
      << OutPionPdg << std::endl;
#endif
    //Event was Single Pion Production
    if(IsPythiaSPP){

      int t = 0;
      switch (OutPionPdg) {
        case -211: { t = 2; break; }
        case 111: { t = 1; break; }
        case 211: { t = 0; break; }
      }

      //DIS contribution
      double dis_spp = fromdis * alfadis(j, k, l, t, HadrMass);

      int StruckNucleonChrg = finalcharge + t - 1;
      int ProducedNucleonPdg = ( (StruckNucleonChrg == 1) ? 2212 : 2112 );

      double SPPFact = SPPF(j, k, l, t, HadrMass);
      double AlfaFact = alfadelta(j, k, l, t, HadrMass);
#ifdef DEBUG_RES_REWEIGHT
      if(!(fabs(SPPFact)>0)){
        std::cout << "[SPPFACT-INFO]: " << NeutrinoPdg << ", " << IsCC << ", "
          << StruckNucleonPdg << std::endl;
        std::cout << "[SPPFACT-INFO]: " << j << "," << k << "," << l << ","
          << t << "," << HadrMass << std::endl;
      }
      SPPF(j, k, l, t, HadrMass);
      assert(fabs(SPPFact)>0);
#endif
      //resonant SPP contribution
      double delta_spp = cr_sec_delta (FFSet, delta_axial_mass, delta_C5A,
        Enu, HadrMass, q0, NeutrinoPdg, StruckNucleonPdg, ProducedNucleonPdg,
            OutPionPdg, IsCC) / SPPFact * AlfaFact;

      if( (rwparams.nucleus_p + rwparams.nucleus_n) > 7 ){
        //Pionless delta decay reduction.
        delta_spp *= pdd_red(nuwro_event.in[0].t);
      }
#ifdef DEBUG_RES_REWEIGHT
    double test = (dis_spp + delta_spp);

    double FACT_cr_sec_delta = cr_sec_delta (FFSet, delta_axial_mass, delta_C5A,
        Enu, HadrMass, q0, NeutrinoPdg, StruckNucleonPdg, ProducedNucleonPdg,
            OutPionPdg, IsCC);

    assert((test == test));
    std::cout << "[INFO]: Result = " << test << ". Return Pythia SPP: dis_spp = "
      << dis_spp << ", delta_spp: "
      << delta_spp << " (" << pdd_red(nuwro_event.in[0].t)
      << ") { Enu: " << Enu << ", W: " << HadrMass << ", q0: "
      << q0 << " }" << std::endl;
      std::cout << "SPP Factors: "
        << "FACT_cr_sec_delta = " << FACT_cr_sec_delta
        << ", FACT_SPPF = " << SPPFact
        << ", FACT_alfadelta = " << AlfaFact << std::endl;
      std::cout << FFSet << ", " << delta_axial_mass << ", " << delta_C5A
        << ", " << Enu << ", " << HadrMass << ", " << q0 << ", "
        << NeutrinoPdg << ", " << StruckNucleonPdg << ", "
        << ProducedNucleonPdg << ", " << OutPionPdg << ", " << IsCC << std::endl;
#endif
      return (dis_spp + delta_spp);
    } else { //inelastic pion production and single kaon production
      //Event was all pythia just return DIS xsec.
#ifdef DEBUG_RES_REWEIGHT
    assert((fromdis == fromdis));
    std::cout << "[INFO]: Result = " << fromdis
      << ". Return Pythia DIS fromdis: "
      << fromdis << " { Enu: " << Enu << ", W: " << HadrMass << ", q0: "
      << q0 << " }" <<  std::endl;
#endif
      return fromdis;
    }
  }
}

double NuwroReWeight_MaRES_CA5::CalcWeight(event* nuwro_event){

  if ((fabs(fTwkDial_MaRES) < 1E-8) && (fabs(fTwkDial_CA5) < 1E-8)){
    return 1.0;
  }

  if (!nuwro_event->flag.cc or !nuwro_event->flag.res){
    return 1.0;
  }

  //TODO: for dis contribution not affected by RES params, can insta return 1.0;

  double weight = 1;
  params rwparams = nuwro_event->par;

  double oldweight = GetWghtPropToResXSec(*nuwro_event, rwparams);
#ifdef DEBUG_RES_REWEIGHT
  std::cout << "[INFO]: Reweighting nominal MaRES: { "
    << rwparams.pion_axial_mass << " -> " << fCurr_MaRES << " } and CA5: {"
    << rwparams.pion_C5A << " -> " << fCurr_CA5 << "}" << std::endl;
#endif
  rwparams.pion_axial_mass = fCurr_MaRES;
  rwparams.pion_C5A = fCurr_CA5;

  double newweight = GetWghtPropToResXSec(*nuwro_event, rwparams);

  weight *= newweight/oldweight;
#ifdef DEBUG_RES_REWEIGHT
  std::cout << "Returning event weight of " << weight << " = " << newweight
    << "/" << oldweight << std::endl;
#endif
  assert(std::isfinite(weight));

  return weight;
}

double NuwroReWeight_MaRES_CA5::CalcChisq(void){
  double chisq = 0;
  return chisq;
}

}
}
