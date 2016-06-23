#ifndef _NUWRO_REWEIGHT_MaRES_H_
#define _NUWRO_REWEIGHT_MaRES_H_

#include "params.h"

#include "NuwroWghtEngineI.h"
#include "NuwroReWeightSimpleEvent.h"

namespace nuwro {
namespace rew  {

//#define DEBUG_RES_REWEIGHT

void SetupSPP(void);
void SetupSPP(params & param);

double GetWghtPropToResXSec(event const &nuwro_event, params const & rwparams);
double GetWghtPropToResXSec(SRW::SRWEvent const &ev, params const & rwparams);

class NuwroReWeight_SPP : public NuwroWghtEngineI {
public:

  NuwroReWeight_SPP();
  NuwroReWeight_SPP(params const & param);
  ~NuwroReWeight_SPP();

  bool SystIsHandled(NuwroSyst_t syst);
  void SetSystematic(NuwroSyst_t syst, double val);
  double GetSystematic(NuwroSyst_t syst);
  double GetSystematicValue(NuwroSyst_t syst);
  void Reset(void);
  void Reconfigure(void);
  void SetDefaults(params const &);
  double CalcWeight(event* nuwro_event);
  double CalcWeight(SRW::SRWEvent const& nuwro_event, params const &par);
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

  double fTwkDial_SPPBkgScale;
  double fDef_SPPBkgScale;
  double fCurr_SPPBkgScale;
  double fError_SPPBkgScale;

};

} // rew namespace
} // nuwro namespace

#endif
