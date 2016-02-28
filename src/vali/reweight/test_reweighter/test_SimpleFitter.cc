//c++
#include <iostream>
#include <vector>

//POSIX
#include <sys/time.h>

//ROOT
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"

#ifdef ROOT6

#include "Minuit2/Minuit2Minimizer.h"
#include "Math/Functor.h"

#else

#include "TFitterMinuit.h"
#include "Minuit2/FCNBase.h"

#endif

#include "TH1D.h"

//nuwro
#include "event1.h"

//nuwro reweighting
#include "NuwroReWeight.h"
#include "NuwroReWeight_MaCCQE.h"
#include "NuwroReWeight_MaRES_CA5.h"
#include "NuwroSyst.h"
#include "NuwroSystSet.h"
#include "RooTrackerEvent.h"

#include "NuwroReWeightSimple.h"

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

using namespace RooTrackerUtils;

#define DEBUG_SIMPFIT
//#define LOUD_EVAL

#ifdef _OPENMP
#define DEBUG_MULTITHREADING
#endif

#define PROFILE

#ifdef _OPENMP
static size_t const NumThreads = 4;
#else
static size_t const NumThreads = 1;
#endif

#ifdef ROOT6
struct NPLLR_nwev {
#else
struct NPLLR_nwev : public ROOT::Minuit2::FCNBase {
#endif
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
      PlotHistos[i]->Reset();
    }
    # pragma omp parallel for
    for(size_t i = 0; i < Events->size(); ++i){
      GenWeights[i] = Events->at(i).weight/double(NInTree)*1E40;
    }
#ifdef DEBUG_MULTITHREADING
    std::vector<Double_t> GenWeights_mtdb;
    GenWeights_mtdb.resize(GenWeights.size());
    for(size_t i = 0; i < Events->size(); ++i){
      GenWeights_mtdb[i] = Events->at(i).weight/double(NInTree)*1E40;
    }
    for(size_t i = 0; i < Events->size(); ++i){
      assert(GenWeights_mtdb[i] == GenWeights[i]);
    }
#endif
  }

  Double_t CalculateTestStat() const {
    double LLR = 0;
    # pragma omp parallel for reduction(+ : LLR)
    for(size_t i = 1; i < Data->GetNbinsX(); ++i){
      Double_t DataBin = Data->GetBinContent(i);
      Double_t MCBin = PlotHistos[0]->GetBinContent(i);
      Double_t LogEVal = ((DataBin>0)&&(MCBin>0))? DataBin*log(DataBin/MCBin) : 0;
#ifdef LOUD_EVAL
      std::cout << "[FIT]: data = " << DataBin << ", MCBin = " << MCBin
        << ", LogEVal " << LogEVal << std::endl;
#endif
      LLR +=  MCBin - DataBin + LogEVal;
    }
#ifdef DEBUG_SIMPFIT
    assert(std::isfinite(LLR));
#endif
    return LLR;
  }
