#!/bin/sh

if [ ! "${NUWRO_ROOT}" ]; then
  echo "[ERROR]: NUWRO_ROOT is not set."
  exit 1
fi

if [ ! -e ${NUWRO_ROOT}/src/vali/reweight/scripts/reweight_vali_script_base.sh ]; then
  echo "[ERROR]: Expected ${NUWRO_ROOT}/src/vali/reweight/scripts/reweight_vali_script_base.sh to exist. Cannot continue."
  exit 1
fi

if [ ! -e ${NUWRO_ROOT}/src/vali/reweight/scripts/generate_reweight_vali_data.sh ]; then
  echo "[ERROR]: Expected ${NUWRO_ROOT}/src/vali/reweight/scripts/generate_reweight_vali_data.sh to exist. Cannot continue."
  exit 1
fi

if [ ! -e ${NUWRO_ROOT}/src/vali/reweight/scripts/build_reweight_vali_plotscript.sh ]; then
  echo "[ERROR]: Expected ${NUWRO_ROOT}/src/vali/reweight/scripts/build_reweight_vali_plotscript.sh to exist. Cannot continue."
  exit 1
fi

source ${NUWRO_ROOT}/src/vali/reweight/scripts/reweight_vali_script_base.sh

DIALNAME="kNuwro_CA5"
DIALPRETTYNAME="C_{A}^{5}"
DIALVALM2="0.99"
DIALVAL0="1.19"
DIALVAL2="1.39"
DIALUNITS="MeV"
NEVS="50000"

echo "[INFO]: Using ${PARAM_BASE}, specialised with ${FLUX_HIST}, and ${TARGET_PARAMS}, generating ${NEVS} events to test dial: ${DIALNAME}"

#Could do this is CLI overrides, but it is useful to have the parameters files for validation
cat ${PARAM_BASE} | sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" | sed "s/pion_C5A =.*$/pion_C5A=${DIALVAL0}/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^r].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_res_nc =1/dyn_res_nc=0/g" > dial_0_params.txt
cat ${PARAM_BASE} | sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" | sed "s/pion_C5A =.*$/pion_C5A=${DIALVAL2}/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^r].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_res_nc =1/dyn_res_nc=0/g" > dial_2_params.txt
cat ${PARAM_BASE} | sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" | sed "s/pion_C5A =.*$/pion_C5A=${DIALVALM2}/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^r].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_res_nc =1/dyn_res_nc=0/g" > dial_m2_params.txt

${NUWRO_ROOT}/src/vali/reweight/scripts/generate_reweight_vali_data.sh ${DIALNAME}

#################################################
#  Add Plots
#################################################

echo "[INFO]: Building: Plot_${DIALNAME}_Vars.cc"

cp ${NUWRO_ROOT}/src/vali/reweight/PlotDialVars.cc.in Plot_${DIALNAME}_Vars.cc

#pmu
echo "VarToPlot1D FS_mu_p; FS_mu_p.XAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\"; FS_mu_p.XVariableName = \"pmu\"; FS_mu_p.XVariablePrettyName = \"#it{p}^{#mu}\";  FS_mu_p.DialName = \"${DIALNAME}\"; FS_mu_p.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_mu_p.DialUnits = \"${DIALUNITS}\"; FS_mu_p.PlotString = \"HMFSLepton_4Mom.Vect().Mag()\"; FS_mu_p.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==11)\"; FS_mu_p.DialValm2 = \"${DIALVALM2}\"; FS_mu_p.DialVal0 = \"${DIALVAL0}\"; FS_mu_p.DialVal2 = \"${DIALVAL2}\"; FS_mu_p.NXBins = 50; FS_mu_p.XBinMin = 0; FS_mu_p.XBinMax = 2.5; FS_mu_p.LegX1 = 0.45; FS_mu_p.LegY1 = 0.6; FS_mu_p.LegX2 = 0.925; FS_mu_p.LegY2 = 0.925; FS_mu_p.LogHist = false; FS_mu_p.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_mu_p);" >> Plot_${DIALNAME}_Vars.cc
#thetamu
echo "VarToPlot1D FS_mu_ct; FS_mu_ct.XAxisTitle = \"cos#it{#theta}^{#mu}\"; FS_mu_ct.XVariableName = \"ctmu\"; FS_mu_ct.XVariablePrettyName = \"cos#it{#theta}^{#mu}\";  FS_mu_ct.DialName = \"${DIALNAME}\"; FS_mu_ct.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_mu_ct.DialUnits = \"${DIALUNITS}\"; FS_mu_ct.PlotString = \"HMFSLepton_4Mom.CosTheta()\"; FS_mu_ct.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==11)\"; FS_mu_ct.DialValm2 = \"${DIALVALM2}\"; FS_mu_ct.DialVal0 = \"${DIALVAL0}\"; FS_mu_ct.DialVal2 = \"${DIALVAL2}\"; FS_mu_ct.NXBins = 50; FS_mu_ct.XBinMin = -1; FS_mu_ct.XBinMax = 1; FS_mu_ct.LegX1 = 0.175; FS_mu_ct.LegY1 = 0.6; FS_mu_ct.LegX2 = 0.675; FS_mu_ct.LegY2 = 0.925; FS_mu_ct.LogHist = true; FS_mu_ct.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_mu_ct);" >> Plot_${DIALNAME}_Vars.cc

