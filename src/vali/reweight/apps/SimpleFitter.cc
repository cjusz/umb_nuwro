//c++
#include <iostream>
#include <vector>
#include <sstream>

//POSIX
#include <sys/time.h>

//ROOT
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"

#include "TFitterMinuit.h"
#include "Minuit2/FCNBase.h"

#include "TH1D.h"

//nuwro
#include "event1.h"

//nuwro reweighting
#include "NuwroReWeight.h"
#include "NuwroReWeight_QEL.h"
#include "NuwroReWeight_SPP.h"
#include "NuwroSyst.h"
#include "NuwroSystSet.h"
#include "RooTrackerEvent.h"
#include "NuwroReWeightSimple.h"

//nuwro vali
#include "ValiPlotter.h"

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

std::string num2str(int i){
  std::stringstream ss("");
  ss << i;
  return ss.str();
}

std::string num2str(double i){
  std::stringstream ss("");
  ss << i;
  return ss.str();
}

using namespace RooTrackerUtils;

//#define DEBUG_SIMPFIT

#define PROFILE

#ifdef _OPENMP
static size_t const NumThreads = 4;
#else
static size_t const NumThreads = 1;
#endif


struct NPLLR_nwev : public ROOT::Minuit2::FCNBase {

  std::vector<event> const * Events;
  std::vector<Double_t> GenWeights;
  std::vector<Double_t> mutable NominalWeights;
  std::vector<Double_t> mutable Weights;
  std::vector<TH1D*> mutable PlotHistos;
  Double_t FileWeight;
  TH1D const *Data;

  NPLLR_nwev(std::vector<event> const *evs, TH1D const *data, Long64_t NInTree) :
    GenWeights(), NominalWeights(), Weights(), PlotHistos() {
    Events = evs;
    Data = data;
    NominalWeights.resize(Events->size());
    Weights.resize(Events->size());
    GenWeights.resize(Events->size());
    SRW::GenerateNominalRESWeights((*Events), NominalWeights);
    PlotHistos.resize(NumThreads);
    for(size_t i = 0; i < NumThreads; i++){
      PlotHistos[i] = static_cast<TH1D*>(data->Clone());
      PlotHistos[i]->Sumw2(false);
      PlotHistos[i]->Reset();
    }
    # pragma omp parallel for
    for(size_t i = 0; i < Events->size(); ++i){
      GenWeights[i] = Events->at(i).weight/double(NInTree)*1E40;
    }
  }

  Double_t CalculateTestStat() const {
    double LLR = 0;
    # pragma omp parallel for reduction(+ : LLR)
    for(size_t i = 1; i < Data->GetNbinsX(); ++i){
      Double_t DataBin = Data->GetBinContent(i);
      Double_t MCBin = PlotHistos[0]->GetBinContent(i);
      Double_t LogEVal = ((DataBin>0)&&(MCBin>0))? DataBin*log(DataBin/MCBin) : 0;
      LLR +=  MCBin - DataBin + LogEVal;
    }
#ifdef DEBUG_SIMPFIT
    assert(std::isfinite(LLR));
#endif
    return LLR;
  }

