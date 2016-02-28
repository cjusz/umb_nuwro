#ifndef _NUWRO_REWEIGHT_MaRES_H_
#define _NUWRO_REWEIGHT_MaRES_H_

#include "params.h"

#include "NuwroWghtEngineI.h"

namespace nuwro {
namespace rew  {

//#define DEBUG_RES_REWEIGHT

void SetupSPP(void);
void SetupSPP(params & param);

double GetWghtPropToResXSec(event const &nuwro_event, params const & rwparams,
  bool RTDebug=false);

class NuwroReWeight_MaRES_CA5 : public NuwroWghtEngineI {
public:

  NuwroReWeight_MaRES_CA5();
  NuwroReWeight_MaRES_CA5(params const & param);
  ~NuwroReWeight_MaRES_CA5();

  bool SystIsHandled(NuwroSyst_t syst);
  void SetSystematic(NuwroSyst_t syst, double val);
  void Reset(void);
  void Reconfigure(void);
  double CalcWeight(event* nuwro_event);
  double CalcChisq(void);

static bool DoSetupSPP;

private:
  void Init(void);

  double fTwkDial_MaRES;
  double fDef_MaRES;
  double fCurr_MaRES;
  double fError_MaRES;

  double fTwkDial_CA5;
  double fDef_CA5;
  double fCurr_CA5;
  double fError_CA5;

};

} // rew namespace
} // nuwro namespace

#endif
