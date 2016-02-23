#include "NuwroReWeight_MaCCQE.h"

#include "NuwroSystUncertainty.h"
#include "ff.h"
#include "qel_sigma.h"

#include <iostream>

#include "TFile.h"
#include "TROOT.h"
#include "TMath.h"

using namespace nuwro::rew;

NuwroReWeight_MaCCQE::NuwroReWeight_MaCCQE(){
  fTwkDial_MaCCQE = 0;
  fDef_MaCCQE = 1200;
  fCurr_MaCCQE = fDef_MaCCQE;
}

NuwroReWeight_MaCCQE::NuwroReWeight_MaCCQE(params const & param){
  fTwkDial_MaCCQE = 0;
  fDef_MaCCQE = param.qel_cc_axial_mass;
  fCurr_MaCCQE = fDef_MaCCQE;
}

NuwroReWeight_MaCCQE::~NuwroReWeight_MaCCQE(){}

bool NuwroReWeight_MaCCQE::SystIsHandled(NuwroSyst_t syst){

  switch(syst){
    case kNuwro_MaCCQE: { return true; }
    default: { return false; }
  }
}

void NuwroReWeight_MaCCQE::SetSystematic(NuwroSyst_t syst, double val){
  if(syst == kNuwro_MaCCQE){ fTwkDial_MaCCQE = val; }
}

void NuwroReWeight_MaCCQE::Reset(void){
  fTwkDial_MaCCQE = 0;
  this->Reconfigure();
}

void NuwroReWeight_MaCCQE::Reconfigure(void){

  NuwroSystUncertainty * fracerr = NuwroSystUncertainty::Instance();
  fError_MaCCQE = fracerr->OneSigmaErr(kNuwro_MaCCQE,
    (fTwkDial_MaCCQE > 0)?1:-1);
  if(fabs(fTwkDial_MaCCQE) > 1E-8){
#ifdef DEBUG_QE_REWEIGHT
    std::cout << "[INFO]: NuwroReWeight_MaCCQE::Reconfigure: New MaQE value = "
      << fDef_MaCCQE << " + (" << fError_MaCCQE << "  *  " << fTwkDial_MaCCQE
      << ")" << std::endl;
#endif
  }
  fCurr_MaCCQE = fDef_MaCCQE + fError_MaCCQE * fTwkDial_MaCCQE;
}


double NuwroReWeight_MaCCQE::CalcWeight(event* nuwro_event){

  if (fabs(fTwkDial_MaCCQE) < 1E-8){ return 1.0; }

  if (!nuwro_event->flag.cc or !nuwro_event->flag.qel){ return 1.0; }

  double weight = 1;
  params rwparams = (nuwro_event->par);

  ff_configure(rwparams);

  double E = nuwro_event->E();
  double q2 = nuwro_event->q2();
  double m_lep = nuwro_event->out[0].mass();
  double m_nuc = nuwro_event->N0().mass();
  bool pdg_neut = nuwro_event->nu().pdg < 0;

  double oldweight = qel_sigma( E, q2, 0, pdg_neut, m_lep, m_nuc );
#ifdef DEBUG_QE_REWEIGHT
  std::cout << "Old / NEW axial mass = " << rwparams.qel_cc_axial_mass << "/"
    << fCurr_MaCCQE << std::endl;
#endif
  rwparams.qel_cc_axial_mass = fCurr_MaCCQE;
  ff_configure(rwparams);

  double newweight = qel_sigma( E, q2, 0, pdg_neut, m_lep, m_nuc );

  weight *= newweight/oldweight;
#ifdef DEBUG_QE_REWEIGHT
  std::cout << "Returning event weight of " << weight << std::endl;
#endif

  return weight;

}

double NuwroReWeight_MaCCQE::CalcChisq(void){
  double chisq = 0;
  return chisq;
}
