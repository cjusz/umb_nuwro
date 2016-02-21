#ifndef _NUWRO_SET_OF_SYSTEMATICS_H_
#define _NUWRO_SET_OF_SYSTEMATICS_H_

#include <string>
#include <map>
#include <vector>

#include "NuwroSyst.h"
#include "NuwroWghtEngineI.h"

namespace nuwro {
namespace rew   {

class NuwroSystInfo;

class NuwroSystSet {

public:
  NuwroSystSet();
  NuwroSystSet(const NuwroSystSet & syst_set);
 ~NuwroSystSet();

  const NuwroSystInfo& Add(NuwroSyst_t syst, double init=0, double min=-1,
    double max=+1, double step=0.05);
  const NuwroSystInfo& Add(NuwroSyst_t syst,
    NuwroSystInfo const & systInfo);
  void Remove(NuwroSyst_t syst);
  void SetSystVal(NuwroSyst_t syst, double value);
  int Size(void) const;
  bool Contains(NuwroSyst_t syst) const;
  void Print(void) const;

  NuwroSystInfo &GetSystInfo(NuwroSyst_t syst);

  std::vector<NuwroSyst_t> GetAllSysts(void) const;

private:
  std::map<NuwroSyst_t, NuwroSystInfo> fSystematics;
};

class NuwroSystInfo {

public:
  NuwroSystInfo();
  NuwroSystInfo(NuwroSystInfo const &);
  NuwroSystInfo(double init, double min, double max, double step);

  void TurnUp(int notches=1) {
    CurValue += notches*Step;
    if(CurValue > MaxValue){ CurValue = MaxValue; }
  }
  void TurnDown(int notches=1) {
    CurValue -= notches*Step;
    if(CurValue < MinValue){ CurValue = MinValue; }
  }

  ~NuwroSystInfo(){}

  double CurValue;
  double InitValue;
  double MinValue;
  double MaxValue;
  double Step;
  bool IsValid;
};

} // rew namespace
} // nuwro namespace

#endif

