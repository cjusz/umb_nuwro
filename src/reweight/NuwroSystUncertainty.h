#ifndef _NUWRO_SYST_UNCERTAINTY_H_
#define _NUWRO_SYST_UNCERTAINTY_H_

#include "NuwroSyst.h"
#include <map>

namespace nuwro {
namespace rew {

class NuwroSystUncertainty {

public:
  static NuwroSystUncertainty *Instance(void);
  static bool TearDown(void);

  double OneSigmaErr(nuwro::rew::NuwroSyst_t syst, int sign = 0) const;
  void SetUncertainty(nuwro::rew::NuwroSyst_t syst, double plus_err,
                      double minus_err);

private:
  void SetDefaults(void);

  std::map<nuwro::rew::NuwroSyst_t, double> fOneSigPlusErrMap;
  std::map<nuwro::rew::NuwroSyst_t, double> fOneSigMnusErrMap;

  NuwroSystUncertainty();
  NuwroSystUncertainty(const NuwroSystUncertainty &err);
  ~NuwroSystUncertainty();

  static NuwroSystUncertainty *fInstance;
};

} // rew namespace
} // niwg namespace

#endif
