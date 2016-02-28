#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TColor.h"

#include "ValiPlotter.h"

namespace Vali {

template<typename X>
X * strTH1(std::string n, std::string t, int nb, double l, double h){
  return new X(n.c_str(),t.c_str(),nb,l,h);
}

template<typename X>
void Draw(X* rt, std::string p, std::string s, std::string o){
  rt->Draw(p.c_str(),s.c_str(),o.c_str());
}

template<typename X>
void SetTitle(X* ax, std::string s){
  ax->SetTitle(s.c_str());
}

template<typename X>
void EnlargeAx(X* ax, Float_t tfact=1.2, Float_t ofact=0xDEADB33F, Float_t lfact=0xDEADB33F){
  ofact = (ofact == 0xDEADB33F)?tfact:ofact;
  lfact = (lfact == 0xDEADB33F)?tfact:lfact;
  ax->SetTitleSize(tfact*ax->GetTitleSize());
  ax->SetTitleOffset(ofact*ax->GetTitleOffset());
  ax->SetLabelSize(lfact*ax->GetLabelSize());
}

template<typename X>
X * strTH2(std::string n, std::string t, int nbx, double lx, double hx, int nby, double ly, double hy){
  return new X(n.c_str(),t.c_str(),nbx,lx,hx,nby,ly,hy);
}

VarToPlot::VarToPlot(){ Reset(); }
void VarToPlot::Reset(){
  XAxisTitle = "";
  XVariableName = "";
  XVariablePrettyName = "";
  DialName = "";
  DialPrettyName = "";
  DialUnits = "";
  PlotString = "";
  SelectionString = "";
  DialValm2 = "";
  DialVal0 = "";
  DialVal2 = "";
  NXBins = 0;
  XBinMin = 0;
  XBinMax = 0;
  LogHist = false;
}
VarToPlot1D::VarToPlot1D() : LegendTitle(""), LegTextScale (1), LegX1(0),
  LegY1(0), LegX2(0), LegY2(0) {
  Reset();
}
VarToPlot2D::VarToPlot2D() : NYBins(0), YBinMin(0), YBinMax(0),
YAxisTitle(""), YVariableName(""), YVariablePrettyName(""){
  Reset();
}

VarToPlot_2Dial::VarToPlot_2Dial(){ Reset(); }
void VarToPlot_2Dial::Reset(){
  XAxisTitle = "";
  XVariableName = "";
  XVariablePrettyName = "";
  Dial1Name = "";
  Dial1PrettyName = "";
  Dial1Units = "";
  Dial2Name = "";
  Dial2PrettyName = "";
  Dial2Units = "";
  PlotString = "";
  SelectionString = "";
  Dial1Valm2 = "";
  Dial1Val0 = "";
  Dial1Val2 = "";
  Dial2Valm2 = "";
  Dial2Val0 = "";
  Dial2Val2 = "";
  NXBins = 0;
  XBinMin = 0;
  XBinMax = 0;
  LogHist = false;
}
VarToPlot1D_2Dial::VarToPlot1D_2Dial() : LegendTitle(""), LegTextScale (0.8),
  LegX1(0), LegY1(0), LegX2(0), LegY2(0) {
  Reset();
}

PlotData::PlotData() : PDFOutName(""), nent_dialm2(""), nent_dial0(""), nent_dial2(""),
  tree_dialm2(NULL), tree_dial0(NULL), tree_dial2(NULL){}

TStyle* ValiStyle(){
  TStyle *Style = new TStyle("st","st");

  Style->SetHatchesLineWidth(Style->GetHatchesLineWidth()*3);

  // use plain black on white colors
  Style->SetFrameBorderMode(0);
  Style->SetCanvasBorderMode(0);
  Style->SetPadBorderMode(0);
  Style->SetPadColor(0);
  Style->SetCanvasColor(0);
  Style->SetStatColor(0);
  Style->SetFillColor(0);
  Style->SetLegendBorderSize(1);

  // set the paper & margin sizes
  Style->SetPaperSize(20,26);

  // use large Times-Roman fonts
  Style->SetTextFont(132);
  Style->SetTextSize(0.15);
  Style->SetLabelFont(132,"x");
  Style->SetLabelFont(132,"y");
  Style->SetLabelFont(132,"z");
  Style->SetLabelFont(132,"t");
  Style->SetTitleFont(132,"x");
  Style->SetTitleFont(132,"y");
  Style->SetTitleFont(132,"z");
  Style->SetTitleFont(132,"t");
  Style->SetTitleFillColor(0);
  Style->SetTitleX(0.25);
  Style->SetTitleFontSize(0.1);
  Style->SetTitleFont(132,"pad");

  // use bold lines and markers
  Style->SetMarkerStyle(20);
  Style->SetHistLineWidth(1.85);
  Style->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars and y error bar caps
  Style->SetErrorX(0.001);

  // do not display any of the standard histogram decorations
  Style->SetOptTitle(0);
  Style->SetOptStat(0);
  Style->SetOptFit(0);

  // put tick marks on top and RHS of plots
  Style->SetPadTickX(1);
  Style->SetPadTickY(1);

  Style->SetPalette(1,0);  // use the nice red->blue palette
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;

  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  Style->SetNumberContours(NCont);

  return Style;
}

void PlotVar1D(VarToPlot1D &v, PlotData &pd){

  std::string h_m2_name = v.XVariableName+"_dialm2";
  std::string h_0_name = v.XVariableName+"_dial0";
  std::string h_2_name = v.XVariableName+"_dial2";
  TH1D* h_dialm2 = strTH1<TH1D>(h_m2_name, v.DialPrettyName + " = " + v.DialValm2 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax);
  TH1D* h_dial0 = strTH1<TH1D>(h_0_name, v.DialPrettyName + " = " + v.DialVal0 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax);
  TH1D* h_dial2 = strTH1<TH1D>(h_2_name, v.DialPrettyName +" = " + v.DialVal2 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax);

  std::string h_0_wm2_name = v.XVariableName+"_dial0_weightm2";
  std::string h_0_w2_name = v.XVariableName+"_dial0_weight2";
  TH1D* h_dial0_weight_m2 = strTH1<TH1D>(h_0_wm2_name, v.DialPrettyName + " = " + v.DialVal0 + " " + v.DialUnits + ", Reweighted to " + v.DialValm2 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax);
  TH1D* h_dial0_weight_2 = strTH1<TH1D>(h_0_w2_name, v.DialPrettyName + " = " + v.DialVal0 + " " + v.DialUnits + ", Reweighted to " + v.DialVal2 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax);


  Draw(pd.tree_dialm2, v.PlotString + " >> " + h_m2_name,
    std::string("(EvtWght*1E-38*(" + v.SelectionString + "))/")+pd.nent_dialm2+".0","GOFF");

  Draw(pd.tree_dial0, v.PlotString + " >> " + h_0_name,
    std::string("(EvtWght*1E-38*(" + v.SelectionString + "))/")+pd.nent_dial0+".0","GOFF");

  Draw(pd.tree_dial2, v.PlotString + " >> " + h_2_name,
    std::string("(EvtWght*1E-38*(" + v.SelectionString + "))/")+pd.nent_dial2+".0","GOFF");

  Draw(pd.tree_dial0, v.PlotString + " >> " + h_0_wm2_name,
    std::string("weight_") + v.DialName + "_0_to_minus2*(EvtWght*1E-38*(" + v.SelectionString + "))/"+pd.nent_dial0+".0","GOFF");

  Draw(pd.tree_dial0, v.PlotString + " >> " + h_0_w2_name,
    std::string("weight_") + v.DialName + "_0_to_2*(EvtWght*1E-38*(" + v.SelectionString + "))/"+pd.nent_dial0+".0","GOFF");

  h_dialm2->Scale(1.0,"width");
  h_dial0->Scale(1.0,"width");
  h_dial2->Scale(1.0,"width");
  h_dial0_weight_m2->Scale(1.0,"width");
  h_dial0_weight_2->Scale(1.0,"width");

  h_dialm2->SetMarkerStyle(0);
  h_dial0->SetMarkerStyle(0);
  h_dial2->SetMarkerStyle(0);
  h_dial0_weight_m2->SetMarkerStyle(0);
  h_dial0_weight_2->SetMarkerStyle(0);

  h_dialm2->SetLineWidth(2);
  h_dialm2->SetLineStyle(1);
  h_dialm2->SetLineColor(kBlack);

  h_dial0->SetLineWidth(2);
  h_dial0->SetLineStyle(1);
  h_dial0->SetLineColor(kBlue);

  h_dial2->SetLineWidth(2);
  h_dial2->SetLineStyle(1);
  h_dial2->SetLineColor(kRed);

  h_dial0_weight_m2->SetLineWidth(3);
  h_dial0_weight_m2->SetLineStyle(2);
  h_dial0_weight_m2->SetLineColor(kGreen-2);

  h_dial0_weight_2->SetLineWidth(3);
  h_dial0_weight_2->SetLineStyle(2);
  h_dial0_weight_2->SetLineColor(kBlack);

  TCanvas * c1 = new TCanvas(v.XVariableName.c_str(),"");
  c1->SetLogy(v.LogHist);
  c1->SetMargin(0.15,0.03,0.13,0.06);

  TLegend * leg = new TLegend(v.LegX1,v.LegY1,v.LegX2,v.LegY2);
  leg->SetTextSize(leg->GetTextSize()*v.LegTextScale);
  leg->SetHeader(v.LegendTitle.c_str());
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(-1);
  leg->SetBorderSize(-1);

  SetTitle(h_dial2->GetXaxis(),v.XAxisTitle);
  SetTitle(h_dial2->GetYaxis(),"#frac{d#sigma}{d" + v.XVariablePrettyName + "} (cm^{2} nucleon^{-1} GeV^{-1})");
  EnlargeAx(h_dial2->GetXaxis(),1.25,1.25,1.5);
  EnlargeAx(h_dial2->GetYaxis(),1.25,1.25,1.5);

  h_dial2->Draw("EHIST");
  h_dialm2->Draw("EHIST SAME");
  h_dial0->Draw("EHIST SAME");
  h_dial0_weight_m2->Draw("EHIST SAME");
  h_dial0_weight_2->Draw("EHIST SAME");

  leg->AddEntry(h_dial2, h_dial2->GetTitle(), "l");
  leg->AddEntry(h_dialm2, h_dialm2->GetTitle(), "l");
  leg->AddEntry(h_dial0, h_dial0->GetTitle(), "l");
  leg->AddEntry(h_dial0_weight_m2, h_dial0_weight_m2->GetTitle(), "l");
  leg->AddEntry(h_dial0_weight_2, h_dial0_weight_2->GetTitle(), "l");

  leg->Draw();

  c1->SaveAs(pd.PDFOutName.c_str());

}

void PlotVar1D_2Dial(VarToPlot1D_2Dial &v, PlotData &pd){

  std::string h_m2_name = v.XVariableName + "_dialsm2";
  std::string h_0_name = v.XVariableName + "_dials0";
  std::string h_2_name = v.XVariableName + "_dials2";
  TH1D* h_dialm2 = strTH1<TH1D>(h_m2_name, v.Dial1PrettyName + " = " + v.Dial1Valm2 + " " + v.Dial1Units + ", " + v.Dial2PrettyName + " = " + v.Dial2Valm2 + " " + v.Dial2Units, v.NXBins, v.XBinMin, v.XBinMax);
  TH1D* h_dial0 = strTH1<TH1D>(h_0_name, v.Dial1PrettyName + " = " + v.Dial1Val0 + " " + v.Dial1Units + ", " + v.Dial2PrettyName + " = " + v.Dial2Val0 + " " + v.Dial2Units, v.NXBins, v.XBinMin, v.XBinMax);
  TH1D* h_dial2 = strTH1<TH1D>(h_2_name, v.Dial1PrettyName + " = " + v.Dial1Val2 + " " + v.Dial1Units + ", " + v.Dial2PrettyName + " = " + v.Dial2Val2 + " " + v.Dial2Units, v.NXBins, v.XBinMin, v.XBinMax);

  std::string h_0_wm2_name = v.XVariableName + "_dials0_weightm2";
  std::string h_0_w2_name = v.XVariableName + "_dials0_weight2";
  TH1D* h_dial0_weight_m2 = strTH1<TH1D>(h_0_wm2_name, v.Dial1PrettyName + " = " + v.Dial1Val0 + " " + v.Dial1Units + " #Rightarrow " + v.Dial1Valm2 + " " + v.Dial1Units
    + ", " + v.Dial2PrettyName + " = " + v.Dial2Val0 + " " + v.Dial2Units + " #Rightarrow " + v.Dial2Valm2 + " " + v.Dial2Units, v.NXBins, v.XBinMin, v.XBinMax);
  TH1D* h_dial0_weight_2 = strTH1<TH1D>(h_0_w2_name, v.Dial1PrettyName + " = " + v.Dial1Val0 + " " + v.Dial1Units + ", #Rightarrow " + v.Dial1Val2 + " " + v.Dial1Units
    + ", " + v.Dial2PrettyName + " = " + v.Dial2Val0 + " " + v.Dial2Units + " #Rightarrow " + v.Dial2Val2 + " " + v.Dial2Units, v.NXBins, v.XBinMin, v.XBinMax);


  Draw(pd.tree_dialm2, v.PlotString + " >> " + h_m2_name,
    std::string("(EvtWght*1E-38*(" + v.SelectionString + "))/")+pd.nent_dialm2+".0","GOFF");

  Draw(pd.tree_dial0, v.PlotString + " >> " + h_0_name,
    std::string("(EvtWght*1E-38*(" + v.SelectionString + "))/")+pd.nent_dial0+".0","GOFF");

  Draw(pd.tree_dial2, v.PlotString + " >> " + h_2_name,
    std::string("(EvtWght*1E-38*(" + v.SelectionString + "))/")+pd.nent_dial2+".0","GOFF");

  Draw(pd.tree_dial0, v.PlotString + " >> " + h_0_wm2_name,
    std::string("weight_") + v.Dial1Name + "_0_to_minus2_" + v.Dial2Name + "_0_to_minus2*(EvtWght*1E-38*(" + v.SelectionString + "))/"+pd.nent_dial0+".0","GOFF");

  Draw(pd.tree_dial0, v.PlotString + " >> " + h_0_w2_name,
    std::string("weight_") + v.Dial1Name + "_0_to_2_" + v.Dial2Name + "_0_to_2*(EvtWght*1E-38*(" + v.SelectionString + "))/"+pd.nent_dial0+".0","GOFF");

  h_dialm2->Scale(1.0,"width");
  h_dial0->Scale(1.0,"width");
  h_dial2->Scale(1.0,"width");
  h_dial0_weight_m2->Scale(1.0,"width");
  h_dial0_weight_2->Scale(1.0,"width");

  h_dialm2->SetMarkerStyle(0);
  h_dial0->SetMarkerStyle(0);
  h_dial2->SetMarkerStyle(0);
  h_dial0_weight_m2->SetMarkerStyle(0);
  h_dial0_weight_2->SetMarkerStyle(0);

  h_dialm2->SetLineWidth(2);
  h_dialm2->SetLineStyle(1);
  h_dialm2->SetLineColor(kBlack);

  h_dial0->SetLineWidth(2);
  h_dial0->SetLineStyle(1);
  h_dial0->SetLineColor(kBlue);

  h_dial2->SetLineWidth(2);
  h_dial2->SetLineStyle(1);
  h_dial2->SetLineColor(kRed);

  h_dial0_weight_m2->SetLineWidth(3);
  h_dial0_weight_m2->SetLineStyle(2);
  h_dial0_weight_m2->SetLineColor(kGreen-2);

  h_dial0_weight_2->SetLineWidth(3);
  h_dial0_weight_2->SetLineStyle(2);
  h_dial0_weight_2->SetLineColor(kBlack);

  TCanvas * c1 = new TCanvas(v.XVariableName.c_str(),"");
  c1->SetLogy(v.LogHist);
  c1->SetMargin(0.15,0.03,0.13,0.06);

  TLegend * leg = new TLegend(v.LegX1,v.LegY1,v.LegX2,v.LegY2);
  leg->SetTextSize(leg->GetTextSize()*v.LegTextScale);
  leg->SetHeader(v.LegendTitle.c_str());
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(-1);
  leg->SetBorderSize(-1);

  SetTitle(h_dial2->GetXaxis(),v.XAxisTitle);
  SetTitle(h_dial2->GetYaxis(),"#frac{d#sigma}{d" + v.XVariablePrettyName + "} (cm^{2} nucleon^{-1} GeV^{-1})");
  EnlargeAx(h_dial2->GetXaxis(),1.25,1.25,1.5);
  EnlargeAx(h_dial2->GetYaxis(),1.25,1.25,1.5);

  h_dial2->Draw("EHIST");
  h_dialm2->Draw("EHIST SAME");
  h_dial0->Draw("EHIST SAME");
  h_dial0_weight_m2->Draw("EHIST SAME");
  h_dial0_weight_2->Draw("EHIST SAME");

  leg->AddEntry(h_dial2, h_dial2->GetTitle(), "l");
  leg->AddEntry(h_dialm2, h_dialm2->GetTitle(), "l");
  leg->AddEntry(h_dial0, h_dial0->GetTitle(), "l");
  leg->AddEntry(h_dial0_weight_m2, h_dial0_weight_m2->GetTitle(), "l");
  leg->AddEntry(h_dial0_weight_2, h_dial0_weight_2->GetTitle(), "l");

  leg->Draw();

  c1->SaveAs(pd.PDFOutName.c_str());
}

void PlotVar2D(VarToPlot2D &v, PlotData &pd){

  std::string h_m2_name = v.XVariableName+v.YVariableName+"_dialm2";
  std::string h_2_name = v.XVariableName+v.YVariableName+"_dial2";
  TH2D* h_dialm2 = strTH2<TH2D>(h_m2_name, v.DialPrettyName + " = " + v.DialValm2 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax, v.NYBins, v.YBinMin, v.YBinMax);
  TH2D* h_dial2 = strTH2<TH2D>(h_2_name, v.DialPrettyName +" = " + v.DialVal2 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax, v.NYBins, v.YBinMin, v.YBinMax);

  std::string h_0_wm2_name = v.XVariableName+v.YVariableName+"_dial0_weightm2";
  std::string h_0_w2_name = v.XVariableName+v.YVariableName+"_dial0_weight2";
  TH2D* h_dial0_weight_m2 = strTH2<TH2D>(h_0_wm2_name, v.DialPrettyName + " = " + v.DialVal0 + " " + v.DialUnits + ", Reweighted to " + v.DialValm2 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax, v.NYBins, v.YBinMin, v.YBinMax);
  TH2D* h_dial0_weight_2 = strTH2<TH2D>(h_0_w2_name, v.DialPrettyName + " = " + v.DialVal0 + " " + v.DialUnits + ", Reweighted to " + v.DialVal2 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax, v.NYBins, v.YBinMin, v.YBinMax);


  Draw(pd.tree_dialm2, v.PlotString + " >> " + h_m2_name,
    std::string("(EvtWght*1E-38*(" + v.SelectionString + "))/")+pd.nent_dialm2+".0","GOFF");

  Draw(pd.tree_dial2, v.PlotString + " >> " + h_2_name,
    std::string("(EvtWght*1E-38*(" + v.SelectionString + "))/")+pd.nent_dial2+".0","GOFF");

  Draw(pd.tree_dial0, v.PlotString + " >> " + h_0_wm2_name,
    std::string("weight_") + v.DialName + "_0_to_minus2*(EvtWght*1E-38*(" + v.SelectionString + "))/"+pd.nent_dial0+".0","GOFF");

  Draw(pd.tree_dial0, v.PlotString + " >> " + h_0_w2_name,
    std::string("weight_") + v.DialName + "_0_to_2*(EvtWght*1E-38*(" + v.SelectionString + "))/"+pd.nent_dial0+".0","GOFF");

  h_dialm2->Scale(1.0,"width");
  h_dial2->Scale(1.0,"width");
  h_dial0_weight_m2->Scale(1.0,"width");
  h_dial0_weight_2->Scale(1.0,"width");

  TCanvas * c1 = new TCanvas((v.XVariableName+v.YVariableName).c_str(),"");
  c1->SetLogz(v.LogHist);

  TH2D* h_dialm2_over_dial0_wm2 = strTH2<TH2D>(h_m2_name+"__"+h_0_wm2_name, v.DialPrettyName + " = " + v.DialValm2 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax, v.NYBins, v.YBinMin, v.YBinMax);
  TH2D* h_dial2_over_dial0_w2 = strTH2<TH2D>(h_2_name+"__"+h_0_w2_name, v.DialPrettyName + " = " + v.DialValm2 + " " + v.DialUnits, v.NXBins, v.XBinMin, v.XBinMax, v.NYBins, v.YBinMin, v.YBinMax);

  h_dialm2_over_dial0_wm2->Divide(h_dialm2,h_dial0_weight_m2);
  h_dial2_over_dial0_w2->Divide(h_dial2,h_dial0_weight_2);

  c1->SetMargin(0,0,0,0);
  c1->Divide(3,2,0,0);
  c1->GetPad(1)->SetMargin(0.15,0.2,0.12,0.06);
  c1->GetPad(2)->SetMargin(0.15,0.2,0.12,0.06);
  c1->GetPad(3)->SetMargin(0.15,0.225,0.12,0.06);
  c1->GetPad(4)->SetMargin(0.15,0.2,0.12,0.06);
  c1->GetPad(5)->SetMargin(0.15,0.2,0.12,0.06);
  c1->GetPad(6)->SetMargin(0.15,0.225,0.12,0.06);

  c1->GetPad(1)->cd();
  SetTitle(h_dialm2->GetXaxis(),v.XAxisTitle);
  SetTitle(h_dialm2->GetYaxis(),v.YAxisTitle);
  SetTitle(h_dialm2->GetZaxis(),"#frac{d^{2}#sigma}{d" + v.XVariablePrettyName + v.YVariablePrettyName + "} (cm^{2} nucleon^{-1} GeV^{-1})");
  EnlargeAx(h_dialm2->GetXaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dialm2->GetYaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dialm2->GetZaxis(),1.0,1.2,1.0);
  h_dialm2->Draw("COLZ");
  c1->GetPad(2)->cd();
  SetTitle(h_dial0_weight_m2->GetXaxis(),v.XAxisTitle);
  SetTitle(h_dial0_weight_m2->GetYaxis(),v.YAxisTitle);
  SetTitle(h_dial0_weight_m2->GetZaxis(),"#frac{d^{2}#sigma}{d" + v.XVariablePrettyName + v.YVariablePrettyName + "} (cm^{2} nucleon^{-1} GeV^{-1})");
  EnlargeAx(h_dial0_weight_m2->GetXaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dial0_weight_m2->GetYaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dial0_weight_m2->GetZaxis(),1.0,1.2,1.0);
  h_dial0_weight_m2->Draw("COLZ");
  c1->GetPad(3)->cd();
  SetTitle(h_dialm2_over_dial0_wm2->GetXaxis(),v.XAxisTitle);
  SetTitle(h_dialm2_over_dial0_wm2->GetYaxis(),v.YAxisTitle);
  SetTitle(h_dialm2_over_dial0_wm2->GetZaxis(),"#frac{"+v.DialPrettyName + " = " + v.DialVal2+"}{"+v.DialPrettyName + " = " + v.DialVal0 + " " + v.DialUnits + ", Reweighted to " + v.DialValm2 + " " + v.DialUnits+"}");
  EnlargeAx(h_dialm2_over_dial0_wm2->GetXaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dialm2_over_dial0_wm2->GetYaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dialm2_over_dial0_wm2->GetZaxis(),1.0,1.35,1.0);
  h_dialm2_over_dial0_wm2->Draw("COLZ");
  c1->GetPad(4)->cd();
  SetTitle(h_dial2->GetXaxis(),v.XAxisTitle);
  SetTitle(h_dial2->GetYaxis(),v.YAxisTitle);
  SetTitle(h_dial2->GetZaxis(),"#frac{d^{2}#sigma}{d" + v.XVariablePrettyName + v.YVariablePrettyName + "} (cm^{2} nucleon^{-1} GeV^{-1})");
  EnlargeAx(h_dial2->GetXaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dial2->GetYaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dial2->GetZaxis(),1.0,1.2,1.0);
  h_dial2->Draw("COLZ");
  c1->GetPad(5)->cd();
  SetTitle(h_dial0_weight_2->GetXaxis(),v.XAxisTitle);
  SetTitle(h_dial0_weight_2->GetYaxis(),v.YAxisTitle);
  SetTitle(h_dial0_weight_2->GetZaxis(),"#frac{d^{2}#sigma}{d" + v.XVariablePrettyName + v.YVariablePrettyName + "} (cm^{2} nucleon^{-1} GeV^{-1})");
  EnlargeAx(h_dial0_weight_2->GetXaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dial0_weight_2->GetYaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dial0_weight_2->GetZaxis(),1.0,1.2,1.0);
  h_dial0_weight_2->Draw("COLZ");
  c1->GetPad(6)->cd();
  SetTitle(h_dial2_over_dial0_w2->GetXaxis(),v.XAxisTitle);
  SetTitle(h_dial2_over_dial0_w2->GetYaxis(),v.YAxisTitle);
  SetTitle(h_dial2_over_dial0_w2->GetZaxis(),"#frac{"+v.DialPrettyName + " = " + v.DialVal2+"}{"+v.DialPrettyName + " = " + v.DialVal0 + " " + v.DialUnits + ", Reweighted to " + v.DialVal2 + " " + v.DialUnits+"}");
  EnlargeAx(h_dial2_over_dial0_w2->GetXaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dial2_over_dial0_w2->GetYaxis(),1.25,1.2,1.0);
  EnlargeAx(h_dial2_over_dial0_w2->GetZaxis(),1.0,1.35,1.0);
  h_dial2_over_dial0_w2->Draw("COLZ");

  c1->SaveAs(pd.PDFOutName.c_str());

}

void PlotValiFitResults(TH1D* FDDistrib, TH1D* MCPreFit, TH1D* MCPostFit,
  TH1D* MCTrueReweight, std::string XAxisName, std::string YAxisName,
  std::string LegendTitle, std::string PDFOutName){

  TCanvas* c1 = new TCanvas("ValiFitResults","");
  c1->SetMargin(0.15,0.03,0.13,0.06);
  ValiStyle()->cd();

  FDDistrib->SetLineColor(kBlack);
  FDDistrib->SetLineWidth(2);
  FDDistrib->SetLineStyle(1);
  FDDistrib->SetTitle("Fake Data");

  MCPreFit->SetLineColor(kRed);
  MCPreFit->SetLineWidth(2);
  MCPreFit->SetLineStyle(2);
  MCPreFit->SetTitle("MC pre-fit");

  MCPostFit->SetLineColor(kBlue);
  MCPostFit->SetLineWidth(2);
  MCPostFit->SetLineStyle(2);
  MCPostFit->SetTitle("MC post-fit");

  MCTrueReweight->SetLineColor(kMagenta);
  MCTrueReweight->SetLineWidth(2);
  MCTrueReweight->SetLineStyle(2);
  MCTrueReweight->SetTitle("MC reweighted with true params.");

  TLegend * leg = new TLegend(0.5,0.5,0.85,0.9);
  leg->SetHeader(LegendTitle.c_str());
  leg->SetTextSize(leg->GetTextSize()*1.25);
  leg->SetFillColor(kWhite);
  leg->SetFillStyle(-1);
  leg->SetBorderSize(-1);
  leg->AddEntry(FDDistrib, FDDistrib->GetTitle(), "l");
  leg->AddEntry(MCPreFit, MCPreFit->GetTitle(), "l");
  leg->AddEntry(MCPostFit, MCPostFit->GetTitle(), "l");
  leg->AddEntry(MCTrueReweight, MCTrueReweight->GetTitle(), "l");

  if(FDDistrib->GetMaximum() > MCPreFit->GetMaximum()){
    FDDistrib->GetYaxis()->SetRangeUser(0,FDDistrib->GetMaximum()*1.2);
    FDDistrib->GetYaxis()->SetTitle(YAxisName.c_str());
    FDDistrib->GetXaxis()->SetTitle(XAxisName.c_str());
    FDDistrib->Draw("");
    MCPreFit->Draw("SAME");
  } else {
    MCPreFit->GetYaxis()->SetRangeUser(0,MCPreFit->GetMaximum()*1.2);
    MCPreFit->GetYaxis()->SetTitle(YAxisName.c_str());
    MCPreFit->GetXaxis()->SetTitle(XAxisName.c_str());
    MCPreFit->Draw("");
    FDDistrib->Draw("SAME");
  }

  MCPostFit->Draw("SAME");
  MCTrueReweight->Draw("SAME");

  leg->Draw();

  c1->SaveAs(PDFOutName.c_str());
  delete c1;
}

}
