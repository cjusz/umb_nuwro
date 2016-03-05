#ifndef _NUWRO_REWEIGHT_ABC_H_
#define _NUWRO_REWEIGHT_ABC_H_

#include "event1.h"

#include "reweight/NuwroSyst.h"
#include "reweight/NuwroSystSet.h"

namespace nuwro {
namespace rew {

class NuwroWghtEngineI {
 public:
  virtual ~NuwroWghtEngineI(){}

  /// does the current weight calculator handle the input nuisance param?
  virtual bool SystIsHandled (NuwroSyst_t syst) = 0;

  /// update the value for the specified nuisance param
  virtual void SetSystematic (NuwroSyst_t syst, double val) = 0;

  /// get the dial value for the specified nuisance param
  virtual double GetSystematic (NuwroSyst_t syst){
    throw syst;
    return 0xdeadbeef;
  };

  /// get the value for the specified nuisance param
  virtual double GetSystematicValue (NuwroSyst_t syst){
    throw syst;
    return 0xdeadbeef;
  };

  /// set all nuisance parameters to default values
  virtual void Reset (void) = 0;

  /// propagate updated nuisance parameter values to actual MC, etc
  virtual void Reconfigure (void) = 0;

  /// calculate a weight for the input event using the current nuisance param values
  virtual double CalcWeight (event * nuwro_event) = 0;

  /// calculate penalty factors
  virtual double CalcChisq (void) = 0;
};

} // rew namespace
} // nuwro namespace

#endif
