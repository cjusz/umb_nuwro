#ifndef _NUWRO_REWEIGHT_MaRES_H_
#define _NUWRO_REWEIGHT_MaRES_H_

#include "params.h"

#include "NuwroWghtEngineI.h"

namespace nuwro {
namespace rew  {

//#define DEBUG_RES_REWEIGHT

void SetupSPP(void);
void SetupSPP(params & param);

double GetWghtPropToResXSec(event const &nuwro_event, params const & rwparams);

class NuwroReWeight_SPP : public NuwroWghtEngineI {
public:

  NuwroReWeight_SPP();
  NuwroReWeight_SPP(params const & param);
  ~NuwroReWeight_SPP();

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

  double fTwkDial_SPPDISBkgScale;
  double fDef_SPPDISBkgScale;
  double fCurr_SPPDISBkgScale;
  double fError_SPPDISBkgScale;

};

} // rew namespace
} // nuwro namespace

#endif