#pq2
echo "VarToPlot1D FS_q2; FS_q2.XAxisTitle = \"#it{Q}^{2} (GeV^2/#it{c}^{2})\"; FS_q2.XVariableName = \"q2\"; FS_q2.XVariablePrettyName = \"#it{Q}^{2}\";  FS_q2.DialName = \"${DIALNAME}\"; FS_q2.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_q2.DialUnits = \"${DIALUNITS}\"; FS_q2.PlotString = \"FourMomentum_Transfer.Mag2()\"; FS_q2.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==11)\"; FS_q2.DialValm2 = \"${DIALVALM2}\"; FS_q2.DialVal0 = \"${DIALVAL0}\"; FS_q2.DialVal2 = \"${DIALVAL2}\"; FS_q2.NXBins = 50; FS_q2.XBinMin = -4; FS_q2.XBinMax = 0; FS_q2.LegX1 = 0.175; FS_q2.LegY1 = 0.6; FS_q2.LegX2 = 0.675; FS_q2.LegY2 = 0.925; FS_q2.LogHist = false; FS_q2.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_q2);" >> Plot_${DIALNAME}_Vars.cc

#qhadrmasscut2
echo "VarToPlot1D hadrmass; hadrmass.XAxisTitle = \"#it{W} (GeV/#it{c}^2)\"; hadrmass.XVariableName = \"hadrmass\"; hadrmass.XVariablePrettyName = \"#it{W}\";  hadrmass.DialName = \"${DIALNAME}\"; hadrmass.DialPrettyName = \"${DIALPRETTYNAME}\"; hadrmass.DialUnits = \"${DIALUNITS}\"; hadrmass.PlotString = \"FS_HadrMass\"; hadrmass.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==11)&&(FS_HadrMass<2.0)\"; hadrmass.DialValm2 = \"${DIALVALM2}\"; hadrmass.DialVal0 = \"${DIALVAL0}\"; hadrmass.DialVal2 = \"${DIALVAL2}\"; hadrmass.NXBins = 50; hadrmass.XBinMin = 1; hadrmass.XBinMax = 2; hadrmass.LegX1 = 0.45; hadrmass.LegY1 = 0.6; hadrmass.LegX2 = 0.925; hadrmass.LegY2 = 0.925; hadrmass.LogHist = false; hadrmass.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(hadrmass);" >> Plot_${DIALNAME}_Vars.cc


