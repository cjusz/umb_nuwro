#ifndef _NUWRO_REWEIGHT_MEC_H_
#define _NUWRO_REWEIGHT_MEC_H_

#include "NuwroWghtEngineI.h"

namespace nuwro {
  namespace rew  {

    class NuwroReWeight_MECNorm : public NuwroWghtEngineI
    {
    public:

      NuwroReWeight_MECNorm();
      ~NuwroReWeight_MECNorm();

      bool   SystIsHandled      (NuwroSyst_t syst);
      void   SetSystematic  (NuwroSyst_t syst, double val);
      void   Reset          (void);
      void   Reconfigure    (void);
      double CalcWeight     (const event & event);
      double CalcChisq      (void);


    private:
      void Init(void);

      double fTwkDial_MEC;
      double fNormDef_MEC;
      double fNormCurr_MEC;
      double fNormError_MEC;



    };

  } // rew namespace

} // nuwro namespace

#endif