  Double_t operator() (std::vector<Double_t> const &x) const {

    SRW::ReWeightRESEvents((*Events),x[0],x[1], Weights, NominalWeights);

    PlotHistos[0]->Reset();

    # pragma omp parallel for
    for(size_t i = 0; i < Events->size(); ++i){

#ifdef _OPENMP
      PlotHistos[omp_get_thread_num()]->Fill(
#else
      PlotHistos[0]->Fill(
#endif
        Events->at(i).out[0].momentum(), Weights[i] * GenWeights[i]);
    }

    for(size_t i = 1; i < NumThreads; ++i){
      PlotHistos[0]->Add(PlotHistos[i]);
      PlotHistos[i]->Reset();
    }

    Double_t LLR = CalculateTestStat();
#ifdef DEBUG_SIMPFIT
    std::cout << "[FIT]: LLR = " << LLR << " @ x[0] = " << x[0]
      << ", & x[1] = " << x[1] << std::endl;
#endif
    return LLR;
  }

  Double_t Up() const {
    return 0.5;
  }
};

int PlotFakeData(TTree * FDRootracker, TH1D &FDDistrib){
  RooTrackerEvent FDEv;
  if(!FDEv.JackIn(FDRootracker)){
    std::cerr << "[ERROR]: Couldn't Jack the FDRooTrackerEvent into the input"
      " tree. Was it malformed?" << std::endl;
    return 8;
  } else {
    std::cout << "[INFO]: StdHepEvent jacked in, here we go." << std::endl;
  }

  Long64_t nEntries = FDRootracker->GetEntries();

  std::vector<event> SignalEvents;
  std::cout << "[INFO]: Plotting the data events..." << std::endl;

  for(Long64_t ent = 0; ent < nEntries; ++ent){
    std::cout << "\r[LOADING]: " << int((ent+1)*100/nEntries) << "\% data read. "
      << std::flush;
    FDRootracker->GetEntry(ent);
    event nwev = GetNuWroEvent1(FDEv, 1000.0);
    //Look for CCRes
    if( (nwev.dyn != 2) || (!nwev.flag.cc) || (!nwev.flag.anty)){
      continue;
    }
    FDDistrib.Fill(nwev.out[0].momentum(),nwev.weight/double(nEntries)*1E40);
  }
  std::cout << std::endl << "[INFO]: Data plotted!" << std::endl;
  return 0;
}

static const size_t eSize = sizeof(event);
static const size_t pSize = sizeof(particle);

inline size_t NuwroEvSize(event const & ev){
  return ev.in.size()*pSize + ev.temp.size()*pSize + ev.out.size()*pSize +
  ev.post.size()*pSize + ev.all.size()*pSize + eSize;
}

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " <Fake Data parameters file> <Fake Data Nuwro RooTracker File> "
    "<Nominal parameters file> <MC Nuwro RooTracker File> <Output PDF File>"
    << std::endl;
}

