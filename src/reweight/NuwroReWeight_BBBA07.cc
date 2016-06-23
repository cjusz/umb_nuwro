#include "NuwroReWeight_BBBA07.h"
#include "NuwroSyst.h"
#include "NuwroSystUncertainty.h"
#include "ff.h"
#include "qel_sigma.h"

#include <iostream>

#include "TFile.h"
#include "TMath.h"
#include "TROOT.h"

using namespace nuwro::rew;

NuwroReWeight_BBBA07::NuwroReWeight_BBBA07() { Reset(); }

NuwroReWeight_BBBA07::NuwroReWeight_BBBA07(params const& param) {
  // Setup Defaults
  Reset();
}

NuwroReWeight_BBBA07::~NuwroReWeight_BBBA07() {}

bool NuwroReWeight_BBBA07::SystIsHandled(NuwroSyst_t syst) {
  // Lazy so just looping through all
  for (int i = 0; i < 7; i++) {
    if (syst == kNuwro_BBBA07_AEp1 + i) return true;
    if (syst == kNuwro_BBBA07_AMp1 + i) return true;
    if (syst == kNuwro_BBBA07_AEn1 + i) return true;
    if (syst == kNuwro_BBBA07_AMn1 + i) return true;
  }

  return false;
}

void NuwroReWeight_BBBA07::SetSystematic(NuwroSyst_t syst, double val) {
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
}

void NuwroReWeight_BBBA07::Reset(void) {
  // Setup Defaults
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

  for (int i = 0; i < 7; i++) {
    p_AEp_twk[i] = 0.0;
    p_AEp[i] = p_AEp_def[i];

    p_AMp_twk[i] = 0.0;
    p_AMp[i] = p_AMp_def[i];

    p_AEn_twk[i] = 0.0;
    p_AEn[i] = p_AEn_def[i];

    p_AMn_twk[i] = 0.0;
    p_AMn[i] = p_AMn_def[i];
  }
  this->Reconfigure();
}

void NuwroReWeight_BBBA07::Reconfigure(void) {
  NuwroSystUncertainty* fracerr = NuwroSystUncertainty::Instance();

  NuwroSyst_t curenums[7] = {kNuwro_BBBA07_AEp1, kNuwro_BBBA07_AEp2,
                             kNuwro_BBBA07_AEp3, kNuwro_BBBA07_AEp4,
                             kNuwro_BBBA07_AEp5, kNuwro_BBBA07_AEp6,
                             kNuwro_BBBA07_AEp7};
  for (int i = 0; i < 7; i++) {
    double fError = fracerr->OneSigmaErr(curenums[i], p_AEp_twk[i] > 0);
    p_AEp[i] = p_AEp_def[i] * (1 + fError * p_AEp_twk[i]);
  }

  NuwroSyst_t curenums2[7] = {kNuwro_BBBA07_AMp1, kNuwro_BBBA07_AMp2,
                              kNuwro_BBBA07_AMp3, kNuwro_BBBA07_AMp4,
                              kNuwro_BBBA07_AMp5, kNuwro_BBBA07_AMp6,
                              kNuwro_BBBA07_AMp7};
  for (int i = 0; i < 7; i++) {
    double fError = fracerr->OneSigmaErr(curenums2[i], p_AMp_twk[i] > 0);
    p_AMp[i] = p_AMp_def[i] * (1 + fError * p_AMp_twk[i]);
  }

  NuwroSyst_t curenums3[7] = {kNuwro_BBBA07_AEn1, kNuwro_BBBA07_AEn2,
                              kNuwro_BBBA07_AEn3, kNuwro_BBBA07_AEn4,
                              kNuwro_BBBA07_AEn5, kNuwro_BBBA07_AEn6,
                              kNuwro_BBBA07_AEn7};
  for (int i = 0; i < 7; i++) {
    double fError = fracerr->OneSigmaErr(curenums3[i], p_AEn_twk[i] > 0);
    p_AEn[i] = p_AEn_def[i] * (1 + fError * p_AEn_twk[i]);
  }

  NuwroSyst_t curenums4[7] = {kNuwro_BBBA07_AMn1, kNuwro_BBBA07_AMn2,
                              kNuwro_BBBA07_AMn3, kNuwro_BBBA07_AMn4,
                              kNuwro_BBBA07_AMn5, kNuwro_BBBA07_AMn6,
                              kNuwro_BBBA07_AMn7};
  for (int i = 0; i < 7; i++) {
    double fError = fracerr->OneSigmaErr(curenums4[i], p_AMn_twk[i] > 0);
    p_AMn[i] = p_AMn_def[i] * (1.0 + fError * p_AMn_twk[i]);
  }
}

