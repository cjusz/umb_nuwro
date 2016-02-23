#include <iostream>
#include <vector>

#include <TMath.h>
#include <TString.h>

#include "reweight/NuwroReWeight.h"
#include "reweight/NuwroWghtEngineI.h"

using namespace nuwro::rew;

NuwroReWeight::NuwroReWeight(){}

NuwroReWeight::~NuwroReWeight(){
  for(WghtMapIt it = fWghtCalc.begin(); it != fWghtCalc.end(); ++it) {
    if(it->second) {
      delete it->second;
    }
  }
}

void NuwroReWeight::AdoptWghtCalc(string name, NuwroWghtEngineI* wcalc){
  if(!wcalc){ return; }
  AbandonWghtCalc(name);
  fWghtCalc[name] = wcalc;
}

bool NuwroReWeight::AbandonWghtCalc(std::string name){
  if(fWghtCalc.count(name) && fWghtCalc[name]){
    delete fWghtCalc[name];
  }
  return fWghtCalc.erase(name);
}

NuwroWghtEngineI* NuwroReWeight::WghtCalc(std::string name){
  if(fWghtCalc.count(name)){ return fWghtCalc[name]; }
  return NULL;
}

NuwroSystSet &NuwroReWeight::Systematics(void){
  return fSystSet;
}

void NuwroReWeight::Reconfigure(void){

  std::vector<NuwroSyst_t> const &svec = fSystSet.GetAllSysts();

  for(WghtMapIt wght_it = fWghtCalc.begin(); wght_it != fWghtCalc.end();
    ++wght_it) {

    NuwroWghtEngineI * wcalc = wght_it->second;

    for(std::vector<NuwroSyst_t>::const_iterator syst_it = svec.begin();
      syst_it != svec.end(); ++syst_it){
      NuwroSyst_t const &syst = (*syst_it);
      if(wcalc->SystIsHandled(syst)){
        wcalc->SetSystematic(syst, fSystSet.GetSystInfo(syst).CurValue);
      }
    }//params

    wcalc->Reconfigure();
  }
}

double NuwroReWeight::CalcWeight(event * event){

  double weight = 1.0;
  for(WghtMapIt it = fWghtCalc.begin(); it != fWghtCalc.end(); ++it) {
    weight *= it->second->CalcWeight(event);
  }
  return weight;
}

double NuwroReWeight::CalcChisq(void){

  double tot_chisq = 0.0;

  for(WghtMapIt it = fWghtCalc.begin(); it != fWghtCalc.end(); ++it) {
    double chisq = it->second->CalcChisq();
    std::cout << "[INFO]: Penalty calculator " << it->first << ": " << chisq
      << std::endl;
    tot_chisq += chisq;
  }
  return tot_chisq;
}

void NuwroReWeight::Print(){

  std::vector<NuwroSyst_t> const& syst_vec = Systematics().GetAllSysts();

  std::cout << "[INFO]: NuwroReWeight: Current set of systematic params:"
    << std::endl;
  for(std::vector<NuwroSyst_t>::const_iterator syst_it = syst_vec.begin();
    syst_it != syst_vec.end(); ++syst_it){
    std::cout << "\tDial "  << (*syst_it)
      << " is set to " << Systematics().GetSystInfo((*syst_it)).CurValue
      << std::endl;
  }
  std::cout << "[INFO]: Chi2 penalty: " << CalcChisq() << std::endl;
}