int main(int argc, char const *argv[]){
  TH1::SetDefaultSumw2(false);
#ifdef _OPENMP
  omp_set_num_threads(NumThreads);
  std::cout << "[THREADING]: Setting maximum number of concurrent OpenMP "
    "threads to " << NumThreads << std::endl;
#endif

  if(argc != 6){
    std::cerr << "[ERROR]: Found " << (argc-1) << " cli args. Expected 5."
      << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }

  params TargetParams;
  TargetParams.read(argv[1]);
  params NomParams;
  NomParams.read(argv[3]);

  TFile *InputFDFile = TFile::Open(argv[2]);
  if(!InputFDFile || ! InputFDFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << argv[2] << " for reading."
      << std::endl;
    PrintUsage(argv[0]);
    return 2;
  }

  TTree *FDRootracker = dynamic_cast<TTree*>(InputFDFile->Get("nRooTracker"));
  if(!FDRootracker){
    std::cerr << "[ERROR]: Couldn't find TTree (\"nRooTracker\") in file "
      << argv[2] << "." << std::endl;
    PrintUsage(argv[0]);
    return 4;
  }

  TH1D *FDDistrib = new TH1D("FakeDataDistribution","FakeDataDistribution",
    15,0,3000);
  FDDistrib->SetDirectory(NULL);
  // FDDistrib->Sumw2(true);
  if(PlotFakeData(FDRootracker,(*FDDistrib))){
    PrintUsage(argv[0]);
    return 8;
  }


  TFile *InputMCFile = TFile::Open(argv[4]);
  if(!InputMCFile || ! InputMCFile->IsOpen()){
    std::cerr << "[ERROR]: Couldn't open " << argv[4] << " for reading."
      << std::endl;
    PrintUsage(argv[0]);
    return 2;
  }
  TTree *MCRooTracker = dynamic_cast<TTree*>(InputMCFile->Get("nRooTracker"));
  if(!MCRooTracker){
    std::cerr << "[ERROR]: Couldn't find TTree (\"nRooTracker\") in file "
      << argv[4] << "." << std::endl;
    PrintUsage(argv[0]);
    return 4;
  }

  RooTrackerEvent InpMCEv;
  if(!InpMCEv.JackIn(MCRooTracker)){
    std::cerr << "[ERROR]: Couldn't Jack the RooTrackerEvent into the input"
      " tree. Was it malformed?" << std::endl;
    PrintUsage(argv[0]);
    return 8;
  } else {
    std::cout << "[INFO]: StdHepEvent jacked in, here we go." << std::endl;
  }

  Long64_t nEntries = MCRooTracker->GetEntries();
  Long64_t Filled = 0;

  std::vector<event> SignalEvents;
  SignalEvents.reserve(nEntries);
  std::cout << "[INFO]: Loading the signal..." << std::endl;
#ifdef DEBUG_SIMPFIT
  size_t CacheSize = 0;
#endif
  for(Long64_t ent = 0; ent < nEntries; ++ent){
    std::cout << "\r[LOADING]: " << int((ent+1)*100/nEntries) << "\% loaded "
#ifdef DEBUG_SIMPFIT
      << "(" << (CacheSize/size_t(8E6)) << " Mb / " <<
        ((CacheSize+((nEntries-SignalEvents.size())*sizeof(event)))/size_t(8E6))
      << " Mb reserved)"
#endif
      << std::flush;
    MCRooTracker->GetEntry(ent);
    event nwev = GetNuWroEvent1(InpMCEv, 1000.0);
    //Look for CCRes
    if( (nwev.dyn != 2) || (!nwev.flag.cc) || (!nwev.flag.anty)){
      continue;
    }
    SignalEvents.push_back(nwev);
    SignalEvents.back().par = NomParams;
#ifdef DEBUG_SIMPFIT
    CacheSize += NuwroEvSize(nwev);
#endif
  }
  std::cout << std::endl <<
    "[INFO]: Loaded " << SignalEvents.size() << " signal events into memory."
    << std::endl;

  std::vector<Double_t> Weights;
  Weights.resize(SignalEvents.size());

  SRW::SetupSPP();

  std::cout << "[INFO]: Attempting the fittening. " << std::endl;
  NPLLR_nwev Eval(&SignalEvents, FDDistrib, nEntries);

  std::vector<Double_t> FitStart;
  FitStart.push_back(NomParams.pion_C5A);
  FitStart.push_back(NomParams.pion_axial_mass);

  // Eval.PlotHistos[0]->Sumw2(true);
  Eval(FitStart);
  TH1D* PreFit = static_cast<TH1D*>(Eval.PlotHistos[0]->Clone());
  Eval.PlotHistos[0]->Sumw2(false);

  TFitterMinuit * minuit = new TFitterMinuit();
  minuit->SetMinuitFCN(&Eval);
  minuit->SetPrintLevel(4);
  minuit->SetParameter(0,"CA5",FitStart[0],0.01,0,3.0);
  minuit->SetParameter(1,"MaRES",FitStart[1],0.01,0,3.0);
  minuit->CreateMinimizer();

#ifdef PROFILE
  Double_t wt1 = get_wall_time();
#endif

  int iret = minuit->Minimize();

#ifdef PROFILE
  Double_t wt2 = get_wall_time();
  std::cout << "[PROFILE]: Minimisation took "
    << (wt2-wt1) << " seconds." << std::endl;
#endif

  // Eval.PlotHistos[0]->Sumw2(true);
  std::vector<Double_t> BestFit;
  BestFit.push_back(minuit->GetParameter(0));
  BestFit.push_back(minuit->GetParameter(1));
  Double_t TestStat_BF = Eval(BestFit);

  TH1D* MCBestFitH = static_cast<TH1D*>(Eval.PlotHistos[0]->Clone());
  std::vector<Double_t> ActualBF;
  ActualBF.push_back(TargetParams.pion_C5A);
  ActualBF.push_back(TargetParams.pion_axial_mass);
  Double_t TestStat_ABF = Eval(ActualBF);

  TH1D* MCTrueReWeightH = static_cast<TH1D*>(Eval.PlotHistos[0]->Clone());

  Vali::PlotValiFitResults(FDDistrib, PreFit, MCBestFitH, MCTrueReWeightH,
    "#it{p}_{#mu} (MeV/#it{c})", "#frac{d#sigma}{d#it{p}_{#mu}} (x10^{-40})",
    std::string("C_{A}^{5} True: ") + num2str(TargetParams.pion_C5A)
    + ", BF: " + num2str(minuit->GetParameter(0))
    + " || M_{A}^{RES} True: " + num2str(TargetParams.pion_axial_mass)
    + ", BF: " + num2str(minuit->GetParameter(1)),
    argv[5]);

  std::cout << "[FIT RESULTS]: MC Nom: CA5 = " << NomParams.pion_C5A
    << ", FD True = " << TargetParams.pion_C5A
    << ", Fit Result = " << minuit->GetParameter(0) << std::endl;
  std::cout << "[FIT RESULTS]: MC Nom: MaRES = " << NomParams.pion_axial_mass
    << ", FD True = " << TargetParams.pion_axial_mass
    << ", Fit Result = " << minuit->GetParameter(1) << std::endl;
  std::cout << "[FIT RESULTS]: LLR at best fit: " << TestStat_BF
    << ", at true: " << TestStat_ABF << std::endl;

  if (iret != 0) {
    std::cout << "Minimization failed - exit " ;
    return iret;
  }

}
