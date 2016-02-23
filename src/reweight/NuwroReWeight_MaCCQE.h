#ifndef _NUWRO_REWEIGHT_MAQE_H_
#define _NUWRO_REWEIGHT_MAQE_H_

#include "params.h"

#include "NuwroWghtEngineI.h"

namespace nuwro {
namespace rew  {

//#define DEBUG_QE_REWEIGHT

class NuwroReWeight_MaCCQE : public NuwroWghtEngineI {
public:

  NuwroReWeight_MaCCQE();
  NuwroReWeight_MaCCQE(params const & param);
  ~NuwroReWeight_MaCCQE();

  bool SystIsHandled(NuwroSyst_t syst);
  void SetSystematic(NuwroSyst_t syst, double val);
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

};

} // rew namespace
} // nuwro namespace

#endif
