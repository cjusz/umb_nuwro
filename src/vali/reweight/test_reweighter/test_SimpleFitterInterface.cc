#include <cassert>
#include <cmath>

#include <iostream>

#include "TCanvas.h"
#include "TH1D.h"
#include "TLegend.h"

#include "event1.h"

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
    << " <Input NuWro eventsout files>" << std::endl;
}

bool IsSignal(PODSimpleAnalysisFormat const &ev){
  return !((ev.NuWroDyn != 2) || (!ev.NuWroCC) || (!ev.NuWroAnty));
}

int main(int argc, char const * argv[]){
  if(argc != 2){
    std::cerr << "[ERROR]: Found " << (argc-1) << " cli args. Expected 1."
      << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }
  std::vector<event> SignalEvents;
  std::vector<PODSimpleAnalysisFormat> SignalSAFs;
  SRW::LoadSignalEventsIntoVector(argv[1],SignalEvents,SignalSAFs,&IsSignal);

  SRW::SetupSPP(SignalEvents.front().par);

  std::vector<Double_t> NominalWeights;
  NominalWeights.resize(SignalEvents.size());
  SRW::GenerateNominalRESWeights(SignalEvents,NominalWeights);
  for(size_t i = 0; i < SignalEvents.size(); ++i){
    assert(std::isfinite(NominalWeights[i]));
    assert(NominalWeights[i]>0);
  }
  std::cout << "[INFO]: Successfully loaded event vector and generated "
    "NominalWeights." << std::endl;
  std::vector<Double_t> EvReWeights;
  EvReWeights.resize(SignalEvents.size());
  SRW::ReWeightRESEvents(SignalEvents, 1.0, 1.0, EvReWeights, NominalWeights);

  PlotTest(SignalSAFs,NominalWeights,EvReWeights);

  return 0;
}
