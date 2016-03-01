#ifndef _NUWRO_REWEIGHT_MEC_H_
#define _NUWRO_REWEIGHT_MEC_H_

#include "NuwroWghtEngineI.h"

namespace nuwro {
  namespace rew  {

    class NuwroReWeight_FlagNorm : public NuwroWghtEngineI
    {
    public:

      NuwroReWeight_FlagNorm();
      ~NuwroReWeight_FlagNorm();

      bool   SystIsHandled      (NuwroSyst_t syst);
      void   SetSystematic  (NuwroSyst_t syst, double val);
      void   Reset          (void);
      void   Reconfigure    (void);
      double CalcWeight     (event* event);
      double CalcChisq      (void);


    private:
      void Init(void);

      double fTwkDial_QEL;
      double fNormDef_QEL;
      double fNormCurr_QEL;
      double fNormError_QEL;

      double fTwkDial_RES;
      double fNormDef_RES;
      double fNormCurr_RES;
      double fNormError_RES;

      double fTwkDial_DIS;
      double fNormDef_DIS;
      double fNormCurr_DIS;
      double fNormError_DIS;

      double fTwkDial_COH;
      double fNormDef_COH;
      double fNormCurr_COH;
      double fNormError_COH;

      double fTwkDial_MEC;
      double fNormDef_MEC;
      double fNormCurr_MEC;
      double fNormError_MEC;

      double fTwkDial_CC;
      double fNormDef_CC;
      double fNormCurr_CC;
      double fNormError_CC;

      double fTwkDial_NC;
      double fNormDef_NC;
      double fNormCurr_NC;
      double fNormError_NC;

      double fTwkDial_ANTINU;
      double fNormDef_ANTINU;
      double fNormCurr_ANTINU;
      double fNormError_ANTINU;

      double fTwkDial_DYN[10];
      double fNormDef_DYN[10];
      double fNormCurr_DYN[10];
      double fNormError_DYN[10];

    };

  } // rew namespace

} // nuwro namespace

#endif
