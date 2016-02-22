#ifndef _NUWRO_REWEIGHT_MaRES_H_
#define _NUWRO_REWEIGHT_MaRES_H_

#include "NuwroWghtEngineI.h"

namespace nuwro {
namespace rew  {

class NuwroReWeight_RES_CA5 : public NuwroWghtEngineI {
public:

  NuwroReWeight_RES_CA5();
  ~NuwroReWeight_RES_CA5();

  bool SystIsHandled(NuwroSyst_t syst);
  void SetSystematic(NuwroSyst_t syst, double val);
  void Reset(void);
  void Reconfigure(void);
  double CalcWeight(event* nuwro_event);
  double CalcChisq(void);


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
