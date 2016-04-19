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
  double CalcChisq(void);

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

};

} // rew namespace
} // nuwro namespace

#endif
