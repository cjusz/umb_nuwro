#ifndef _NUWRO_SYSTEMATIC_PARAM_H_
#define _NUWRO_SYSTEMATIC_PARAM_H_

#include <string>

namespace nuwro {
namespace rew {

typedef enum ENuWroSyst {
  kNullSystematic = 0,
  kNuwro_MaCCQE,
  kNuwro_MECNorm,
  kNNuWroSystematics
} NuwroSyst_t;

class NuwroSyst {
public:

  static std::string AsString(NuwroSyst_t syst);
  static NuwroSyst_t FromString(std::string syst);
  static NuwroSyst_t Next(NuwroSyst_t const &syst);
  static NuwroSyst_t Prev(NuwroSyst_t const &syst);
  static void Adv(NuwroSyst_t &syst);
  static void Red(NuwroSyst_t &syst);
};

}
}

std::ostream& operator<<(std::ostream &os, nuwro::rew::ENuWroSyst const & syst);

#endif