double NuwroReWeight_BBBA07::CalcWeight(event* nuwro_event) {
  if ((!nuwro_event->flag.cc) || (!nuwro_event->flag.qel)) {
#ifdef DEBUG_QE_REWEIGHT
    std::cout << "[WARN]: This is not a CCQE event. nwEv.flag.cc: "
              << nuwro_event->flag.cc
              << ", nwEv.flag.qel: " << nuwro_event->flag.qel << std::endl;
#endif
    return 1.0;
  }

  // Get start qel_sigma
  double weight = 1;
  params rwparams = (nuwro_event->par);

  if (rwparams.qel_vector_ff_set != 7) return 1.0;

  ff_configure(rwparams);

  double E = nuwro_event->E();
  double q2 = nuwro_event->q2();
  double m_lep = nuwro_event->out[0].mass();
  double m_nuc = nuwro_event->N0().mass();
  bool pdg_neut = nuwro_event->nu().pdg < 0;

  double oldweight = qel_sigma(E, q2, 0, pdg_neut, m_lep, m_nuc);

  // Set New Dials
  rwparams.bba07_AEp1 = p_AEp[0];
  rwparams.bba07_AEp2 = p_AEp[1];
  rwparams.bba07_AEp3 = p_AEp[2];
  rwparams.bba07_AEp4 = p_AEp[3];
  rwparams.bba07_AEp5 = p_AEp[4];
  rwparams.bba07_AEp6 = p_AEp[5];
  rwparams.bba07_AEp7 = p_AEp[6];

  rwparams.bba07_AMp1 = p_AMp[0];
  rwparams.bba07_AMp2 = p_AMp[1];
  rwparams.bba07_AMp3 = p_AMp[2];
  rwparams.bba07_AMp4 = p_AMp[3];
  rwparams.bba07_AMp5 = p_AMp[4];
  rwparams.bba07_AMp6 = p_AMp[5];
  rwparams.bba07_AMp7 = p_AMp[6];

  rwparams.bba07_AEn1 = p_AEn[0];
  rwparams.bba07_AEn2 = p_AEn[1];
  rwparams.bba07_AEn3 = p_AEn[2];
  rwparams.bba07_AEn4 = p_AEn[3];
  rwparams.bba07_AEn5 = p_AEn[4];
  rwparams.bba07_AEn6 = p_AEn[5];
  rwparams.bba07_AEn7 = p_AEn[6];

  rwparams.bba07_AMn1 = p_AMn[0];
  rwparams.bba07_AMn2 = p_AMn[1];
  rwparams.bba07_AMn3 = p_AMn[2];
  rwparams.bba07_AMn4 = p_AMn[3];
  rwparams.bba07_AMn5 = p_AMn[4];
  rwparams.bba07_AMn6 = p_AMn[5];
  rwparams.bba07_AMn7 = p_AMn[6];

  // Call reconfigure
  ff_configure(rwparams);

  // Get new weight
  double newweight = qel_sigma(E, q2, 0, pdg_neut, m_lep, m_nuc);

  weight *= newweight / oldweight;

  return weight;
}

double NuwroReWeight_BBBA07::CalcChisq(void) {
  double chisq = 0;
  return chisq;
}
