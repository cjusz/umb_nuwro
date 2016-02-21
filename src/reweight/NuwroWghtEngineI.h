#ifndef _NUWRO_REWEIGHT_ABC_H_
#define _NUWRO_REWEIGHT_ABC_H_

#include "event1.h"
#include "RooTrackerEvent.h"


#include "NuwroSyst.h"
#include "NuwroSystSet.h"

namespace nuwro {
namespace rew {

class NuwroWghtEngineI {
 public:
  virtual ~NuwroWghtEngineI(){}

  /// does the current weight calculator handle the input nuisance param?
  virtual bool SystIsHandled (NuwroSyst_t syst) = 0;

  /// update the value for the specified nuisance param
  virtual void SetSystematic (NuwroSyst_t syst, double val) = 0;

  ///  set all nuisance parameters to default values
  virtual void Reset (void) = 0;

  /// propagate updated nuisance parameter values to actual MC, etc
  virtual void Reconfigure (void) = 0;

  /// calculate a weight for the input event using the current nuisance param values
  virtual double CalcWeight (event * nuwro_event) = 0;
  virtual double CalcWeight (RooTrackerEvent &nuwro_event){ return 0.0; };

  /// calculate penalty factors
  virtual double CalcChisq (void) = 0;
};

} // rew namespace
} // nuwro namespace

#endif
