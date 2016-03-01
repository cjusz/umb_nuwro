#ifndef _NUWRO_REWEIGHT_BBBA07_H_
#define _NUWRO_REWEIGHT_BBBA07_H_

#include "params.h"

#include "NuwroWghtEngineI.h"

namespace nuwro {
namespace rew  {

//#define DEBUG_QE_REWEIGHT

class NuwroReWeight_BBBA07 : public NuwroWghtEngineI {
public:

  NuwroReWeight_BBBA07();
  NuwroReWeight_BBBA07(params const & param);
  ~NuwroReWeight_BBBA07();

  bool SystIsHandled(NuwroSyst_t syst);
  void SetSystematic(NuwroSyst_t syst, double val);
  void Reset(void);
  void Reconfigure(void);
  double CalcWeight(event* nuwro_event);
  double CalcChisq(void);

private:
  void Init(void);

  double p_AEp[7];
  double p_AMp[7];
  double p_AEn[7];
  double p_AMn[7];
  
  double p_AEp_twk[7];
  double p_AMp_twk[7];
  double p_AEn_twk[7];
  double p_AMn_twk[7];

  double p_AEp_def[7];
  double p_AMp_def[7];
  double p_AEn_def[7];
  double p_AMn_def[7];
  
};

} // rew namespace
} // nuwro namespace

#endif
