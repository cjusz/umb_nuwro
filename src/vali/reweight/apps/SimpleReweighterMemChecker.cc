#include <cassert>
#include <cmath>

#include <iostream>

#include "TCanvas.h"
#include "TH1D.h"
#include "TLegend.h"

#include "event1.h"

//#define SRW_DONT_LOAD_EVENTS
#include "NuwroReWeightSimple.h"

#include "SimpleAnalysisFormat.h"
#include "ValiPlotter.h"


void PlotTest(std::vector<PODSimpleAnalysisFormat> &SignalSAFs,
              std::vector<Double_t> &NominalWeights,
              std::vector<Double_t> &EvReWeights){

  Vali::ValiStyle()->cd();
  TH1D * Nominal =
    new TH1D("PreReWeight","Nominal;#frac{d#sigma}{d#it{p}_{#mu}}"
      " (nucleon^{-1} cm^{-2} MeV^{-1});#it{p}_{#mu} (MeV/#it{c})",100,0,2.0);
  TH1D * ReWeight =
    new TH1D("ReWeight","ReWeight;#frac{d#sigma}{d#it{p}_{#mu}}"
      " (nucleon^{-1} cm^{-2} MeV^{-1});#it{p}_{#mu} (MeV/#it{c})",100,0,2.0);

  for(size_t i = 0; i < SignalSAFs.size(); ++i){
    Nominal->Fill(SignalSAFs[i].HMFSLepton_4Mom.Vect().Mag(),SignalSAFs[i].EvtWght);
    ReWeight->Fill(SignalSAFs[i].HMFSLepton_4Mom.Vect().Mag(),SignalSAFs[i].EvtWght *
      EvReWeights[i]);
  }

  ReWeight->SetLineColor(kRed);
  ReWeight->SetLineStyle(2);

  Nominal->Scale(1.0,"width");
  ReWeight->Scale(1.0,"width");

  TCanvas *c1 = new TCanvas("test","test");
  TLegend * leg = new TLegend(0.5,0.5,0.85,0.9);
  leg->SetHeader("NuWro Test Load+Reweight");
  leg->SetTextSize(leg->GetTextSize()*1.25);
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(-1);
  leg->SetBorderSize(-1);
  leg->AddEntry(Nominal, Nominal->GetTitle(), "l");
  leg->AddEntry(ReWeight, ReWeight->GetTitle(), "l");

  if(ReWeight->GetMaximum() > Nominal->GetMaximum()){
    ReWeight->Draw();
    Nominal->Draw("SAME");
  } else {
    Nominal->Draw();
    ReWeight->Draw("SAME");
  }
  leg->Draw();
  c1->SaveAs("TestLoadAndReweight.pdf");
  delete c1;
}

void PrintUsage(char const * rcmd){
  std::cout << "[USAGE]: " << rcmd
    << "[#Events To Load] <Input NuWro eventsout files>" << std::endl;
}

bool IsSignal(PODSimpleAnalysisFormat const &ev){
  return true;
}
bool IsSignalNW(event const &ev){
  return true;
}


int main(int argc, char const * argv[]){
  if ((argc < 2) || (argc > 3)) {
    std::cerr << "[ERROR]: Found " << (argc-1) << " cli args. Expected 1 or 2."
      << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }
  int argo = 1;
  size_t LoadNoMoreThan = std::numeric_limits<Long64_t>::max();
  if(argc==3){
    LoadNoMoreThan = atol(argv[1]);
    LoadNoMoreThan = LoadNoMoreThan?LoadNoMoreThan:std::numeric_limits<Long64_t>::max();
    argo++;
  }
  std::vector<event> SignalEvents;
  std::vector<PODSimpleAnalysisFormat> SignalSAFs;
  std::vector<SRW::SRWEvent> SignalSRWs;
  SRW::LoadSignalEventsIntoVector(argv[argo],SignalEvents,SignalSAFs,&IsSignal,
    LoadNoMoreThan);
  std::cout << "[INFO]: Clearing vectors... " << std::endl;

  SignalEvents.clear(); SignalSAFs.clear();
  SignalEvents.resize(0); SignalSAFs.resize(0);

//Just NuWro Events

  SRW::LoadSignalNuWroEventsIntoVector(argv[argo],SignalEvents,&IsSignalNW,
    LoadNoMoreThan);
  std::cout << "[INFO]: Clearing vectors... " << std::endl;

  SignalEvents.clear(); SignalEvents.resize(0);

//Just SAF

  SRW::LoadSignalEventsIntoSAFVector(argv[argo],SignalSAFs,&IsSignal,
    LoadNoMoreThan);
  std::cout << "[INFO]: Clearing vectors... " << std::endl;

  SignalSAFs.clear(); SignalSAFs.resize(0);


//Just SRW

  SRW::LoadSignalSRWEventsIntoVector(argv[argo],SignalSRWs,&IsSignalNW,
    LoadNoMoreThan);
  std::cout << "[INFO]: Clearing vectors... " << std::endl;

  SignalSRWs.clear(); SignalSRWs.resize(0);

  return 0;
}
