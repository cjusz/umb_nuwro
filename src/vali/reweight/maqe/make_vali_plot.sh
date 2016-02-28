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

DIALNAME="kNuwro_MaCCQE"
DIALPRETTYNAME="M_{A}^{QE}"
DIALVALM2="1000"
DIALVAL0="1200"
DIALVAL2="1400"
DIALUNITS="MeV"
NEVS="50000"

echo "[INFO]: Using ${PARAM_BASE}, specialised with ${FLUX_HIST}, and ${TARGET_PARAMS}, generating ${NEVS} events to test dial: ${DIALNAME}"

cat ${PARAM_BASE} | sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" | sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=${DIALVAL0}/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_qel_nc =1/dyn_qel_nc=0/g" > dial_0_params.txt
cat ${PARAM_BASE} | sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" | sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=${DIALVAL2}/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_qel_nc =1/dyn_qel_nc=0/g" > dial_2_params.txt
cat ${PARAM_BASE} | sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" | sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=${DIALVALM2}/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_qel_nc =1/dyn_qel_nc=0/g" > dial_m2_params.txt

${NUWRO_ROOT}/src/vali/reweight/scripts/generate_reweight_vali_data.sh ${DIALNAME}

#################################################
#  Add Plots
#################################################

echo "[INFO]: Building: Plot_${DIALNAME}_Vars.cc"

cp ${NUWRO_ROOT}/src/vali/reweight/PlotDialVars.cc.in Plot_${DIALNAME}_Vars.cc

#pmu
echo "VarToPlot1D FS_mu_p; FS_mu_p.XAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\"; FS_mu_p.XVariableName = \"pmu\"; FS_mu_p.XVariablePrettyName = \"#it{p}^{#mu}\";  FS_mu_p.DialName = \"${DIALNAME}\"; FS_mu_p.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_mu_p.DialUnits = \"${DIALUNITS}\"; FS_mu_p.PlotString = \"HMFSLepton_4Mom.Vect().Mag()\"; FS_mu_p.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==1)\"; FS_mu_p.DialValm2 = \"${DIALVALM2}\"; FS_mu_p.DialVal0 = \"${DIALVAL0}\"; FS_mu_p.DialVal2 = \"${DIALVAL2}\"; FS_mu_p.NXBins = 50; FS_mu_p.XBinMin = 0; FS_mu_p.XBinMax = 2.5; FS_mu_p.LegX1 = 0.45; FS_mu_p.LegY1 = 0.6; FS_mu_p.LegX2 = 0.925; FS_mu_p.LegY2 = 0.925; FS_mu_p.LogHist = false; FS_mu_p.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_mu_p);" >> Plot_${DIALNAME}_Vars.cc
#thetamu
echo "VarToPlot1D FS_mu_ct; FS_mu_ct.XAxisTitle = \"cos#it{#theta}^{#mu}\"; FS_mu_ct.XVariableName = \"ctmu\"; FS_mu_ct.XVariablePrettyName = \"cos#it{#theta}^{#mu}\";  FS_mu_ct.DialName = \"${DIALNAME}\"; FS_mu_ct.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_mu_ct.DialUnits = \"${DIALUNITS}\"; FS_mu_ct.PlotString = \"HMFSLepton_4Mom.CosTheta()\"; FS_mu_ct.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==1)\"; FS_mu_ct.DialValm2 = \"${DIALVALM2}\"; FS_mu_ct.DialVal0 = \"${DIALVAL0}\"; FS_mu_ct.DialVal2 = \"${DIALVAL2}\"; FS_mu_ct.NXBins = 50; FS_mu_ct.XBinMin = -1; FS_mu_ct.XBinMax = 1; FS_mu_ct.LegX1 = 0.175; FS_mu_ct.LegY1 = 0.6; FS_mu_ct.LegX2 = 0.675; FS_mu_ct.LegY2 = 0.925; FS_mu_ct.LogHist = true; FS_mu_ct.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_mu_ct);" >> Plot_${DIALNAME}_Vars.cc

