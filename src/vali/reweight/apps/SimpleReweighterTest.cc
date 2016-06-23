#include <cmath>
#include <iostream>
#include <vector>

#include "TCanvas.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TLine.h"
#include "TMarker.h"
#include "TStyle.h"

// nuwro core
#include "event1.h"
#include "params.h"

// nuwro reweight
#include "NuwroReWeightSimple.h"

void PrintUsage(char const *rcmd) {
  std::cout << "[USAGE]: " << rcmd
            << " <NuWro eventsout1.root> <NuWro eventsout2.root>" << std::endl;
}

std::vector<Double_t> PlotVar;
params par;
bool first = true;
bool IsSignal(event const &ev) {
  if (first) {
    par = ev.par;
    first = false;
  }
  if (ev.dyn < 4) {
    Double_t mom = 0;
    for (std::vector<particle>::const_iterator p_it = ev.post.begin();
         p_it < ev.post.end(); ++p_it) {
      particle const &post_part = (*p_it);
      if (post_part.pdg == 13) {
        mom = post_part.momentum() / 1000.0;
        break;
      }
    }
    PlotVar.push_back(mom);
    return true;
  }
  return false;
}

int main(int argc, char const *argv[]) {
  if (argc != 3) {
    std::cerr << "[ERROR]: Found " << (argc - 1) << " cli args. Expected 2."
              << std::endl;
    PrintUsage(argv[0]);
    return 1;
  }

  Long64_t LoadNoMoreThan = 10000000;

  std::vector<SRW::SRWEvent> SignalSRWs1;
  SRW::LoadSignalSRWEventsIntoVector(argv[1], SignalSRWs1, &IsSignal,
                                     LoadNoMoreThan);
  std::vector<Double_t> Var1;
  Var1.swap(PlotVar);
  params Params1 = par;
  SRW::SimpleReWeightParams srwp1(Params1.pion_C5A, Params1.pion_axial_mass,
                                Params1.SPPBkgScale, Params1.qel_cc_axial_mass);

  first = true;
  std::vector<SRW::SRWEvent> SignalSRWs2;
  SRW::LoadSignalSRWEventsIntoVector(argv[2], SignalSRWs2, &IsSignal,
                                     LoadNoMoreThan);
  std::vector<Double_t> Var2;
  Var2.swap(PlotVar);
  params Params2 = par;
  SRW::SimpleReWeightParams srwp2(Params2.pion_C5A, Params2.pion_axial_mass,
                                Params2.SPPBkgScale, Params2.qel_cc_axial_mass);

  SRW::SetupSPP(Params1);

  std::vector<Double_t> EvReWeights;
  EvReWeights.resize(SignalSRWs1.size());
  SRW::ReWeightEvents(
      SignalSRWs1, Params1, EvReWeights, std::vector<Double_t>(),
      srwp2);

  std::cout << "Reweighting: " << srwp1 << " => " << srwp2 << std::endl;

  TH1::SetDefaultSumw2(true);
  TH1D *TestReweight1p = new TH1D("TestReweight1p", "Nominal", 20, 0, 3);
  TH1D *TestReweight1 =
      new TH1D("TestReweight1", "Nominal => Target", 20, 0, 3);
  TH1D *TestReweight2 = new TH1D("TestReweight2", "Target", 20, 0, 3);

  for (size_t i = 0; i < Var1.size(); ++i) {
    TestReweight1p->Fill(Var1[i], SignalSRWs1[i].NominalWeight * 1E38);
  }
  for (size_t i = 0; i < Var1.size(); ++i) {
    // std::cout << "rw: " << EvReWeights[i] << ", nw: " << SignalSRWs1[i].NominalWeight * 1E38 << std::endl;
    TestReweight1->Fill(Var1[i],
                        SignalSRWs1[i].NominalWeight * 1E38 * EvReWeights[i]);
  }
  for (size_t i = 0; i < Var2.size(); ++i) {
    TestReweight2->Fill(Var2[i], SignalSRWs2[i].NominalWeight * 1E38);
  }
  TCanvas *c1 = new TCanvas("TestReweight", "");
  c1->cd();
  gStyle->SetOptStat(0);
  TestReweight1p->SetLineColor(kRed);
  TestReweight1->SetLineColor(kBlue);
  TestReweight2->SetLineColor(kBlack);
  TestReweight1p->SetLineStyle(1);
  TestReweight1->SetLineStyle(2);
  TestReweight2->SetLineStyle(3);
  TestReweight1p->SetLineWidth(2);
  TestReweight1->SetLineWidth(2);
  TestReweight2->SetLineWidth(2);
  TestReweight1p->GetYaxis()->SetRangeUser(0,
                                           TestReweight1p->GetMaximum() * 1.2);
  TestReweight1p->Draw("EHIST");
  TestReweight1->Draw("EHIST SAME");
  TestReweight2->Draw("EHIST SAME");
  c1->BuildLegend(0.35, 0.6, 0.9, 0.9);
  c1->SaveAs("TestReweight.pdf");
}
