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

void PlotTest(std::vector<Double_t> &SignalLeptonMomentum,
              std::vector<SRW::SRWEvent> &SignalSRWs,
              std::vector<Double_t> &EvReWeights) {
  Vali::ValiStyle()->cd();
  TH1D *Nominal =
      new TH1D("PreReWeight",
               "Nominal;#it{p}_{#mu} (GeV/#it{c})"
               "(MeV/#it{c});#frac{d#sigma}{d#it{p}_{#mu}}(nucleon^{-1} "
               "cm^{-2} GeV^{-1})",
               100, 0, 2.0);
  TH1D *ReWeight =
      new TH1D("ReWeight",
               "ReWeight;#it{p}_{#mu} (GeV/#it{c})"
               "(MeV/#it{c});#frac{d#sigma}{d#it{p}_{#mu}}(nucleon^{-1} "
               "cm^{-2} GeV^{-1})",
               100, 0, 2.0);

  for (size_t i = 0; i < SignalSRWs.size(); ++i) {
    Nominal->Fill(SignalLeptonMomentum[i], SignalSRWs[i].NominalWeight);
    ReWeight->Fill(SignalLeptonMomentum[i],
                   SignalSRWs[i].NominalWeight * EvReWeights[i]);
  }

  ReWeight->SetLineColor(kRed);
  ReWeight->SetLineStyle(2);

  Nominal->Scale(1.0, "width");
  ReWeight->Scale(1.0, "width");

  TCanvas *c1 = new TCanvas("test", "test");
  TLegend *leg = new TLegend(0.5, 0.5, 0.85, 0.9);
  leg->SetHeader("NuWro Test Load+Reweight");
  leg->SetTextSize(leg->GetTextSize() * 1.25);
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(-1);
  leg->SetBorderSize(-1);
  leg->AddEntry(Nominal, Nominal->GetTitle(), "l");
  leg->AddEntry(ReWeight, ReWeight->GetTitle(), "l");

  if (ReWeight->GetMaximum() > Nominal->GetMaximum()) {
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

void PrintUsage(char const *rcmd) {
  std::cout << "[USAGE]: " << rcmd
            << " [#Events To Load] <Input NuWro eventsout files>" << std::endl;
}
std::vector<Double_t> SignalLeptonMomentum;
params par;
bool IsSignal(event const &ev) {
  static bool first = true;
  if (first) {
    par = ev.par;
    first = false;
  }
  if ((ev.dyn == 2) && (ev.flag.cc) && (!ev.flag.anty)) {
    SignalLeptonMomentum.push_back(ev.out[0].momentum() / 1000.0);
    return true;
  }
  return false;
}

int main(int argc, char const *argv[]) {
  if ((argc < 2) || (argc > 3)) {
    std::cerr << "[ERROR]: Found " << (argc - 1)
              << " cli args. Expected 1 or 2." << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }
  int argo = 1;
  size_t LoadNoMoreThan = std::numeric_limits<Long64_t>::max();
  if (argc == 3) {
    LoadNoMoreThan = atol(argv[1]);
    LoadNoMoreThan =
        LoadNoMoreThan ? LoadNoMoreThan : std::numeric_limits<Long64_t>::max();
    argo++;
  }

  std::vector<SRW::SRWEvent> SignalSRWs;
  SRW::LoadSignalSRWEventsIntoVector(argv[argo], SignalSRWs, &IsSignal,
                                     LoadNoMoreThan);

  SRW::SetupSPP(par);

  std::vector<Double_t> NominalWeights;
  NominalWeights.resize(SignalSRWs.size());
  SRW::GenerateNominalWeights(SignalSRWs, par, NominalWeights);
  for (size_t i = 0; i < std::min(LoadNoMoreThan, SignalSRWs.size()); ++i) {
    assert(std::isfinite(NominalWeights[i]));
    assert(NominalWeights[i] > 0);
  }
  std::cout << "[INFO]: Successfully loaded event vector and generated "
               "NominalWeights."
            << std::endl;
  std::vector<Double_t> EvReWeights;
  EvReWeights.resize(SignalSRWs.size());
  SRW::ReWeightRESEvents(SignalSRWs, par, EvReWeights, NominalWeights, 1.0,
                         1.0);

  PlotTest(SignalLeptonMomentum, SignalSRWs, EvReWeights);

  return 0;
}
