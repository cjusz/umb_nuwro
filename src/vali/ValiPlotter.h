#include <sstream>
#include <string>

#include "TTree.h"
#include "TStyle.h"

namespace Vali {

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

void PlotValiFitResults(TH1D* FDDistrib, TH1D* MCPreFit, TH1D* MCPostFit,
  TH1D* MCTrueReweight, std::string XAxisName, std::string YAxisName,
  std::string LegendTitle, std::string PDFOutName);

}