#ifdef ROOT6
  Double_t operator() (Double_t const *x) const {
#else
  Double_t operator() (std::vector<Double_t> const &x) const {
#endif
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

#ifdef DEBUG_MULTITHREADING
    TH1D* PlotHisto_mtdb = static_cast<TH1D*>(PlotHistos[0]->Clone());
    PlotHisto_mtdb->Reset();
    for(size_t i = 0; i < Events->size(); ++i){
      PlotHisto_mtdb->Fill(
        Events->at(i).out[0].momentum(), Weights[i] * GenWeights[i]);
    }

    for(size_t i = 0; i < PlotHisto_mtdb->GetNbinsX()+1; ++i){
      if(fabs(PlotHisto_mtdb->GetBinContent(i) -
          PlotHistos[0]->GetBinContent(i)) > 1E-8){
        std::cout << "[ERROR]: PlotHisto_mtdb->GetBinContent(" << i << ")"
            << " ( = " << PlotHisto_mtdb->GetBinContent(i)
            << ") != PlotHistos[0]->GetBinContent(" << i << ")"
            << " ( = " << PlotHistos[0]->GetBinContent(i) << std::endl;
      }
      assert(fabs(PlotHisto_mtdb->GetBinContent(i) -
          PlotHistos[0]->GetBinContent(i)) < 1E-8);
    }
    delete PlotHisto_mtdb;
#endif

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
    "<Nominal parameters file> <MC Nuwro RooTracker File> <Output File>"
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

  TH1D *FDDistrib = new TH1D("FakeDataDistribution","",15,0,3000);
  FDDistrib->SetDirectory(NULL);
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

  TFile* OutFile = TFile::Open(argv[5],"RECREATE");
  FDDistrib->SetDirectory(OutFile);
  TCanvas * c1 = new TCanvas("FitRes","");
  FDDistrib->Draw("");

  std::vector<Double_t> Weights;
  Weights.resize(SignalEvents.size());

  SRW::SetupSPP();

  std::cout << "[INFO]: Attempting the fittening. " << std::endl;
  NPLLR_nwev Eval(&SignalEvents, FDDistrib, nEntries);
#ifdef ROOT6
  Double_t FitStart[] = {NomParams.pion_C5A, NomParams.pion_axial_mass};
#else
  std::vector<Double_t> FitStart;
  FitStart.push_back(NomParams.pion_C5A);
  FitStart.push_back(NomParams.pion_axial_mass);
#endif
  Eval(FitStart);
  TH1D* PreFit = (TH1D*)Eval.PlotHistos[0]->Clone();

  PreFit->SetDirectory(OutFile);
  PreFit->SetLineColor(kBlue);
  PreFit->SetNameTitle("MCPreFit","MCPreFit");
  PreFit->Write("MCPreFit");
  PreFit->SetDirectory(NULL);
  PreFit->Draw("SAME");

#ifdef ROOT6
  ROOT::Minuit2::Minuit2Minimizer min ( ROOT::Minuit2::kMigrad );
  min.SetMaxFunctionCalls(1000000);
  min.SetMaxIterations(100000);
  min.SetTolerance(0.001);

  ROOT::Math::Functor f(Eval,2);

  min.SetFunction(f);

  // Set the free variables to be minimized!
  min.SetLimitedVariable(0,"CA5",NomParams.pion_C5A,0.1,0,3.0);
  min.SetLimitedVariable(1,"MaRES",NomParams.pion_axial_mass,0.1,0,3.0);
#else
  TFitterMinuit * minuit = new TFitterMinuit();
  minuit->SetMinuitFCN(&Eval);
  minuit->SetPrintLevel(4);
  minuit->SetParameter(0,"CA5",FitStart[0],0.01,0,3.0);
  minuit->SetParameter(1,"MaRES",FitStart[1],0.01,0,3.0);
  minuit->CreateMinimizer();
#endif


#ifdef PROFILE
  Double_t wt1 = get_wall_time();
#endif

#ifdef ROOT6
  min.Minimize();
#else
  int iret = minuit->Minimize();
#endif

#ifdef PROFILE
  Double_t wt2 = get_wall_time();
  std::cout << "[PROFILE]: Minimisation took "
    << (wt2-wt1) << " seconds." << std::endl;
#endif

#ifdef ROOT6
  std::cout << "[INFO]: Nominal parameters: CA5 == " << NomParams.pion_C5A
    << ", MaRES == " << NomParams.pion_axial_mass << std::endl;
  min.PrintResults();
  Eval.PlotHistos[0]->SetDirectory(OutFile);
  OutFile->Write();
  OutFile->Close();
  for(size_t i = 1; i < Eval.PlotHistos.size(); ++i){
    delete Eval.PlotHistos[i];
  }
#else
  std::vector<Double_t> BestFit;
  BestFit.push_back(minuit->GetParameter(0));
  BestFit.push_back(minuit->GetParameter(1));
  Double_t TestStat_BF = Eval(BestFit);
  Eval.PlotHistos[0]->SetLineColor(kRed);
  Eval.PlotHistos[0]->SetLineWidth(2);
  Eval.PlotHistos[0]->SetLineStyle(2);
  Eval.PlotHistos[0]->SetNameTitle("BestFitMC","BestFitMC");
  Eval.PlotHistos[0]->Write("BestFitMC");
  Eval.PlotHistos[0]->Draw("SAME");
  std::vector<Double_t> ActualBF;
  ActualBF.push_back(TargetParams.pion_C5A);
  ActualBF.push_back(TargetParams.pion_axial_mass);
  Double_t TestStat_ABF = Eval(ActualBF);
  Eval.PlotHistos[0]->SetLineColor(kMagenta);
  Eval.PlotHistos[0]->SetLineWidth(2);
  Eval.PlotHistos[0]->SetLineStyle(2);
  Eval.PlotHistos[0]->SetNameTitle("TrueReweightedMC","TrueReweightedMC");
  Eval.PlotHistos[0]->Write("TrueReweightedMC");
  Eval.PlotHistos[0]->Draw("SAME");
  Eval.PlotHistos[0]->SetDirectory(NULL);
  c1->BuildLegend();
  c1->SaveAs("FitResults.pdf");
  std::cout << "[FIT RESULTS]: MC Nom: CA5 = " << NomParams.pion_C5A
    << ", FD True = " << TargetParams.pion_C5A
    << ", Fit Result = " << minuit->GetParameter(0) << std::endl;
  std::cout << "[FIT RESULTS]: MC Nom: MaRES = " << NomParams.pion_axial_mass
    << ", FD True = " << TargetParams.pion_axial_mass
    << ", Fit Result = " << minuit->GetParameter(1) << std::endl;
  std::cout << "[FIT RESULTS]: LLR at best fit: " << TestStat_BF
    << ", at true: " << TestStat_ABF << std::endl;
  delete c1;
  OutFile->Write();
  OutFile->Close();
  // for(size_t i = 1; i < Eval.PlotHistos.size(); ++i){
  //   delete Eval.PlotHistos[i];
  // }
  if (iret != 0) {
    std::cout << "Minimization failed - exit " ;
    return iret;
  }
#endif


}
