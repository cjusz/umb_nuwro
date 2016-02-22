#include <sstream>
#include <string>

#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"

inline TStyle* MakeT2KStyle(){
  TStyle *t2kStyle = new TStyle("T2K","T2K approved plots style");

  t2kStyle->SetHatchesLineWidth(t2kStyle->GetHatchesLineWidth()*3);

  // use plain black on white colors
  t2kStyle->SetFrameBorderMode(0);
  t2kStyle->SetCanvasBorderMode(0);
  t2kStyle->SetPadBorderMode(0);
  t2kStyle->SetPadColor(0);
  t2kStyle->SetCanvasColor(0);
  t2kStyle->SetStatColor(0);
  t2kStyle->SetFillColor(0);
  t2kStyle->SetLegendBorderSize(1);

  // set the paper & margin sizes
  t2kStyle->SetPaperSize(20,26);

  // use large Times-Roman fonts
  t2kStyle->SetTextFont(132);
  t2kStyle->SetTextSize(0.15);
  t2kStyle->SetLabelFont(132,"x");
  t2kStyle->SetLabelFont(132,"y");
  t2kStyle->SetLabelFont(132,"z");
  t2kStyle->SetLabelFont(132,"t");
  t2kStyle->SetTitleFont(132,"x");
  t2kStyle->SetTitleFont(132,"y");
  t2kStyle->SetTitleFont(132,"z");
  t2kStyle->SetTitleFont(132,"t");
  t2kStyle->SetTitleFillColor(0);
  t2kStyle->SetTitleX(0.25);
  t2kStyle->SetTitleFontSize(0.1);
  t2kStyle->SetTitleFont(132,"pad");

  // use bold lines and markers
  t2kStyle->SetMarkerStyle(20);
  t2kStyle->SetHistLineWidth(1.85);
  t2kStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars and y error bar caps
  t2kStyle->SetErrorX(0.001);

  // do not display any of the standard histogram decorations
  t2kStyle->SetOptTitle(0);
  t2kStyle->SetOptStat(0);
  t2kStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  t2kStyle->SetPadTickX(1);
  t2kStyle->SetPadTickY(1);

  return t2kStyle;
}