#pq2
echo "VarToPlot1D FS_q2; FS_q2.XAxisTitle = \"#it{Q}^{2} (GeV^2/#it{c}^{2})\"; FS_q2.XVariableName = \"q2\"; FS_q2.XVariablePrettyName = \"#it{Q}^{2}\";  FS_q2.DialName = \"${DIALNAME}\"; FS_q2.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_q2.DialUnits = \"${DIALUNITS}\"; FS_q2.PlotString = \"FourMomentum_Transfer.Mag2()\"; FS_q2.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==1)\"; FS_q2.DialValm2 = \"${DIALVALM2}\"; FS_q2.DialVal0 = \"${DIALVAL0}\"; FS_q2.DialVal2 = \"${DIALVAL2}\"; FS_q2.NXBins = 50; FS_q2.XBinMin = -4; FS_q2.XBinMax = 0; FS_q2.LegX1 = 0.175; FS_q2.LegY1 = 0.6; FS_q2.LegX2 = 0.675; FS_q2.LegY2 = 0.925; FS_q2.LogHist = false; FS_q2.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_q2);" >> Plot_${DIALNAME}_Vars.cc

#phmp
echo "VarToPlot1D FS_hmp_p; FS_hmp_p.XAxisTitle = \"#it{p}^{p,Highest} (GeV/#it{c})\"; FS_hmp_p.XVariableName = \"phmp\"; FS_hmp_p.XVariablePrettyName = \"#it{p}^{p,Highest}\";  FS_hmp_p.DialName = \"${DIALNAME}\"; FS_hmp_p.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_hmp_p.DialUnits = \"${DIALUNITS}\"; FS_hmp_p.PlotString = \"HMProton_4Mom.Vect().Mag()\"; FS_hmp_p.SelectionString = \"(HMProton_PDG!=0)&&(NeutConventionReactionCode==1)\"; FS_hmp_p.DialValm2 = \"${DIALVALM2}\"; FS_hmp_p.DialVal0 = \"${DIALVAL0}\"; FS_hmp_p.DialVal2 = \"${DIALVAL2}\"; FS_hmp_p.NXBins = 50; FS_hmp_p.XBinMin = 0; FS_hmp_p.XBinMax = 2.5; FS_hmp_p.LegX1 = 0.45; FS_hmp_p.LegY1 = 0.6; FS_hmp_p.LegX2 = 0.925; FS_hmp_p.LegY2 = 0.925; FS_hmp_p.LogHist = false; FS_hmp_p.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_hmp_p);" >> Plot_${DIALNAME}_Vars.cc
#thetahmp
echo "VarToPlot1D FS_hmp_ct; FS_hmp_ct.XAxisTitle = \"cos#it{#theta}^{p,Highest}\"; FS_hmp_ct.XVariableName = \"cthmp\"; FS_hmp_ct.XVariablePrettyName = \"cos#it{#theta}^{p,Highest}\";  FS_hmp_ct.DialName = \"${DIALNAME}\"; FS_hmp_ct.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_hmp_ct.DialUnits = \"${DIALUNITS}\"; FS_hmp_ct.PlotString = \"HMProton_4Mom.CosTheta()\"; FS_hmp_ct.SelectionString = \"(HMProton_PDG!=0)&&(NeutConventionReactionCode==1)\"; FS_hmp_ct.DialValm2 = \"${DIALVALM2}\"; FS_hmp_ct.DialVal0 = \"${DIALVAL0}\"; FS_hmp_ct.DialVal2 = \"${DIALVAL2}\"; FS_hmp_ct.NXBins = 50; FS_hmp_ct.XBinMin = -1; FS_hmp_ct.XBinMax = 1; FS_hmp_ct.LegX1 = 0.175; FS_hmp_ct.LegY1 = 0.6; FS_hmp_ct.LegX2 = 0.675; FS_hmp_ct.LegY2 = 0.925; FS_hmp_ct.LogHist = true; FS_hmp_ct.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D(FS_hmp_ct);" >> Plot_${DIALNAME}_Vars.cc

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


