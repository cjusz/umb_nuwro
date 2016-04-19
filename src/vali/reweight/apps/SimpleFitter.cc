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

#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

#include "TH1D.h"

//nuwro
#include "event1.h"

//nuwro reweighting
#include "NuwroReWeight.h"
#include "NuwroReWeight_QEL.h"
#include "NuwroReWeight_SPP.h"
#include "NuwroSyst.h"
#include "NuwroSystSet.h"
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

template <size_t N>
std::ostream & operator<< (std::ostream &os, Double_t (&a)[N]){
  os << "{ ";
  for(size_t i = 0; i < N; ++i){
    os << a[i] << ((i==(N-1)) ? ", ": "}" );
  }
  return os << std::flush;
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

#define DEBUG_SIMPFIT

#define PROFILE

#ifdef _OPENMP
static size_t NumThreads = 4;
#else
static size_t NumThreads = 1;
#endif


bool IsSignal(event const & ev){
  return ev.flag.cc && ev.flag.res && (!ev.flag.anty);
}

struct NPLLR_nwev {

  std::vector<event> Events;
  std::vector<Double_t> mutable NominalWeights;
  std::vector<Double_t> mutable Weights;
  std::vector<TH1D*> mutable PlotHistos;
  Double_t FileWeight;
  TH1D const *Data;

  NPLLR_nwev(TH1D const *data, char const *InputFileName) :
    NominalWeights(), Weights(), PlotHistos() {
    SRW::LoadSignalNuWroEventsIntoVector(InputFileName, Events, &IsSignal);
    if(!Events.size()){
      std::cerr << "[ERROR]: MC Selection contains no events!!!!" << std::endl;
      throw 5;
    }
    SRW::SetupSPP(Events.front().par);
    NominalWeights.resize(Events.size());
    Weights.resize(Events.size());
    SRW::GenerateNominalWeights(Events, NominalWeights);

    Data = data;
    PlotHistos.resize(NumThreads);
    for(size_t i = 0; i < NumThreads; i++){
      PlotHistos[i] = static_cast<TH1D*>(data->Clone());
      PlotHistos[i]->Sumw2(false);
      PlotHistos[i]->Reset();
    }
  }

  Double_t CalculateTestStat() const {
    double LLR = 0;
    // # pragma omp parallel for reduction(+ : LLR)
    for(size_t i = 1; i < Data->GetNbinsX(); ++i){
      Double_t DataBin = Data->GetBinContent(i);
      Double_t MCBin = (PlotHistos[0]->GetBinContent(i) > 0) ?
        PlotHistos[0]->GetBinContent(i) : 1e-15;
      Double_t LogEVal = (DataBin > 0)? DataBin*log(MCBin/DataBin) : 0;
      LLR +=  MCBin - DataBin - LogEVal;
    }
#ifdef DEBUG_SIMPFIT
    assert(std::isfinite(LLR));
#endif
    return LLR;
  }

  Double_t operator() (Double_t const *x) const {

    SRW::ReWeightRESEvents(Events, Weights, NominalWeights, x[0], x[1], x[2]);

    PlotHistos[0]->Reset();

    # pragma omp parallel for
    for(size_t i = 0; i < Events.size(); ++i){

#ifdef _OPENMP
      PlotHistos[omp_get_thread_num()]->Fill(
#else
      PlotHistos[0]->Fill(
#endif
        Events.at(i).out[0].momentum(),
          Weights[i] * Events.at(i).weight);
    }

    for(size_t i = 1; i < NumThreads; ++i){
      PlotHistos[0]->Add(PlotHistos[i]);
      PlotHistos[i]->Reset();
    }

    Double_t LLR = CalculateTestStat();
#ifdef DEBUG_SIMPFIT
    std::cout << "[FIT]: LLR = " << LLR << " @ x[0] = " << x[0]
      << ", x[1] = " << x[1] <<", x[2] = " << x[2] << std::endl;
#endif
    return LLR;
  }

  Double_t Up() const {
    return 0.5;
  }
};

bool DoMinosErrors(ROOT::Math::Minimizer * min, int iparam){
  double minos_elow, minos_eup;
  bool minos_win;

  std::cout << "[INFO]: Running MINOS errors for param: "
    << min->VariableName(iparam) << "..." << std::endl;

  minos_win = min->GetMinosError(iparam,minos_elow,minos_eup);

  if(minos_win){
    std::cout << "[FIT RESULT]: " << min->VariableName(iparam) << " : "
      << min->X()[iparam] << " +/- " << min->Errors()[iparam] << std::endl;
    std::cout << "\t MINOS: " << minos_elow << ", +" << minos_eup << std::endl;
  } else {
    std::cout << "\t MINOS: Failed." << std::endl;
  }

  return minos_win;
}

bool FDSignalSelection(event const & ev){
  return ev.flag.cc && ev.flag.res && (!ev.flag.anty);
}

int PlotFakeData(char const * FDEventFileName, TH1D &FDDistrib){
  std::vector<event> DataEvs;
  if(!SRW::LoadSignalNuWroEventsIntoVector(FDEventFileName, DataEvs,
    FDSignalSelection)){
    return 1;
  }

  size_t nEntries = DataEvs.size();
  for(size_t ent = 0; ent < nEntries; ++ent){
    event &nwev = DataEvs[ent];
    FDDistrib.Fill(nwev.out[0].momentum(), nwev.weight);
  }
  std::cout << std::endl << "[INFO]: Data plotted!" << std::endl;
  return 0;
}

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << " <Fake Data parameters file> <Fake Data Nuwro eventsout File> "
    "<Nominal parameters file> <MC Nuwro eventsout File> <Output PDF File>"
    << std::endl;
}

int main(int argc, char const *argv[]){
  TH1::SetDefaultSumw2(false);
  // NumThreads = 1;
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

  TH1D *FDDistrib = new TH1D("FakeDataDistribution","FakeDataDistribution",
    15,0,3000);
  FDDistrib->SetDirectory(NULL);
  if(PlotFakeData(argv[2],(*FDDistrib))){
    PrintUsage(argv[0]);
    return 8;
  }

  std::cout << "[INFO]: Attempting the fittening. " << std::endl;
  NPLLR_nwev Eval(FDDistrib, argv[4]);

  Double_t FitStart[3];
  FitStart[0] = NomParams.pion_C5A;
  FitStart[1] = NomParams.pion_axial_mass;
  FitStart[2] = NomParams.SPPBkgScale;

  Eval(FitStart);
  TH1D* PreFit = static_cast<TH1D*>(Eval.PlotHistos[0]->Clone());

  ROOT::Math::Minimizer * min =
    ROOT::Math::Factory::CreateMinimizer("Minuit2","");
  ROOT::Math::Functor f(Eval,3);
  min->SetFunction(f);
  min->SetMaxIterations(-1);
  min->SetTolerance(1E-40);
  min->SetPrintLevel(0);
  min->SetLimitedVariable(0,"CA5",FitStart[0],0.25,0,5);
  min->SetLimitedVariable(1,"MaRES",FitStart[1],0.25,0,5);
  min->SetLimitedVariable(2,"SPPBkgScale",FitStart[2],0.5,0,5);

#ifdef PROFILE
  Double_t wt1 = get_wall_time();
#endif

  min->Minimize();

#ifdef PROFILE
  Double_t wt2 = get_wall_time();
  std::cout << "[PROFILE]: Minimisation took "
    << (wt2-wt1) << " seconds." << std::endl;
#endif

#ifdef PROFILE
  wt1 = get_wall_time();
#endif
  DoMinosErrors(min,0);
  std::cout << "[TRUE]: pion_C5A: " << TargetParams.pion_C5A << std::endl;
  DoMinosErrors(min,1);
  std::cout << "[TRUE]: pion_axial_mass: " << TargetParams.pion_axial_mass
    << std::endl;
  DoMinosErrors(min,2);
  std::cout << "[TRUE]: SPPBkgScale: " << TargetParams.SPPBkgScale
    << std::endl;
#ifdef PROFILE
  wt2 = get_wall_time();
  std::cout << "[PROFILE]: MINOS errors took "
    << (wt2-wt1) << " seconds." << std::endl;
#endif

  Double_t BestFit[3];
  BestFit[0] = min->X()[0];
  BestFit[1] = min->X()[1];
  BestFit[2] = min->X()[2];
  Double_t TestStat_BF = Eval(BestFit);

  TH1D* MCBestFitH = static_cast<TH1D*>(Eval.PlotHistos[0]->Clone());
  Double_t ActualBF[3];
  ActualBF[0] = TargetParams.pion_C5A;
  ActualBF[1] = TargetParams.pion_axial_mass;
  ActualBF[2] = TargetParams.SPPBkgScale;
  Double_t TestStat_ABF = Eval(ActualBF);

  std::cout << "BF: " << BestFit << ", Actual: " << ActualBF << std::endl;

  TH1D* MCTrueReWeightH = static_cast<TH1D*>(Eval.PlotHistos[0]->Clone());

  Vali::PlotValiFitResults(FDDistrib, PreFit, MCBestFitH, MCTrueReWeightH,
    "#it{p}_{#mu} (MeV/#it{c})", "#frac{d#sigma}{d#it{p}_{#mu}} (x10^{-40})",
    std::string("C_{A}^{5} True: ") + num2str(TargetParams.pion_C5A)
    + ", BF: " + num2str(min->X()[0])
    + " || M_{A}^{RES} True: " + num2str(TargetParams.pion_axial_mass)
    + ", BF: " + num2str(min->X()[1])
    + " || SPPBkgScale True: " + num2str(TargetParams.SPPBkgScale)
    + ", BF: " + num2str(min->X()[2]),
    argv[5]);

}