int main(int argc, char const * argv[]){

  TH1::SetDefaultSumw2();
  MakeT2KStyle()->cd();

  TFile* _file0 = TFile::Open(argv[1],"OPEN");
  TFile* _file1 = TFile::Open(argv[2],"OPEN");
  TFile* _file2 = TFile::Open(argv[3],"OPEN");
  TFile* _file3 = TFile::Open(argv[4],"OPEN");

  TTree * tree_ma10 = static_cast<TTree*>(_file0->Get("nRooTracker"));
  std::stringstream ss;
  ss << tree_ma10->GetEntries();
  std::string nent_ma10 = ss.str(); ss.str("");
  TTree * tree_ma12 = static_cast<TTree*>(_file1->Get("nRooTracker"));
  ss << tree_ma12->GetEntries();
  std::string nent_ma12 = ss.str(); ss.str("");
  TTree * tree_ma14 = static_cast<TTree*>(_file2->Get("nRooTracker"));
  ss << tree_ma14->GetEntries();
  std::string nent_ma14 = ss.str(); ss.str("");

  tree_ma12->AddFriend("nRooTracker_weights", _file3->GetName());

//Muon Momentum

  TH1D* QE_MuMom_ma10 = new TH1D("QE_MuMom_ma10","M_{a}^{QE} = 1 GeV",25,0,2);
  TH1D* QE_MuMom_ma12 = new TH1D("QE_MuMom_ma12","M_{a}^{QE} = 1.2 GeV",25,0,2);
  TH1D* QE_MuMom_ma14 = new TH1D("QE_MuMom_ma14","M_{a}^{QE} = 1.4 GeV;#it{p}^"
    "{#mu} (GeV/#it{c});#sigma (cm^{2} nucleon^{-1} GeV^{-1})",25,0,2);

  TH1D* QE_MuMom_ma12_weight_10 = new TH1D("QE_MuMom_ma12_weight_10","M_{a}^"
    "{QE} = 1.2 GeV, Reweighted to 1 GeV",25,0,2);
  TH1D* QE_MuMom_ma12_weight_14 = new TH1D("QE_MuMom_ma12_weight_14","M_{a}^"
    "{QE} = 1.2 GeV, Reweighted to 1.4 GeV",25,0,2);

  tree_ma10->Draw("sqrt(StdHepP4[2][0]*StdHepP4[2][0]+StdHepP4[2][1]*StdHepP4"
    "[2][1]+StdHepP4[2][2]*StdHepP4[2][2]) >> QE_MuMom_ma10",
    (std::string("(EvtWght*1E-38*(EvtCode.fString.Atoi()==1))/")+nent_ma10+".0").c_str(),"GOFF");

  tree_ma12->Draw("sqrt(StdHepP4[2][0]*StdHepP4[2][0]+StdHepP4[2][1]*StdHepP4"
    "[2][1]+StdHepP4[2][2]*StdHepP4[2][2]) >> QE_MuMom_ma12",
    (std::string("(EvtWght*1E-38*(EvtCode.fString.Atoi()==1))/")+nent_ma12+".0").c_str(),"GOFF");

  tree_ma14->Draw("sqrt(StdHepP4[2][0]*StdHepP4[2][0]+StdHepP4[2][1]*StdHepP4"
    "[2][1]+StdHepP4[2][2]*StdHepP4[2][2]) >> QE_MuMom_ma14",
    (std::string("(EvtWght*1E-38*(EvtCode.fString.Atoi()==1))/")+nent_ma14+".0").c_str(),"GOFF");

  tree_ma12->Draw("sqrt(StdHepP4[2][0]*StdHepP4[2][0]+StdHepP4[2][1]*StdHepP4"
    "[2][1]+StdHepP4[2][2]*StdHepP4[2][2]) >> QE_MuMom_ma12_weight_14",
    (std::string("weight_MAQE_0_to_2*(EvtWght*1E-38*(EvtCode.fString.Atoi()==1))/")+nent_ma12+".0").c_str(),"GOFF");

  tree_ma12->Draw("sqrt(StdHepP4[2][0]*StdHepP4[2][0]+StdHepP4[2][1]*StdHepP4"
    "[2][1]+StdHepP4[2][2]*StdHepP4[2][2]) >> QE_MuMom_ma12_weight_10",
    (std::string("weight_MAQE_0_to_minus2*(EvtWght*1E-38*(EvtCode.fString.Atoi()==1))/")+nent_ma12+".0").c_str(),"GOFF");

  QE_MuMom_ma10->Scale(1.0,"width");
  QE_MuMom_ma12->Scale(1.0,"width");
  QE_MuMom_ma14->Scale(1.0,"width");
  QE_MuMom_ma12_weight_10->Scale(1.0,"width");
  QE_MuMom_ma12_weight_14->Scale(1.0,"width");

  QE_MuMom_ma10->SetMarkerStyle(0);
  QE_MuMom_ma12->SetMarkerStyle(0);
  QE_MuMom_ma14->SetMarkerStyle(0);
  QE_MuMom_ma12_weight_10->SetMarkerStyle(0);
  QE_MuMom_ma12_weight_14->SetMarkerStyle(0);

  QE_MuMom_ma10->SetLineWidth(2);
  QE_MuMom_ma10->SetLineStyle(1);
  QE_MuMom_ma10->SetLineColor(kBlack);

  QE_MuMom_ma12->SetLineWidth(2);
  QE_MuMom_ma12->SetLineStyle(1);
  QE_MuMom_ma12->SetLineColor(kBlue);

  QE_MuMom_ma14->SetLineWidth(2);
  QE_MuMom_ma14->SetLineStyle(1);
  QE_MuMom_ma14->SetLineColor(kRed);

  QE_MuMom_ma12_weight_10->SetLineWidth(3);
  QE_MuMom_ma12_weight_10->SetLineStyle(2);
  QE_MuMom_ma12_weight_10->SetLineColor(kGreen-2);

  QE_MuMom_ma12_weight_14->SetLineWidth(3);
  QE_MuMom_ma12_weight_14->SetLineStyle(2);
  QE_MuMom_ma12_weight_14->SetLineColor(kBlack);

  TCanvas * c1 = new TCanvas("dummycanv","");

  TLegend * leg = new TLegend(0.36,0.5,0.89,0.89);
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(-1);
  leg->SetBorderSize(-1);

  QE_MuMom_ma14->Draw("EHIST");
  QE_MuMom_ma10->Draw("EHIST SAME");
  QE_MuMom_ma12->Draw("EHIST SAME");
  QE_MuMom_ma12_weight_10->Draw("EHIST SAME");
  QE_MuMom_ma12_weight_14->Draw("EHIST SAME");

  leg->AddEntry(QE_MuMom_ma14, QE_MuMom_ma14->GetTitle(), "l");
  leg->AddEntry(QE_MuMom_ma10, QE_MuMom_ma10->GetTitle(), "l");
  leg->AddEntry(QE_MuMom_ma12, QE_MuMom_ma12->GetTitle(), "l");
  leg->AddEntry(QE_MuMom_ma12_weight_10, QE_MuMom_ma12_weight_10->GetTitle(), "l");
  leg->AddEntry(QE_MuMom_ma12_weight_14, QE_MuMom_ma12_weight_14->GetTitle(), "l");

  leg->Draw();

  TCanvas *c3 = new TCanvas("dum","");
  c3->SaveAs("maqe_weights_valiplot.pdf[");
  c1->SaveAs("maqe_weights_valiplot.pdf");

//Muon CosTheta

  TH1D* QE_MuCTheta_ma10 = new TH1D("QE_MuCTheta_ma10","M_{a}^{QE} = 1 GeV",25,-1,1);
  TH1D* QE_MuCTheta_ma12 = new TH1D("QE_MuCTheta_ma12","M_{a}^{QE} = 1.2 GeV",25,-1,1);
  TH1D* QE_MuCTheta_ma14 = new TH1D("QE_MuCTheta_ma14","M_{a}^{QE} = 1.4 GeV;cos"
    "#theta^{#mu};#sigma (cm^{2} nucleon^{-1} GeV^{-1})",25,-1,1);

  TH1D* QE_MuCTheta_ma12_weight_10 = new TH1D("QE_MuCTheta_ma12_weight_10","M_{a}^"
    "{QE} = 1.2 GeV, Reweighted to 1 GeV",25,-1,1);
  TH1D* QE_MuCTheta_ma12_weight_14 = new TH1D("QE_MuCTheta_ma12_weight_14","M_{a}^"
    "{QE} = 1.2 GeV, Reweighted to 1.4 GeV",25,-1,1);

  tree_ma10->Draw("StdHepP4[2][2]/sqrt(StdHepP4[2][0]*StdHepP4[2][0]+StdHepP4[2][1]*StdHepP4"
    "[2][1]+StdHepP4[2][2]*StdHepP4[2][2]) >> QE_MuCTheta_ma10",
    (std::string("(EvtWght*1E-38*(EvtCode.fString.Atoi()==1))/")+nent_ma10+".0").c_str(),"GOFF");

  tree_ma12->Draw("StdHepP4[2][2]/sqrt(StdHepP4[2][0]*StdHepP4[2][0]+StdHepP4[2][1]*StdHepP4"
    "[2][1]+StdHepP4[2][2]*StdHepP4[2][2]) >> QE_MuCTheta_ma12",
    (std::string("(EvtWght*1E-38*(EvtCode.fString.Atoi()==1))/")+nent_ma12+".0").c_str(),"GOFF");

  tree_ma14->Draw("StdHepP4[2][2]/sqrt(StdHepP4[2][0]*StdHepP4[2][0]+StdHepP4[2][1]*StdHepP4"
    "[2][1]+StdHepP4[2][2]*StdHepP4[2][2]) >> QE_MuCTheta_ma14",
    (std::string("(EvtWght*1E-38*(EvtCode.fString.Atoi()==1))/")+nent_ma14+".0").c_str(),"GOFF");

  tree_ma12->Draw("StdHepP4[2][2]/sqrt(StdHepP4[2][0]*StdHepP4[2][0]+StdHepP4[2][1]*StdHepP4"
    "[2][1]+StdHepP4[2][2]*StdHepP4[2][2]) >> QE_MuCTheta_ma12_weight_14",
    (std::string("weight_MAQE_0_to_2*(EvtWght*1E-38*(EvtCode.fString.Atoi()==1))/")+nent_ma12+".0").c_str(),"GOFF");

  tree_ma12->Draw("StdHepP4[2][2]/sqrt(StdHepP4[2][0]*StdHepP4[2][0]+StdHepP4[2][1]*StdHepP4"
    "[2][1]+StdHepP4[2][2]*StdHepP4[2][2]) >> QE_MuCTheta_ma12_weight_10",
    (std::string("weight_MAQE_0_to_minus2*(EvtWght*1E-38*(EvtCode.fString.Atoi()==1))/")+nent_ma12+".0").c_str(),"GOFF");

  QE_MuCTheta_ma10->Scale(1.0,"width");
  QE_MuCTheta_ma12->Scale(1.0,"width");
  QE_MuCTheta_ma14->Scale(1.0,"width");
  QE_MuCTheta_ma12_weight_10->Scale(1.0,"width");
  QE_MuCTheta_ma12_weight_14->Scale(1.0,"width");

  QE_MuCTheta_ma10->SetMarkerStyle(0);
  QE_MuCTheta_ma12->SetMarkerStyle(0);
  QE_MuCTheta_ma14->SetMarkerStyle(0);
  QE_MuCTheta_ma12_weight_10->SetMarkerStyle(0);
  QE_MuCTheta_ma12_weight_14->SetMarkerStyle(0);

  QE_MuCTheta_ma10->SetLineWidth(2);
  QE_MuCTheta_ma10->SetLineStyle(1);
  QE_MuCTheta_ma10->SetLineColor(kBlack);

  QE_MuCTheta_ma12->SetLineWidth(2);
  QE_MuCTheta_ma12->SetLineStyle(1);
  QE_MuCTheta_ma12->SetLineColor(kBlue);

  QE_MuCTheta_ma14->SetLineWidth(2);
  QE_MuCTheta_ma14->SetLineStyle(1);
  QE_MuCTheta_ma14->SetLineColor(kRed);

  QE_MuCTheta_ma12_weight_10->SetLineWidth(3);
  QE_MuCTheta_ma12_weight_10->SetLineStyle(2);
  QE_MuCTheta_ma12_weight_10->SetLineColor(kGreen-2);

  QE_MuCTheta_ma12_weight_14->SetLineWidth(3);
  QE_MuCTheta_ma12_weight_14->SetLineStyle(2);
  QE_MuCTheta_ma12_weight_14->SetLineColor(kBlack);

  TCanvas * c2 = new TCanvas("dummycanv","");

  c2->SetLogy(true);

  TLegend * leg2 = new TLegend(0.15,0.55,0.65,0.89);
  leg2->SetFillColor(kWhite);
  leg2->SetFillStyle(-1);
  leg2->SetBorderSize(-1);

  QE_MuCTheta_ma14->Draw("EHIST");
  QE_MuCTheta_ma10->Draw("EHIST SAME");
  QE_MuCTheta_ma12->Draw("EHIST SAME");
  QE_MuCTheta_ma12_weight_10->Draw("EHIST SAME");
  QE_MuCTheta_ma12_weight_14->Draw("EHIST SAME");

  leg2->AddEntry(QE_MuCTheta_ma14, QE_MuCTheta_ma14->GetTitle(), "l");
  leg2->AddEntry(QE_MuCTheta_ma10, QE_MuCTheta_ma10->GetTitle(), "l");
  leg2->AddEntry(QE_MuCTheta_ma12, QE_MuCTheta_ma12->GetTitle(), "l");
  leg2->AddEntry(QE_MuCTheta_ma12_weight_10, QE_MuCTheta_ma12_weight_10->GetTitle(), "l");
  leg2->AddEntry(QE_MuCTheta_ma12_weight_14, QE_MuCTheta_ma12_weight_14->GetTitle(), "l");

  leg2->Draw();
  c2->SaveAs("maqe_weights_valiplot.pdf");

  _file0->Close();
  _file1->Close();
  _file2->Close();
  _file3->Close();

  c3->SaveAs("maqe_weights_valiplot.pdf]");

}