#ct_p_mu
echo "VarToPlot2D FS_ct_p_mu; FS_ct_p_mu.YAxisTitle = \"cos#it{#theta}^{#mu} (GeV/#it{c})\"; FS_ct_p_mu.YVariableName = \"ct\"; FS_ct_p_mu.YVariablePrettyName = \"cos#it{#theta}^{#mu}\"; FS_ct_p_mu.XAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\"; FS_ct_p_mu.XVariableName = \"pmu\"; FS_ct_p_mu.XVariablePrettyName = \"#it{p}^{#mu}\";  FS_ct_p_mu.DialName = \"${DIALNAME}\"; FS_ct_p_mu.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_ct_p_mu.DialUnits = \"${DIALUNITS}\"; FS_ct_p_mu.PlotString = \"HMFSLepton_4Mom.CosTheta():HMFSLepton_4Mom.Vect().Mag()\"; FS_ct_p_mu.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==1)\"; FS_ct_p_mu.DialValm2 = \"${DIALVALM2}\"; FS_ct_p_mu.DialVal0 = \"${DIALVAL0}\"; FS_ct_p_mu.DialVal2 = \"${DIALVAL2}\"; FS_ct_p_mu.NXBins = 20; FS_ct_p_mu.XBinMin = 0; FS_ct_p_mu.XBinMax = 1.5; FS_ct_p_mu.NYBins = 20; FS_ct_p_mu.YBinMin = -1; FS_ct_p_mu.YBinMax = 1; FS_ct_p_mu.LogHist = false; PlotVar2D(FS_ct_p_mu);" >> Plot_${DIALNAME}_Vars.cc

#ct_p_p
echo "VarToPlot2D FS_ct_p_p; FS_ct_p_p.YAxisTitle = \"cos#it{#theta}^{p,Highest} (GeV/#it{c})\"; FS_ct_p_p.YVariableName = \"ct\"; FS_ct_p_p.YVariablePrettyName = \"cos#it{#theta}^{p,Highest}\"; FS_ct_p_p.XAxisTitle = \"#it{p}^{p,Highest} (GeV/#it{c})\"; FS_ct_p_p.XVariableName = \"pp\"; FS_ct_p_p.XVariablePrettyName = \"#it{p}^{p,Highest}\";  FS_ct_p_p.DialName = \"${DIALNAME}\"; FS_ct_p_p.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_ct_p_p.DialUnits = \"${DIALUNITS}\"; FS_ct_p_p.PlotString = \"HMProton_4Mom.CosTheta():HMProton_4Mom.Vect().Mag()\"; FS_ct_p_p.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==1)\"; FS_ct_p_p.DialValm2 = \"${DIALVALM2}\"; FS_ct_p_p.DialVal0 = \"${DIALVAL0}\"; FS_ct_p_p.DialVal2 = \"${DIALVAL2}\"; FS_ct_p_p.NXBins = 20; FS_ct_p_p.XBinMin = 0; FS_ct_p_p.XBinMax = 1.5; FS_ct_p_p.NYBins = 20; FS_ct_p_p.YBinMin = -1; FS_ct_p_p.YBinMax = 1; FS_ct_p_p.LogHist = false; PlotVar2D(FS_ct_p_p);" >> Plot_${DIALNAME}_Vars.cc


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

if ! ${NUWRO_ROOT}/src/vali/reweight/scripts/build_reweight_vali_plotscript.sh "Plot_${DIALNAME}_Vars.cc"; then exit 1; fi

./Plot_Vars saf_dial_m2_eventsout.root saf_dial_0_eventsout.root saf_dial_2_eventsout.root ${DIALNAME}_weightsfile.root ${DIALNAME}_valid.pdf
