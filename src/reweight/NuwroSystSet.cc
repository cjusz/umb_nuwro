#include <iostream>

#include "NuwroSystSet.h"

using namespace nuwro::rew;

NuwroSystSet::NuwroSystSet(){}

NuwroSystSet::NuwroSystSet(const NuwroSystSet & syst_set){
  fSystematics.clear();
  fSystematics.insert(syst_set.fSystematics.begin(),
    syst_set.fSystematics.end());
}

NuwroSystSet::~NuwroSystSet(){}

const NuwroSystInfo& NuwroSystSet::Add(NuwroSyst_t syst, double init,
  double min, double max, double step){

  if(syst == kNullSystematic) {
    std::cout << "[WARN](__FILE__:__LINE__): Attempted to add Null systematic."
      << std::endl;
    return fSystematics[kNullSystematic];
  }

  //Will only remove if it exists.
  Remove(syst);

  return (fSystematics[syst] = NuwroSystInfo(init, min, max, step));
}
const NuwroSystInfo& NuwroSystSet::Add(NuwroSyst_t syst,
  NuwroSystInfo const & systInfo){

  if(syst == kNullSystematic) {
    std::cout << "[WARN](__FILE__:__LINE__): Attempted to add Null systematic."
      << std::endl;
    return fSystematics[kNullSystematic];
  }

  Remove(syst);

  return (fSystematics[syst] = systInfo);
}

void NuwroSystSet::Remove(NuwroSyst_t syst){
  fSystematics.erase(syst);
}

int NuwroSystSet::Size(void) const {
  return fSystematics.size();
}

bool NuwroSystSet::Contains(NuwroSyst_t syst) const {
  return (fSystematics.find(syst) != fSystematics.end()) &&
    (fSystematics.find(syst)->second.IsValid);
}

std::vector<NuwroSyst_t>
NuwroSystSet::GetAllSysts(void) const {
  std::vector<NuwroSyst_t> svec;

  for(std::map<NuwroSyst_t, NuwroSystInfo>::const_iterator it =
    fSystematics.begin(); it != fSystematics.end(); ++it){
    if(it->second.IsValid){
      svec.push_back(it->first);
    }
  }
  return svec;
}

NuwroSystInfo &NuwroSystSet::GetSystInfo(NuwroSyst_t syst) {
  if(Contains(syst)){
    return (fSystematics.find(syst)->second);
  }
  return fSystematics[kNullSystematic];
}

void NuwroSystSet::SetSystVal(NuwroSyst_t syst, double val){
  if(!Contains(syst)){
    Add(syst);
  }
  fSystematics.find(syst)->second.CurValue = val;
}

void NuwroSystSet::Print(void) const{

  std::cout << "[INFO] Considering " << this->Size() << " systematics:"
    << std::endl;

  for(std::map<NuwroSyst_t, NuwroSystInfo>::const_iterator it =
    fSystematics.begin(); it != fSystematics.end(); ++it){
    std::cout << "\t" << it->first << "(" << it->second.MinValue << "-"
      << it->second.MaxValue << ":" << it->second.Step << "), Current value = "
      << it->second.CurValue << std::endl;
  }
}

NuwroSystInfo::NuwroSystInfo() : CurValue(0), InitValue(0), MinValue(0),
MaxValue(0), Step(0), IsValid(false){}

NuwroSystInfo::NuwroSystInfo(NuwroSystInfo const & other) :
  CurValue(other.CurValue), InitValue(other.InitValue),
  MinValue(other.MinValue), MaxValue(other.MaxValue), Step(other.Step),
  IsValid(other.IsValid){}

NuwroSystInfo::NuwroSystInfo(double init, double min, double max, double step) :
  CurValue(init), InitValue(init), MinValue(min), MaxValue(max), Step(step),
  IsValid(true){}
