#ifdef DEBUG_SRW
#include <stdexcept>
#include <cassert>
#endif

#include <iostream>

#include <omp.h>

#include "TFile.h"
#include "TTree.h"

#include "NuwroReWeight_MaRES_CA5.h"

#include "SimpleAnalysisFormat.h"

#define DEBUG_SRW
#ifdef _OPENMP
//#define DEBUG_SRW_OMP
#endif

namespace SRW {

static const size_t eSize = sizeof(event);
static const size_t pSize = sizeof(particle);

inline size_t NuwroEvSize(event const & ev){
  return ev.in.size()*pSize + ev.temp.size()*pSize + ev.out.size()*pSize +
  ev.post.size()*pSize + ev.all.size()*pSize + eSize;
}

inline void SetupSPP(){
  nuwro::rew::SetupSPP();
}
inline void SetupSPP(params const &p){
  params p_dummy = p;
  nuwro::rew::SetupSPP(p_dummy);
}

inline Double_t GetRESWeight(event const & nwEv,
  Double_t to_CA5,
  Double_t to_MaRES,
  Double_t NominalWeight=0xDEADBEEF){

  params rwParams = nwEv.par;

  if(NominalWeight==0xDEADBEEF){
    NominalWeight = nuwro::rew::GetWghtPropToResXSec(nwEv,rwParams);
  }

  rwParams.pion_C5A = to_CA5;
  rwParams.pion_axial_mass = to_MaRES;

#ifdef DEBUG_SRW
  Double_t NewWeight = nuwro::rew::GetWghtPropToResXSec(nwEv,rwParams);
  assert(std::isfinite(NominalWeight));
  assert(std::isfinite(NewWeight));
  assert(std::isfinite(NewWeight/NominalWeight));
  return (NewWeight/NominalWeight);
#else
  return (nuwro::rew::GetWghtPropToResXSec(nwEv,rwParams)/NominalWeight);
#endif
}

inline void ReWeightRESEvents(std::vector<event> const &nwEvs,
  Double_t to_CA5,
  Double_t to_MaRES, std::vector<Double_t> &OutWeights,
  std::vector<Double_t> const&InWeights=std::vector<Double_t>()){

  bool HaveNomWeights = InWeights.size();

#ifdef DEBUG_SRW
  if(nwEvs.size()!=OutWeights.size()){
    std::cout << "[ERROR](" << __FILE__ << ":" << __LINE__
      << "): Recieved " << nwEvs.size() << " NuWro events and "
      << OutWeights.size() << " space for weights." << std::endl;
    throw std::exception();
  }
#endif

#ifdef DEBUG_SRW_OMP
  std::vector<Int_t> ThreadEvs;
  ThreadEvs.resize(11);
#endif

  # pragma omp parallel for
  for(size_t i = 0; i < nwEvs.size(); ++i){
    OutWeights[i] = GetRESWeight(nwEvs[i],to_CA5,to_MaRES,
      HaveNomWeights ? InWeights[i] :
                      0xDEADBEEF);
#ifdef DEBUG_SRW_OMP
    if(omp_get_thread_num()<9){
      ThreadEvs[omp_get_thread_num()]++;
    } else { ThreadEvs[10]++; }
#endif
  }


#ifdef DEBUG_SRW_OMP
  for(size_t i = 0; i < (ThreadEvs.size()-1); ++i){
    if(!ThreadEvs[i]){ continue; }
    std::cout << "[INFO]: Reweighting Thread #" << i << ", handled "
      << ThreadEvs[i] << " events." << std::endl;
  }
  if(ThreadEvs[10]){
    std::cout << "[INFO]: Other threads handled "
            << ThreadEvs[10] << " events." << std::endl;
  }
#endif
}

inline void GenerateNominalRESWeights(std::vector<event> const &nwEvs,
  std::vector<Double_t> &NominalWeights){
#ifdef DEBUG_SRW
  if(nwEvs.size() != NominalWeights.size()){
    std::cout << "[ERROR] (" << __FILE__ << ":" << __LINE__
      << "): Recieved " << nwEvs.size()
      << " NuWro events and " << NominalWeights.size() << " space for "
      "nominal weights." << std::endl;
    throw std::exception();
  }
#endif

  # pragma omp parallel for
  for(size_t i = 0; i < nwEvs.size(); ++i){
    NominalWeights[i] =
      nuwro::rew::GetWghtPropToResXSec(nwEvs[i],nwEvs[i].par, false);
  }

}

typedef bool(*SignalFunction)(PODSimpleAnalysisFormat const &ev);

inline bool LoadTree(std::string FileName, std::string TreeName, TFile * &file,
  TTree * &tree){

  file = TFile::Open(FileName.c_str());
  if(!file || ! file->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << FileName << " for reading."
      << std::endl;
    return false;
  }

  tree = dynamic_cast<TTree*>(file->Get(TreeName.c_str()));
  if(!tree){
    std::cerr << "[ERROR]: Couldn't find TTree (\"" << "TreeName"
      << "\") in file " << FileName << "." << std::endl;
    return false;
  }
  return true;
}

inline bool LoadSignalEventsIntoVector(std::string NuWroEvFileName,
  std::vector<event> &SignalEvents,
  std::vector<PODSimpleAnalysisFormat>& SignalSAFs, SignalFunction IsSignal){

  TFile *NuWroEvFile = NULL;
  TTree *NuWroEvTree = NULL;

  if( (!LoadTree(NuWroEvFileName,"treeout",NuWroEvFile,NuWroEvTree)) ){
    return false;
  }

  event *nwEv = 0; // ROOT is in control of object life cycle
  NuWroEvTree->SetBranchAddress("e", &nwEv);

  Long64_t nEntries = NuWroEvTree->GetEntries();
  Long64_t Loaded = 0;

  SignalEvents.reserve(nEntries);
  SignalSAFs.reserve(nEntries);
  std::cout << "[INFO]: Loading the signal..." << std::endl;
  size_t CacheSize = 0;
  for(Long64_t ent = 0; ent < nEntries; ++ent){
    size_t CacheSize_Mb = (CacheSize/size_t(8E6));
    size_t Res_Mb = ((CacheSize
        + ((SignalEvents.capacity()-SignalEvents.size())*sizeof(event))
        + ((SignalSAFs.capacity()-SignalSAFs.size())*sizeof(event)) ) /
      size_t(8E6));
    std::cout << "\r[LOADING]: " << int((ent+1)*100/nEntries) << "\% loaded "
      << "(" << CacheSize_Mb << " Mb / " << Res_Mb << " Mb reserved)"
      << std::flush;
    NuWroEvTree->GetEntry(ent);

    event const & nwev = (*nwEv);

    PODSimpleAnalysisFormat const &podsaf =
      MakePODSimpleAnalysisFormat(nwev);

    if(!IsSignal(podsaf)){
      continue;
    }
    SignalEvents.push_back(nwev);
    CacheSize += NuwroEvSize(nwev);
    SignalSAFs.push_back(podsaf);
    //Add proper xsec norm
    SignalSAFs.back().EvtWght /= double(nEntries);
    CacheSize += sizeof(podsaf);
    Loaded++;
  } std::cout << std::endl;
  std::cout << "[INFO]: Loaded " << Loaded << " signal events out of "
    << nEntries << " input events." << std::endl;

  NuWroEvFile->Close();
  return true;
}

}