#phmpi
echo "VarToPlot1D FS_hmpi_p; FS_hmpi_p.XAxisTitle = \"#it{p}^{#pi,Highest} (GeV/#it{c})\"; FS_hmpi_p.XVariableName = \"phmpi\"; FS_hmpi_p.XVariablePrettyName = \"#it{p}^{#pi,Highest}\";  FS_hmpi_p.DialName = \"${DIALNAME}\"; FS_hmpi_p.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_hmpi_p.DialUnits = \"${DIALUNITS}\"; FS_hmpi_p.PlotString = \"HMPion_4Mom.Vect().Mag()\"; FS_hmpi_p.SelectionString = \"(HMPion_PDG!=0)&&(NeutConventionReactionCode==11)\"; FS_hmpi_p.DialValm2 = \"${DIALVALM2}\"; FS_hmpi_p.DialVal0 = \"${DIALVAL0}\"; FS_hmpi_p.DialVal2 = \"${DIALVAL2}\"; FS_hmpi_p.NXBins = 50; FS_hmpi_p.XBinMin = 0; FS_hmpi_p.XBinMax = 2.5; FS_hmpi_p.LegX1 = 0.45; FS_hmpi_p.LegY1 = 0.6; FS_hmpi_p.LegX2 = 0.925; FS_hmpi_p.LegY2 = 0.925; FS_hmpi_p.LogHist = false; FS_hmpi_p.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_hmpi_p);" >> Plot_${DIALNAME}_Vars.cc
#thetahmpi
echo "VarToPlot1D FS_hmpi_ct; FS_hmpi_ct.XAxisTitle = \"cos#it{#theta}^{#pi,Highest}\"; FS_hmpi_ct.XVariableName = \"cthmpi\"; FS_hmpi_ct.XVariablePrettyName = \"cos#it{#theta}^{#pi,Highest}\";  FS_hmpi_ct.DialName = \"${DIALNAME}\"; FS_hmpi_ct.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_hmpi_ct.DialUnits = \"${DIALUNITS}\"; FS_hmpi_ct.PlotString = \"HMPion_4Mom.CosTheta()\"; FS_hmpi_ct.SelectionString = \"(HMPion_PDG!=0)&&(NeutConventionReactionCode==11)\"; FS_hmpi_ct.DialValm2 = \"${DIALVALM2}\"; FS_hmpi_ct.DialVal0 = \"${DIALVAL0}\"; FS_hmpi_ct.DialVal2 = \"${DIALVAL2}\"; FS_hmpi_ct.NXBins = 50; FS_hmpi_ct.XBinMin = -1; FS_hmpi_ct.XBinMax = 1; FS_hmpi_ct.LegX1 = 0.175; FS_hmpi_ct.LegY1 = 0.6; FS_hmpi_ct.LegX2 = 0.675; FS_hmpi_ct.LegY2 = 0.925; FS_hmpi_ct.LogHist = true; FS_hmpi_ct.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_hmpi_ct);" >> Plot_${DIALNAME}_Vars.cc

#phmp
echo "VarToPlot1D FS_hmp_p; FS_hmp_p.XAxisTitle = \"#it{p}^{p,Highest} (GeV/#it{c})\"; FS_hmp_p.XVariableName = \"phmp\"; FS_hmp_p.XVariablePrettyName = \"#it{p}^{p,Highest}\";  FS_hmp_p.DialName = \"${DIALNAME}\"; FS_hmp_p.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_hmp_p.DialUnits = \"${DIALUNITS}\"; FS_hmp_p.PlotString = \"HMProton_4Mom.Vect().Mag()\"; FS_hmp_p.SelectionString = \"(HMProton_PDG!=0)&&(NeutConventionReactionCode==11)\"; FS_hmp_p.DialValm2 = \"${DIALVALM2}\"; FS_hmp_p.DialVal0 = \"${DIALVAL0}\"; FS_hmp_p.DialVal2 = \"${DIALVAL2}\"; FS_hmp_p.NXBins = 50; FS_hmp_p.XBinMin = 0; FS_hmp_p.XBinMax = 2.5; FS_hmp_p.LegX1 = 0.45; FS_hmp_p.LegY1 = 0.6; FS_hmp_p.LegX2 = 0.925; FS_hmp_p.LegY2 = 0.925; FS_hmp_p.LogHist = false; FS_hmp_p.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_hmp_p);" >> Plot_${DIALNAME}_Vars.cc
#thetahmp
echo "VarToPlot1D FS_hmp_ct; FS_hmp_ct.XAxisTitle = \"cos#it{#theta}^{p,Highest}\"; FS_hmp_ct.XVariableName = \"cthmp\"; FS_hmp_ct.XVariablePrettyName = \"cos#it{#theta}^{p,Highest}\";  FS_hmp_ct.DialName = \"${DIALNAME}\"; FS_hmp_ct.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_hmp_ct.DialUnits = \"${DIALUNITS}\"; FS_hmp_ct.PlotString = \"HMProton_4Mom.CosTheta()\"; FS_hmp_ct.SelectionString = \"(HMProton_PDG!=0)&&(NeutConventionReactionCode==11)\"; FS_hmp_ct.DialValm2 = \"${DIALVALM2}\"; FS_hmp_ct.DialVal0 = \"${DIALVAL0}\"; FS_hmp_ct.DialVal2 = \"${DIALVAL2}\"; FS_hmp_ct.NXBins = 50; FS_hmp_ct.XBinMin = -1; FS_hmp_ct.XBinMax = 1; FS_hmp_ct.LegX1 = 0.175; FS_hmp_ct.LegY1 = 0.6; FS_hmp_ct.LegX2 = 0.675; FS_hmp_ct.LegY2 = 0.925; FS_hmp_ct.LogHist = true; FS_hmp_ct.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_hmp_ct);" >> Plot_${DIALNAME}_Vars.cc

