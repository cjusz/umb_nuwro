#include "TFile.h"
#include "TTree.h"

#include "NuwroReWeightSimpleEvent.h"
#include "SimpleAnalysisFormat.h"

namespace SRW {

static const size_t eSize = sizeof(event);
static const size_t pSize = sizeof(particle);
static const size_t safSize = sizeof(PODSimpleAnalysisFormat);
static const size_t srweSize = sizeof(SRWEvent);

inline size_t NuwroEvSize(event const &ev) {
  return ev.in.size() * pSize + ev.temp.size() * pSize + ev.out.size() * pSize +
         ev.post.size() * pSize + ev.all.size() * pSize + eSize;
}

typedef bool (*SAFSignalFunction)(PODSimpleAnalysisFormat const &ev);
typedef bool (*NuWroSignalFunction)(event const &ev);

inline bool LoadTree(std::string FileName, std::string TreeName, TFile *&file,
                     TTree *&tree) {
  file = TFile::Open(FileName.c_str());
  if (!file || !file->IsOpen()) {
    std::cerr << "[ERROR]: Couldn't open " << FileName << " for reading."
              << std::endl;
    return false;
  }

  tree = dynamic_cast<TTree *>(file->Get(TreeName.c_str()));
  if (!tree) {
    std::cerr << "[ERROR]: Couldn't find TTree (\""
              << "TreeName"
              << "\") in file " << FileName << "." << std::endl;
    return false;
  }
  return true;
}

inline bool LoadSignalEventsIntoVector(
    std::string NuWroEvFileName, std::vector<event> &SignalEvents,
    std::vector<PODSimpleAnalysisFormat> &SignalSAFs,
    SAFSignalFunction IsSignal,
    Long64_t LoadNoMoreThan = std::numeric_limits<Long64_t>::max()) {
  TFile *NuWroEvFile = NULL;
  TTree *NuWroEvTree = NULL;

  if ((!LoadTree(NuWroEvFileName, "treeout", NuWroEvFile, NuWroEvTree))) {
    return false;
  }

  event *nwEv = 0;  // ROOT is in control of object life cycle
  NuWroEvTree->SetBranchAddress("e", &nwEv);

  Long64_t nEntries = std::min(NuWroEvTree->GetEntries(), LoadNoMoreThan);
  Long64_t Loaded = 0;

#ifndef SRW_DONT_LOAD_EVENTS
  SignalEvents.reserve(nEntries);
  SignalSAFs.reserve(nEntries);
#endif
  std::cout << "[INFO]: Loading the signal..." << std::endl;
  size_t CacheSize = 0;
  for (Long64_t ent = 0; ent < nEntries; ++ent) {
#ifdef SRW_DONT_LOAD_EVENTS
    if (!(ent % 1000)) {
      std::cout << "\r[READING]: " << int((ent + 1) * 100 / nEntries)
                << "\% read " << std::flush;
    }
#else
    size_t CacheSize_Mb = (CacheSize / double(1E6));
    size_t Res_Mb =
        ((CacheSize +
          ((SignalEvents.capacity() - SignalEvents.size()) * eSize) +
          ((SignalSAFs.capacity() - SignalSAFs.size()) * safSize)) /
         double(1E6));
    if (!(ent % 1000)) {
      std::cout << "\r[LOADING]: " << int((ent + 1) * 100 / nEntries)
                << "\% loaded "
                << "(" << CacheSize_Mb << " Mb / " << Res_Mb
                << " Mb reserved -- Usage Nw: " << SignalEvents.size() << "/"
                << SignalEvents.capacity() << ", SAFs: " << SignalSAFs.size()
                << "/" << SignalSAFs.capacity() << " )" << std::flush;
    }
#endif
    NuWroEvTree->GetEntry(ent);

    event const &nwev = (*nwEv);

    PODSimpleAnalysisFormat const &podsaf = MakePODSimpleAnalysisFormat(nwev);

    if (!IsSignal(podsaf)) {
      continue;
    }
#ifndef SRW_DONT_LOAD_EVENTS
    SignalEvents.push_back(nwev);
    CacheSize += NuwroEvSize(nwev);
    SignalSAFs.push_back(podsaf);
    // Add proper xsec norm
    SignalSAFs.back().EvtWght /= double(nEntries);
    CacheSize += safSize;
#endif
    Loaded++;
  }
#ifdef SRW_DONT_LOAD_EVENTS
  std::cout << "[INFO]: Read " << Loaded << " signal events out of " << nEntries
            << " input events." << std::endl;
#else
  std::cout << "\r[LOADING]: 100\% loaded "
            << "(" << (CacheSize / double(1E6)) << " Mb / "
            << ((CacheSize +
                 ((SignalEvents.capacity() - SignalEvents.size()) * eSize) +
                 ((SignalSAFs.capacity() - SignalSAFs.size()) * safSize)) /
                double(1E6))
            << " Mb reserved -- Usage Nw: " << SignalEvents.size() << "/"
            << SignalEvents.capacity() << ", SAFs: " << SignalSAFs.size() << "/"
            << SignalSAFs.capacity() << " )" << std::endl;
  std::cout << "[INFO]: Loaded " << Loaded << " signal events out of "
            << nEntries << " input events." << std::endl;
#endif
  NuWroEvFile->Close();
  return true;
}

inline bool LoadSignalEventsIntoSAFVector(
    std::string NuWroEvFileName, std::vector<PODSimpleAnalysisFormat> &SAFs,
    SAFSignalFunction IsSignal,
    Long64_t LoadNoMoreThan = std::numeric_limits<Long64_t>::max()) {
  TFile *NuWroEvFile = NULL;
  TTree *NuWroEvTree = NULL;

  if ((!LoadTree(NuWroEvFileName, "treeout", NuWroEvFile, NuWroEvTree))) {
    return false;
  }

  event *nwEv = 0;  // ROOT is in control of object life cycle
  NuWroEvTree->SetBranchAddress("e", &nwEv);

  Long64_t nEntries = std::min(NuWroEvTree->GetEntries(), LoadNoMoreThan);
  Long64_t Loaded = 0;

  SAFs.reserve(nEntries);
  std::cout << "[INFO]: Loading the SAF event signal..." << std::endl;
  size_t CacheSize = 0;
  for (Long64_t ent = 0; ent < nEntries; ++ent) {
    size_t CacheSize_Mb = (CacheSize / double(1E6));
    size_t Res_Mb = ((CacheSize + ((SAFs.capacity() - SAFs.size()) * safSize)) /
                     double(1E6));
    if (!(ent % 1000)) {
      std::cout << "\r[LOADING]: " << int((ent + 1) * 100 / nEntries)
                << "\% loaded "
                << "(" << CacheSize_Mb << " Mb / " << Res_Mb << " Mb reserved)"
                << std::flush;
    }
    NuWroEvTree->GetEntry(ent);

    event const &nwev = (*nwEv);

    PODSimpleAnalysisFormat const &podsaf = MakePODSimpleAnalysisFormat(nwev);

    if (!IsSignal(podsaf)) {
      continue;
    }
    SAFs.push_back(podsaf);
    // Add proper xsec norm
    SAFs.back().EvtWght /= double(nEntries);
    CacheSize += safSize;
    Loaded++;
  }
  std::cout << "\r[LOADING]: 100\% loaded "
            << "(" << (CacheSize / double(1E6)) << " Mb / "
            << ((CacheSize + ((SAFs.capacity() - SAFs.size()) * safSize)) /
                     double(1E6))
            << " Mb reserved)" << std::endl;
  std::cout << "[INFO]: Loaded " << Loaded << " SAF signal events out of "
            << nEntries << " input events." << std::endl;

  NuWroEvFile->Close();
  return true;
}

inline bool LoadSignalNuWroEventsIntoVector(
    std::string NuWroEvFileName, std::vector<event> &evs,
    NuWroSignalFunction IsSignal,
    Long64_t LoadNoMoreThan = std::numeric_limits<Long64_t>::max()) {
  TFile *NuWroEvFile = NULL;
  TTree *NuWroEvTree = NULL;

  if ((!LoadTree(NuWroEvFileName, "treeout", NuWroEvFile, NuWroEvTree))) {
    return false;
  }

  event *nwEv = 0;  // ROOT is in control of object life cycle
  NuWroEvTree->SetBranchAddress("e", &nwEv);

  Long64_t nEntries = std::min(NuWroEvTree->GetEntries(), LoadNoMoreThan);
  Long64_t Loaded = 0;

  evs.reserve(nEntries);
  std::cout << "[INFO]: Loading the event signal..." << std::endl;
  size_t CacheSize = 0;
  for (Long64_t ent = 0; ent < nEntries; ++ent) {
    double CacheSize_Mb = (CacheSize / double(1E6));
    double Res_Mb =
        ((CacheSize + ((evs.capacity() - evs.size()) * eSize)) / double(1E6));

    if (Res_Mb < 1) {
      std::cout.precision(2);
    } else {
      CacheSize_Mb = int(CacheSize_Mb);
      Res_Mb = int(Res_Mb);
    }
    if (!(ent % 1000)) {
      std::cout << "\r[LOADING]: " << int((ent + 1) * 100 / nEntries)
                << "\% loaded "
                << "(" << CacheSize_Mb << " Mb / " << Res_Mb << " Mb reserved)"
                << std::flush;
    }
    NuWroEvTree->GetEntry(ent);
    std::cout.unsetf(std::ios::floatfield);
    event const &nwev = (*nwEv);

    if (!IsSignal(nwev)) {
      continue;
    }
    evs.push_back(nwev);
    // Add proper xsec norm
    evs.back().weight /= double(nEntries);
    CacheSize += NuwroEvSize(nwev);
    Loaded++;
  }
  std::cout << "\r[LOADING]: 100\% loaded "
            << "(" << (CacheSize / double(1E6)) << " Mb / "
            << ((CacheSize + ((evs.capacity() - evs.size()) * srweSize)) /
                double(1E6))
            << " Mb reserved)" << std::endl;
  std::cout << "[INFO]: Loaded " << Loaded << " NuWro signal events out of "
            << nEntries << " input events." << std::endl;

  NuWroEvFile->Close();
  return true;
}

inline bool LoadSignalSRWEventsIntoVector(
    std::string NuWroEvFileName, std::vector<SRWEvent> &evs,
    NuWroSignalFunction IsSignal,
    Long64_t LoadNoMoreThan = std::numeric_limits<Long64_t>::max()) {
  TFile *NuWroEvFile = NULL;
  TTree *NuWroEvTree = NULL;

  if ((!LoadTree(NuWroEvFileName, "treeout", NuWroEvFile, NuWroEvTree))) {
    return false;
  }

  event *nwEv = 0;  // ROOT is in control of object life cycle
  NuWroEvTree->SetBranchAddress("e", &nwEv);

  Long64_t nEntries = std::min(NuWroEvTree->GetEntries(), LoadNoMoreThan);
  Long64_t Loaded = 0;

  evs.reserve(nEntries);
  std::cout << "[INFO]: Loading the event signal..." << std::endl;
  size_t CacheSize = 0;
  for (Long64_t ent = 0; ent < nEntries; ++ent) {
    double CacheSize_Mb = (CacheSize / double(1E6));
    double Res_Mb = ((CacheSize + ((evs.capacity() - evs.size()) * srweSize)) /
                     double(1E6));

    if (Res_Mb < 1) {
      std::cout.precision(2);
    } else {
      CacheSize_Mb = int(CacheSize_Mb);
      Res_Mb = int(Res_Mb);
    }
    if (!(ent % 1000)) {
      std::cout << "\r[LOADING]: " << int((ent + 1) * 100 / nEntries)
                << "\% loaded "
                << "(" << CacheSize_Mb << " Mb / " << Res_Mb << " Mb reserved)"
                << std::flush;
    }
    NuWroEvTree->GetEntry(ent);
    std::cout.unsetf(std::ios::floatfield);
    event const &nwev = (*nwEv);

    if (!IsSignal(nwev)) {
      continue;
    }
    evs.push_back(SRWEvent(nwev));
    // Add proper xsec norm
    evs.back().NominalWeight /= double(nEntries);
    CacheSize += srweSize;
    Loaded++;
  }
  std::cout << "\r[LOADING]: 100\% loaded "
            << "(" << (CacheSize / double(1E6)) << " Mb / "
            << ((CacheSize + ((evs.capacity() - evs.size()) * srweSize)) /
                double(1E6))
            << " Mb reserved)" << std::endl;
  std::cout << "[INFO]: Loaded " << Loaded << " SRW signal events out of "
            << nEntries << " input events." << std::endl;

  NuWroEvFile->Close();
  return true;
}
}
