#include <iostream>
#include <sstream>

#include "TFile.h"
#include "TTree.h"

#include "event1.h"

#include "DialUtils.h"

using namespace nuwro::rew;


void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " [params file for nominal] <dial to reweight>" << std::endl;
  PrintDialsImplemented();
}

std::string GetDialValStr(Double_t dialval){
  std::stringstream ss;
  if(dialval < 0){
    ss << "minus";
  }
  ss << fabs(dialval);
  return ss.str();
}

int main(int argc, char const *argv[]){

  if( (argc < 2) || (argc > 3) ){
    std::cout << "[ERROR]: Expected 1 or 2 CLI arguments." << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }

  params NomParams;
  int argu = 1;
  if(argc == 3){
    NomParams.read(argv[argu]);
    argu++;
  }

  nuwro::rew::NuwroReWeight_SPP::DoSetupSPP = false;

  NuwroReWeight WghtGen;

  std::vector<ENuWroSyst> Systs;
  std::vector<NuwroSystInfo*> SystInstances;
  std::vector<std::string> WghtEngineNames;
  std::string DialName(argv[argu]);
  ENuWroSyst syst = NuwroSyst::FromString(DialName);

  NuwroWghtEngineI* wghtCalc =
    GetWghtEngineFromDial(syst,NomParams);
  if(!wghtCalc){
    std::cout << "[ERROR]: Invalid Dial, " << syst << ", (from string: \""
      << DialName << "\") specified." << std::endl;
    PrintDialsImplemented();
    return 2;
  }

  WghtGen.AdoptWghtCalc(DialName, wghtCalc);

  WghtGen.Systematics().Add(syst, 0, -4, 4, 2);
  NuwroSystInfo & si = WghtGen.Systematics().GetSystInfo(syst);

  std::stringstream ss;
  si.TurnDown();
  WghtGen.Reconfigure();
  ss << wghtCalc->GetSystematicValue(syst);

  ss << "\n";

  si.TurnUp();
  WghtGen.Reconfigure();
  ss << wghtCalc->GetSystematicValue(syst);

  ss << "\n";

  si.TurnUp();
  WghtGen.Reconfigure();
  ss << wghtCalc->GetSystematicValue(syst);

  std::cout << ss.str() << std::endl;
  return 0;
}