##############
# Example Plot 1D
##############
# VarToPlot1D vname;
# vname.XAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\";
# vname.XVariableName = \"#it{p}^{#mu}\";
# vname.XVariablePrettyName = \"pmu\";
# vname.DialName = \"${DIALNAME}\";
# vname.DialPrettyName = \"${DIALPRETTYNAME}\";
# vname.DialUnits = \"${DIALUNITS}\";
# vname.PlotString = \"HMFSLepton_4Mom.Vect().Mag()\";
# vname.SelectionString = \"(HMFSLepton_PDG==14)&&(NeutConventionReactionCode==1)\";
# vname.DialValm2 = \"1.4\";
# vname.DialVal0 = \"1.2\";
# vname.DialVal2 = \"1\";
# vname.NXBins = 100;
# vname.XBinMin = 0;
# vname.XBinMax = 4;
# vname.LegX1 = 0.45;
# vname.LegY1 = 0.6;
# vname.LegX2 = 0.925;
# vname.LegY2 = 0.925;
# vname.LogHist = false;
# vname.LegendTitle = \"My Legend Title\"


#q2_W
echo "VarToPlot2D FS_q2_W; FS_q2_W.YAxisTitle = \"#it{W} (GeV/#it{c}^{2})\"; FS_q2_W.YVariableName = \"hadrmass\"; FS_q2_W.YVariablePrettyName = \"#it{W}\"; FS_q2_W.XAxisTitle = \"-#it{Q}^{2} (GeV^2/#it{c}^{2})\"; FS_q2_W.XVariableName = \"q2\"; FS_q2_W.XVariablePrettyName = \"-#it{Q}^{2}\";  FS_q2_W.DialName = \"${DIALNAME}\"; FS_q2_W.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_q2_W.DialUnits = \"${DIALUNITS}\"; FS_q2_W.PlotString = \"FS_HadrMass:-FourMomentum_Transfer.Mag2()\"; FS_q2_W.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==11)\"; FS_q2_W.DialValm2 = \"${DIALVALM2}\"; FS_q2_W.DialVal0 = \"${DIALVAL0}\"; FS_q2_W.DialVal2 = \"${DIALVAL2}\"; FS_q2_W.NXBins = 20; FS_q2_W.XBinMin = 0; FS_q2_W.XBinMax = 4; FS_q2_W.NYBins = 20; FS_q2_W.YBinMin = 1; FS_q2_W.YBinMax = 2; FS_q2_W.LogHist = false; PlotVar2D(FS_q2_W);" >> Plot_${DIALNAME}_Vars.cc

#ct_p_mu
echo "VarToPlot2D FS_ct_p_mu; FS_ct_p_mu.YAxisTitle = \"cos#it{#theta}^{#mu} (GeV/#it{c})\"; FS_ct_p_mu.YVariableName = \"ct\"; FS_ct_p_mu.YVariablePrettyName = \"cos#it{#theta}^{#mu}\"; FS_ct_p_mu.XAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\"; FS_ct_p_mu.XVariableName = \"pmu\"; FS_ct_p_mu.XVariablePrettyName = \"#it{p}^{#mu}\";  FS_ct_p_mu.DialName = \"${DIALNAME}\"; FS_ct_p_mu.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_ct_p_mu.DialUnits = \"${DIALUNITS}\"; FS_ct_p_mu.PlotString = \"HMFSLepton_4Mom.CosTheta():HMFSLepton_4Mom.Vect().Mag()\"; FS_ct_p_mu.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==11)\"; FS_ct_p_mu.DialValm2 = \"${DIALVALM2}\"; FS_ct_p_mu.DialVal0 = \"${DIALVAL0}\"; FS_ct_p_mu.DialVal2 = \"${DIALVAL2}\"; FS_ct_p_mu.NXBins = 20; FS_ct_p_mu.XBinMin = 0; FS_ct_p_mu.XBinMax = 1.5; FS_ct_p_mu.NYBins = 20; FS_ct_p_mu.YBinMin = -1; FS_ct_p_mu.YBinMax = 1; FS_ct_p_mu.LogHist = false; PlotVar2D(FS_ct_p_mu);" >> Plot_${DIALNAME}_Vars.cc

