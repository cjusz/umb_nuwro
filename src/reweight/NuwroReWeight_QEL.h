#ifndef _NUWRO_REWEIGHT_QEL_H_
#define _NUWRO_REWEIGHT_QEL_H_

#include "params.h"

#include "NuwroWghtEngineI.h"
#include "NuwroReWeightSimpleEvent.h"

namespace nuwro {
namespace rew  {

//#define DEBUG_QE_REWEIGHT

double GetWghtPropToQEXSec(event const &nuwro_event, params const &rwparams);
double GetWghtPropToQEXSec(SRW::SRWEvent const &ev, params const &rwparams);

class NuwroReWeight_QEL : public NuwroWghtEngineI {
public:

  NuwroReWeight_QEL();
  NuwroReWeight_QEL(params const & param);
  ~NuwroReWeight_QEL();

  bool SystIsHandled(NuwroSyst_t syst);
  void SetSystematic(NuwroSyst_t syst, double val);
  double GetSystematic(NuwroSyst_t syst);
  double GetSystematicValue(NuwroSyst_t syst);
  void Reset(void);
  void Reconfigure(void);
  double CalcWeight(event* nuwro_event);
  double CalcWeight(SRW::SRWEvent const& nuwro_event, params const &par);
  double CalcChisq(void);

  void ResetBBBA07(void);
private:
  void Init(void);

  double fTwkDial_MaCCQE;
  double fDef_MaCCQE;
  double fCurr_MaCCQE;
  double fError_MaCCQE;

  double fTwkDial_MaNCEL;
  double fDef_MaNCEL;
  double fCurr_MaNCEL;
  double fError_MaNCEL;

  double fTwkDial_MaNCEL_s;
  double fDef_MaNCEL_s;
  double fCurr_MaNCEL_s;
  double fError_MaNCEL_s;

  double fTwkDial_DeltaS;
  double fDef_DeltaS;
  double fCurr_DeltaS;
  double fError_DeltaS;

  // BBBA07 Parameters
  double p_AEp[7];
  double p_AMp[7];
  double p_AEn[7];
  double p_AMn[7];
  double p_AAx[7];

  double p_AEp_twk[7];
  double p_AMp_twk[7];
  double p_AEn_twk[7];
  double p_AMn_twk[7];
  double p_AAx_twk[7];

  double p_AEp_def[7];
  double p_AMp_def[7];
  double p_AEn_def[7];
  double p_AMn_def[7];
  double p_AAx_def[7];

  // 2 Comp Parameters
  double fTwkDial_axial_2comp_alpha;
  double fDef_axial_2comp_alpha;
  double fCurr_axial_2comp_alpha;
  double fError_axial_2comp_alpha;
  
  double fTwkDial_axial_2comp_gamma;
  double fDef_axial_2comp_gamma;
  double fCurr_axial_2comp_gamma;
  double fError_axial_2comp_gamma;

  double fTwkDial_axial_3comp_theta;
  double fDef_axial_3comp_theta;
  double fCurr_axial_3comp_theta;
  double fError_axial_3comp_theta;

  double fTwkDial_axial_3comp_beta;
  double fDef_axial_3comp_beta;
  double fCurr_axial_3comp_beta;
  double fError_axial_3comp_beta;

  // ZExp
  int    fTwk_zexp_NTerms;
  int    fDef_zexp_NTerms;
  bool   fTwk_zexp_Q4Limit;
  bool   fDef_zexp_Q4Limit;
  
  double fTwk_zexp_A[10];
  double fDef_zexp_A[10];
  double fCur_zexp_A[10];
  double fErr_zexp_A[10];
  
  double fTwk_zexp_TC;
  double fDef_zexp_TC;
  double fCur_zexp_TC;
  double fErr_zexp_TC;
  
  double fTwk_zexp_T0;
  double fDef_zexp_T0;
  double fCur_zexp_T0;
  double fErr_zexp_T0;

  // Alternative FF Dial
  int fTwk_AxlFFQEL;
  int fDef_AxlFFQEL;
};

} // rew namespace
} // nuwro namespace

#endif
