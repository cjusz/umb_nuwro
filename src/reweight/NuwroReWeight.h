#ifndef _NUWRO_REWEIGHT_H_
#define _NUWRO_REWEIGHT_H_

#include "TObject.h"
#include <string>
#include <map>
#include "NuwroSyst.h"
#include "NuwroSystSet.h"
#include "NuwroSystUncertainty.h"
#include "NuwroWghtEngineI.h"
#include "event1.h"

namespace nuwro {
namespace rew   {

class NuwroReWeight {
public:
   NuwroReWeight();
  ~NuwroReWeight();

  /// add concrete weight calculator, transfers ownership
  void AdoptWghtCalc(std::string name, NuwroWghtEngineI* wcalc);
  bool AbandonWghtCalc(std::string name);
  /// access a weight calculator by name
  NuwroWghtEngineI* WghtCalc(std::string name);
  /// set of enabled systematic params & values
  NuwroSystSet &Systematics(void);
  /// reconfigure weight calculators with new params
  void Reconfigure(void);
  /// calculate weight for input event
  double CalcWeight(event * nuwro_event);
  double CalcWeight(RooTrackerEvent &nuwro_event);
  /// calculate penalty chisq for current values of tweaking dials
  double CalcChisq(void);
  /// print
  void Print(void);
  std::string GetState();

private:
  /// set of enabled nuisance parameters
  NuwroSystSet fSystSet;
  /// concrete weight calculators
  std::map<std::string, NuwroWghtEngineI *> fWghtCalc;
  typedef std::map<std::string, NuwroWghtEngineI *>::iterator WghtMapIt;
  typedef std::map<std::string, NuwroWghtEngineI *>::const_iterator WghtMapCIt;

};


} // rew   namespace
} // nuwro namespace

#endif