#ct_p_pi
echo "VarToPlot2D FS_ct_p_pi; FS_ct_p_pi.YAxisTitle = \"cos#it{#theta}^{#pi,higest} (GeV/#it{c})\"; FS_ct_p_pi.YVariableName = \"ct\"; FS_ct_p_pi.YVariablePrettyName = \"cos#it{#theta}^{#pi,higest}\"; FS_ct_p_pi.XAxisTitle = \"#it{p}^{#pi,higest} (GeV/#it{c})\"; FS_ct_p_pi.XVariableName = \"ppi\"; FS_ct_p_pi.XVariablePrettyName = \"#it{p}^{#pi,higest}\";  FS_ct_p_pi.DialName = \"${DIALNAME}\"; FS_ct_p_pi.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_ct_p_pi.DialUnits = \"${DIALUNITS}\"; FS_ct_p_pi.PlotString = \"HMPion_4Mom.CosTheta():HMPion_4Mom.Vect().Mag()\"; FS_ct_p_pi.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==11)\"; FS_ct_p_pi.DialValm2 = \"${DIALVALM2}\"; FS_ct_p_pi.DialVal0 = \"${DIALVAL0}\"; FS_ct_p_pi.DialVal2 = \"${DIALVAL2}\"; FS_ct_p_pi.NXBins = 20; FS_ct_p_pi.XBinMin = 0; FS_ct_p_pi.XBinMax = 1.5; FS_ct_p_pi.NYBins = 20; FS_ct_p_pi.YBinMin = -1; FS_ct_p_pi.YBinMax = 1; FS_ct_p_pi.LogHist = false; PlotVar2D(FS_ct_p_pi);" >> Plot_${DIALNAME}_Vars.cc

#ct_p_p
echo "VarToPlot2D FS_ct_p_p; FS_ct_p_p.YAxisTitle = \"cos#it{#theta}^{p,Highest} (GeV/#it{c})\"; FS_ct_p_p.YVariableName = \"ct\"; FS_ct_p_p.YVariablePrettyName = \"cos#it{#theta}^{p,Highest}\"; FS_ct_p_p.XAxisTitle = \"#it{p}^{p,Highest} (GeV/#it{c})\"; FS_ct_p_p.XVariableName = \"pp\"; FS_ct_p_p.XVariablePrettyName = \"#it{p}^{p,Highest}\";  FS_ct_p_p.DialName = \"${DIALNAME}\"; FS_ct_p_p.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_ct_p_p.DialUnits = \"${DIALUNITS}\"; FS_ct_p_p.PlotString = \"HMProton_4Mom.CosTheta():HMProton_4Mom.Vect().Mag()\"; FS_ct_p_p.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==11)\"; FS_ct_p_p.DialValm2 = \"${DIALVALM2}\"; FS_ct_p_p.DialVal0 = \"${DIALVAL0}\"; FS_ct_p_p.DialVal2 = \"${DIALVAL2}\"; FS_ct_p_p.NXBins = 20; FS_ct_p_p.XBinMin = 0; FS_ct_p_p.XBinMax = 1.5; FS_ct_p_p.NYBins = 20; FS_ct_p_p.YBinMin = -1; FS_ct_p_p.YBinMax = 1; FS_ct_p_p.LogHist = false; PlotVar2D(FS_ct_p_p);" >> Plot_${DIALNAME}_Vars.cc


##############
# Example Plot 2D
##############
# VarToPlot2D vname;
# vname.XAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\";
# vname.XVariableName = \"#it{p}^{#mu}\";
# vname.XVariablePrettyName = \"pmu\";
# vname.YAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\";
# vname.YVariableName = \"#it{p}^{#mu}\";
# vname.YVariablePrettyName = \"pmu\";
# vname.DialName = \"${DIALNAME}\";
# vname.DialPrettyName = \"${DIALPRETTYNAME}\";
# vname.DialUnits = \"${DIALUNITS}\";
# vname.PlotString = \"HMFSLepton_4Mom.Vect().Mag()\";
# vname.SelectionString = \"(HMFSLepton_PDG==14)&&(NeutConventionReactionCode==1)\";
# vname.DialValm2 = \"1.4\";
# vname.DialVal0 = \"1.2\";
# vname.DialVal2 = \"1\";
# vname.NXBins = 100;
# vname.XBinMin = 0;
# vname.XBinMax = 4;
# vname.NYBins = 100;
# vname.YBinMin = 0;
# vname.YBinMax = 4;
# vname.LogHist = false;

echo "}" >> Plot_${DIALNAME}_Vars.cc

${NUWRO_ROOT}/src/vali/reweight/scripts/build_reweight_vali_plotscript.sh "Plot_${DIALNAME}_Vars.cc"

./Plot_Vars saf_dial_m2_eventsout.root saf_dial_0_eventsout.root saf_dial_2_eventsout.root ${DIALNAME}_weightsfile.root ${DIALNAME}_valid.pdf
