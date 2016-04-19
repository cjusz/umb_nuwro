#include <sstream>
#include <string>

#include "TTree.h"
#include "TStyle.h"
#include "TColor.h"

namespace Vali {

inline
Int_t SetUserPalette(Int_t palette = 0, Int_t nCont=255, Float_t alpha =1){
  //From: https://github.com/litchfld/root-style-files/wiki/Palettes
  Int_t start = -1;
  switch (palette) {
  case 1: {
    //Rainbow palette.
    // * Variation of common style.
    // * Tried to reduce band-effect @ yellow & turquoise, but not perfect
    // * Poor on B/W
    TColor::InitializeColors();
    const Int_t NRGBs = 6;
    Double_t stops[NRGBs] = { 0.00, .001, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 1.00, 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.80, 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.80, 0.51, 1.00, 0.12, 0.00, 0.00 };
    start = TColor::CreateGradientColorTable(NRGBs, stops,
                                           red, green, blue,
                                           nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;
  }
  case 2: {
    //Blue-White-Red palette
    // * Covariance marticies or other [-1, +1] ranges
    // * Useless on B/W
    TColor::InitializeColors();
    const Int_t NRGBs = 5;
    Double_t stops[NRGBs] = { 0.00, 0.01, 0.02, 0.10, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.25, 1.00, 1.00, 0.50 };
    Double_t green[NRGBs] = { 0.00, 0.25, 1.00, 0.25, 0.00 };
    Double_t blue[NRGBs]  = { 0.50, 1.00, 1.00, 0.25, 0.00 };
    start = TColor::CreateGradientColorTable(NRGBs, stops,
                                           red, green, blue,
                                           nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;
  }
  case 3: {
    //Pale-to-dark with color
    // * Encode also in luminosity channel - better for fine detail
    // * B/W okay
    TColor::InitializeColors();
    const Int_t NRGBs = 5;
    Double_t stops[NRGBs] = { 0.00, 0.20, 0.50, 0.80, 1.00 };
    Double_t red[NRGBs]   = { 0.78, 0.60, 0.54, 0.62, 0.55 };
    Double_t green[NRGBs] = { 0.73, 0.69, 0.58, 0.36, 0.00 };
    Double_t blue[NRGBs]  = { 0.84, 0.71, 0.38, 0.09, 0.00 };
    start = TColor::CreateGradientColorTable(NRGBs, stops,
                                           red, green, blue,
                                           nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;
  }
  case 4: {
    //Purple-red-orange
    // * Uses only redish colours, allowing line overlays in blue/green
    // * Also uses luminosity dark-to-light
    // * B/W okay
    TColor::InitializeColors();
    const Int_t NRGBs = 5;
    Double_t stops[NRGBs] = { 0.00, 0.30, 0.60, 0.80, 1.00 };
    Double_t red[NRGBs]   = { 0.16, 0.32, 0.70, 1.00, 1.00 };
    Double_t green[NRGBs] = { 0.00, 0.00, 0.01, 0.48, 0.90 };
    Double_t blue[NRGBs]  = { 0.16, 0.32, 0.00, 0.00, 0.65 };
    start = TColor::CreateGradientColorTable(NRGBs, stops,
                                           red, green, blue,
                                           nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;
  }
  case 5: {
    //Cubehelix (http://www.mrao.cam.ac.uk/~dag/CUBEHELIX/)
    // * Primarily Intensity, but with colour variation.
    // * in ROOT 6.04
    // * B/W very good
    // * Not so good for colourblind at peak.
    TColor::InitializeColors();
    const Int_t NRGBs = 9;
    Double_t stops[NRGBs] = { 0.000, 0.125, 0.250, 0.375, 0.500,
                              0.625, 0.750, 0.875, 1.000};
    Double_t red[NRGBs]   = { 0.000, 0.096, 0.010, 0.212, 0.691,
                              0.924, 0.791, 0.760, 1.0000};
    Double_t green[NRGBs] = { 0.000, 0.115, 0.362, 0.504, 0.458,
                              0.469, 0.691, 0.924, 1.000};
    Double_t blue[NRGBs]  = { 0.000, 0.267, 0.312, 0.132, 0.224,
                              0.674, 0.988, 0.959, 1.0000};
    start = TColor::CreateGradientColorTable(NRGBs, stops,
                                             red, green, blue,
                                             nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;
  }
  case 6: {
    //Intended to look similar for all types of colourbindness & normal vision.
    // * (I am not colourblind myself...)
    // * -Equally bad- is still a kind of equal.
    // * B/W is tolerable to OK
    TColor::InitializeColors();
    const Int_t NRGBs = 5;
    Double_t stops[NRGBs] = { 0.00, 0.20, 0.50, 0.80, 1.00 };
    Double_t red[NRGBs]   = { 0.015, 0.047, 0.55, 0.95, 1.00};
    Double_t green[NRGBs] = { 0.010, 0.031, 0.53, 0.61, 0.85};
    Double_t blue[NRGBs]  = { 0.151, 0.455, 0.00, 0.50, 0.70};
    start = TColor::CreateGradientColorTable(NRGBs, stops,
                                           red, green, blue,
                                           nCont, alpha);
    gStyle->SetNumberContours(nCont);
    break;
  }
  default:
    std::cout << "Unrecognised User Palette" << std::endl;
    return -1;
    break;

  } //switch
  std::cout << "Palette range is [" << start << ", "
            << start + nCont << ")" << std::endl;
  return start;
}

struct VarToPlot {
  std::string XAxisTitle;
  std::string XVariableName;
  std::string XVariablePrettyName;
  std::string DialName;
  std::string DialPrettyName;
  std::string DialUnits;
  std::string PlotString;
  std::string SelectionString;
  std::string DialValm2;
  std::string DialVal0;
  std::string DialVal2;
  int NXBins;
  double XBinMin;
  double XBinMax;
  bool LogHist;
  VarToPlot();
  void Reset(); // Gimmie c++11 so I can chain call constructors.
};

struct VarToPlot1D : VarToPlot  {
  std::string LegendTitle;
  double LegTextScale;
  double LegX1;
  double LegY1;
  double LegX2;
  double LegY2;
  VarToPlot1D();
};

struct VarToPlot2D : VarToPlot {
  int NYBins;
  double YBinMin;
  double YBinMax;
  std::string YAxisTitle;
  std::string YVariableName;
  std::string YVariablePrettyName;
  VarToPlot2D();
};

struct VarToPlot_2Dial {
  std::string XAxisTitle;
  std::string XVariableName;
  std::string XVariablePrettyName;
  std::string Dial1Name;
  std::string Dial1PrettyName;
  std::string Dial1Units;
  std::string Dial2Name;
  std::string Dial2PrettyName;
  std::string Dial2Units;
  std::string PlotString;
  std::string SelectionString;
  std::string Dial1Valm2;
  std::string Dial1Val0;
  std::string Dial1Val2;
  std::string Dial2Valm2;
  std::string Dial2Val0;
  std::string Dial2Val2;
  int NXBins;
  double XBinMin;
  double XBinMax;
  bool LogHist;
  VarToPlot_2Dial();
  void Reset(); // Gimmie c++11 so I can chain call constructors.
};

struct VarToPlot1D_2Dial : VarToPlot_2Dial  {
  std::string LegendTitle;
  double LegTextScale;
  double LegX1;
  double LegY1;
  double LegX2;
  double LegY2;
  VarToPlot1D_2Dial();
};

struct VarToPlot2D_2Dial : VarToPlot_2Dial  {
  // std::string LegendTitle;
  // double LegTextScale;
  // double LegX1;
  // double LegY1;
  // double LegX2;
  // double LegY2;
  // VarToPlot1D_2Dial();
};


struct PlotData {

  std::string PDFOutName;
  std::string nent_dialm2;
  std::string nent_dial0;
  std::string nent_dial2;

  TTree * tree_dialm2;
  TTree * tree_dial0;
  TTree * tree_dial2;

  PlotData();
};

TStyle* ValiStyle();
void PlotVar1D(VarToPlot1D &v, PlotData &pd);
void PlotVar2D(VarToPlot2D &v, PlotData &pd);

void PlotVar1D_2Dial(VarToPlot1D_2Dial &v, PlotData &pd);

extern Long64_t _maxToPlot;
inline void SetMaxToPlot(Long64_t PlotNoMoreThan){
  _maxToPlot = PlotNoMoreThan;
}

void PlotValiFitResults(TH1D* FDDistrib, TH1D* MCPreFit, TH1D* MCPostFit,
  TH1D* MCTrueReweight, std::string XAxisName, std::string YAxisName,
  std::string LegendTitle, std::string PDFOutName);

}
