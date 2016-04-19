#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "event1.h"

#include "NuwroReWeight.h"
#include "NuwroReWeight_QEL.h"
#include "NuwroReWeight_FlagNorm.h"
#include "NuwroReWeight_SPP.h"
#include "NuwroSyst.h"
#include "NuwroSystSet.h"
#include "NuwroReWeightSimple.h"

//#define TW_DEBUG

#include "DialUtils.h"

using namespace nuwro::rew;

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " <input NuWro eventsout file> <output weights file name> "
    "<dial name to reweight> [dial name to reweight, ...]" << std::endl;
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

bool IsSignal(event const & ev){
  return true;
}

int main(int argc, char const *argv[]){

  if(argc < 4){
    std::cout << "[ERROR]: Expected at least 3 CLI arguments." << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }

  std::vector<event> evs;
  SRW::LoadSignalNuWroEventsIntoVector(argv[1], evs, &IsSignal, 500000);

  params NomParams = evs[0].par; //Assume that all events in the file have the
  //same theory params.

  NuwroReWeight WghtGen;

  int argu = 3;
  std::vector<ENuWroSyst> Systs;
  std::vector<NuwroSystInfo*> SystInstances;
  std::vector<std::string> WghtEngineNames;
  while (argu < argc){ // leave space for the other files we need
    std::string DialName(argv[argu]);
    ENuWroSyst syst = NuwroSyst::FromString(DialName);

    NuwroWghtEngineI* wghtCalc = NULL;
    bool AddNew = true;
    //Check if we already have a weight engine that can handle this dial
    for(size_t i = 0; i < WghtEngineNames.size(); ++i){
      NuwroWghtEngineI* prevEngine = WghtGen.WghtCalc(WghtEngineNames[i]);
      if(!prevEngine){
        std::cout << "[WARN]: Seem to have lost the weight engine for "
          << WghtEngineNames[i] << std::endl;
        continue;
      }
      if(prevEngine->SystIsHandled(syst)){
        AddNew = false;
        std::cout << "[INFO]: The weight engine for " << WghtEngineNames[i]
          << " can also handle " << DialName << std::endl;
      }
    }

    if(AddNew){
      NuwroWghtEngineI* wghtCalc =
        GetWghtEngineFromDial(syst,NomParams);
      if(!wghtCalc){
        std::cout << "[ERROR]: Invalid Dial, " << syst << ", (from string: \""
          << DialName << "\") specified." << std::endl;
        PrintDialsImplemented();
        return 32;
      } else {
        WghtGen.AdoptWghtCalc(DialName, wghtCalc);
        WghtEngineNames.push_back(DialName);
      }
    }

    WghtGen.Systematics().Add(syst, 0, -4, 4, 2);
    SystInstances.push_back(&WghtGen.Systematics().GetSystInfo(syst));
    Systs.push_back(syst);
    std::cout << "[INFO]: Test reweighting " << syst << std::endl;
    argu++;
  }

  TFile* outputFile = TFile::Open(argv[2],"RECREATE");
  if(!outputFile || ! outputFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << argv[2] << " for writing."
      << std::endl;
    PrintUsage(argv[0]);
    return 16;
  }

  TTree* outputTree = new TTree("weights","");

  //LABEL: Adding weight branches
  std::vector< std::pair<std::string, Double_t> > weights;
  for(size_t i = 0; i < SystInstances.size(); i++){
    NuwroSystInfo &d1Inst = (*SystInstances[i]);
    std::string d1Name = NuwroSyst::AsString(Systs[i]);
    std::string d1_valn = GetDialValStr(d1Inst.CurValue);
    d1Inst.TurnUp(1);
    std::string d1_valu = GetDialValStr(d1Inst.CurValue);

    std::string bname1 = "weight_" + d1Name + "_" + d1_valn + "_to_"
      + d1_valu;
    weights.push_back(std::make_pair(bname1,0));

    for(size_t j = (i+1); j < SystInstances.size(); j++){
      NuwroSystInfo &d2Inst = (*SystInstances[j]);
      std::string d2Name = NuwroSyst::AsString(Systs[j]);
      std::string d2_valn = GetDialValStr(d2Inst.CurValue);
      d2Inst.TurnUp(1);
      std::string d2_valu = GetDialValStr(d2Inst.CurValue);
      d2Inst.TurnDown(2);
      std::string d2_vald = GetDialValStr(d2Inst.CurValue);
      std::string bname11 = "weight_" + d1Name + "_" + d1_valn + "_to_"
        + d1_valu + "_" + d2Name + "_" + d2_valn + "_to_" + d2_valu;
      std::string bname12 = "weight_" + d1Name + "_" + d1_valn + "_to_"
        + d1_valu + "_" + d2Name + "_" + d2_valn + "_to_" + d2_vald;
      weights.push_back(std::make_pair(bname11,0));
      weights.push_back(std::make_pair(bname12,0));
      std::cout << "[INFO]: Adding branches for: \n\t" << bname11
        << "\n\t" << bname12 << std::endl;
      d2Inst.TurnUp(1);
    }

    d1Inst.TurnDown(2);
    std::string d1_vald = GetDialValStr(d1Inst.CurValue);
    std::string bname2 = "weight_" + d1Name + "_" + d1_valn + "_to_"
      + d1_vald;
    //to make sure that the ordering is correct.
    weights.push_back(std::make_pair(bname2,0));

    for(size_t j = (i+1); j < SystInstances.size(); j++){
      NuwroSystInfo &d2Inst = (*SystInstances[j]);
      std::string d2Name = NuwroSyst::AsString(Systs[j]);
      std::string d2_valn = GetDialValStr(d2Inst.CurValue);
      d2Inst.TurnUp();
      std::string d2_valu = GetDialValStr(d2Inst.CurValue);
      d2Inst.TurnDown(2);
      std::string d2_vald = GetDialValStr(d2Inst.CurValue);
      std::string bname21 = "weight_" + d1Name + "_" + d1_valn + "_to_"
        + d1_vald + "_" + d2Name + "_" + d2_valn + "_to_" + d2_valu;
      std::string bname22 = "weight_" + d1Name + "_" + d1_valn + "_to_"
        + d1_vald + "_" + d2Name + "_" + d2_valn + "_to_" + d2_vald;
      weights.push_back(std::make_pair(bname21,0));
      weights.push_back(std::make_pair(bname22,0));
      std::cout << "[INFO]: Adding branches for: \n\t" << bname21
        << "\n\t" << bname22 << std::endl;
      d2Inst.TurnUp(1);
    }
    std::cout << "[INFO]: Adding branches for: \n\t" << bname1
      << "\n\t" << bname2 << std::endl;
    d1Inst.TurnUp(1);
  }

  for(size_t i = 0; i < weights.size(); ++i){
    outputTree->Branch(weights[i].first.c_str(),&(weights[i].second));
  }

  size_t nEntries = evs.size();
  size_t Filled = 0;

  for(size_t ent = 0; ent < nEntries; ++ent){
    event &nwev = evs[ent];

    // The ordering of dial twiddling and CalcWeight must be kept in sync with
    // the above loop @ LABEL: Adding weight branches. Or a more sophisticated
    // method of synching weight values and dial values. e.g. could be std::map
    // (hash) of doubles rather than a vector of branch name:double.
    size_t wghtctr = 0;
    for(size_t i = 0; i < SystInstances.size(); i++){
      NuwroSystInfo &d1Inst = (*SystInstances[i]);

      d1Inst.TurnUp(1); WghtGen.Reconfigure();
#ifdef TW_DEBUG
      std::cout << "[RW]: " << NuwroSyst::AsString(Systs[i]) << " -> "
        << d1Inst.CurValue << ", => " << weights[wghtctr].first << std::endl;
#endif
      weights[wghtctr].second = WghtGen.CalcWeight(&nwev); wghtctr++;

      for(size_t j = (i+1); j < SystInstances.size(); j++){
        NuwroSystInfo &d2Inst = (*SystInstances[j]);
        d2Inst.TurnUp(1); WghtGen.Reconfigure();
#ifdef TW_DEBUG
        std::cout << "[RW]:\t +" << NuwroSyst::AsString(Systs[j]) << " -> "
          << d2Inst.CurValue << ", => " << weights[wghtctr].first << std::endl;
#endif
        weights[wghtctr].second = WghtGen.CalcWeight(&nwev); wghtctr++;
        d2Inst.TurnDown(2); WghtGen.Reconfigure();
#ifdef TW_DEBUG
        std::cout << "[RW]:\t +" << NuwroSyst::AsString(Systs[j]) << " -> "
          << d2Inst.CurValue << ", => " << weights[wghtctr].first << std::endl;
#endif
        weights[wghtctr].second = WghtGen.CalcWeight(&nwev); wghtctr++;
        d2Inst.TurnUp(1);
#ifdef TW_DEBUG
        std::cout << "[RW]:\t +" << NuwroSyst::AsString(Systs[j]) << " -> "
          << d2Inst.CurValue << std::endl;
#endif
      }

      d1Inst.TurnDown(2); WghtGen.Reconfigure();
#ifdef TW_DEBUG
      std::cout << "[RW]: " << NuwroSyst::AsString(Systs[i]) << " -> "
        << d1Inst.CurValue << ", => " << weights[wghtctr].first << std::endl;
#endif
      weights[wghtctr].second = WghtGen.CalcWeight(&nwev); wghtctr++;

      for(size_t j = (i+1); j < SystInstances.size(); j++){
        NuwroSystInfo &d2Inst = (*SystInstances[j]);
        d2Inst.TurnUp(1); WghtGen.Reconfigure();
#ifdef TW_DEBUG
        std::cout << "[RW]:\t +" << NuwroSyst::AsString(Systs[j]) << " -> "
          << d2Inst.CurValue << ", => " << weights[wghtctr].first << std::endl;
#endif
        weights[wghtctr].second = WghtGen.CalcWeight(&nwev); wghtctr++;
        d2Inst.TurnDown(2); WghtGen.Reconfigure();
#ifdef TW_DEBUG
        std::cout << "[RW]:\t +" << NuwroSyst::AsString(Systs[j]) << " -> "
          << d2Inst.CurValue << ", => " << weights[wghtctr].first << std::endl;
#endif
        weights[wghtctr].second = WghtGen.CalcWeight(&nwev); wghtctr++;
        d2Inst.TurnUp(1);
#ifdef TW_DEBUG
        std::cout << "[RW]:\t +" << NuwroSyst::AsString(Systs[j]) << " -> "
          << d2Inst.CurValue << std::endl;
#endif
      }
      d1Inst.TurnUp(1);
#ifdef TW_DEBUG
      std::cout << "[RW]: " << NuwroSyst::AsString(Systs[i]) << " -> "
        << d1Inst.CurValue << std::endl;
#endif
    }

    outputTree->Fill(); Filled++;
    std::cout << "\r" << int((ent+1)*100/nEntries) << "\% done."
#ifdef TW_DEBUG
      << std::endl;
#else
      << std::flush;
#endif
  }

  std::cout << std::endl << "[INFO]: Wrote " << Filled << " entries."
    << std::endl;
  outputTree->Write();
  outputFile->Write();
  outputFile->Close();
}
