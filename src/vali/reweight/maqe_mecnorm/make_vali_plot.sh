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

DIAL1NAME="kNuwro_Ma_CCQE"
DIAL1PRETTYNAME="M_{A}^{CCQE}"
DIAL1VALM2="880"
DIAL1VAL0="1200"
DIAL1VAL2="1520"
DIAL1UNITS="MeV"
DIAL2NAME="kNuwro_MECNorm"
DIAL2PRETTYNAME="MECNorm"
DIAL2VALM2="0.5"
DIAL2VAL0="1.0"
DIAL2VAL2="1.5"
DIAL2UNITS=""
NEVS="5000"

echo "[INFO]: Using ${PARAM_BASE}, specialised with ${FLUX_HIST}, and ${TARGET_PARAMS}, generating ${NEVS} events to test dials: ${DIAL1NAME} ${DIAL2NAME}"


cat ${PARAM_BASE} \
| sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" \
| sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=${DIAL1VAL0}/g" \
| sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" \
| sed "s:@target/C.txt:@${TARGET_PARAMS}:g" \
| sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" \
| sed -e "s/dyn_mec_cc =0/dyn_mec_cc=1/g" \
| sed -e "s/dyn_qel_nc =1/dyn_qel_nc=0/g" > dial_0_params.txt
cat ${PARAM_BASE} \
| sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" \
| sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=${DIAL1VAL2}/g" \
| sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" \
| sed "s:@target/C.txt:@${TARGET_PARAMS}:g" \
| sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" \
| sed -e "s/dyn_mec_cc =0/dyn_mec_cc=1/g" \
| sed -e "s/dyn_res_nc =1/dyn_res_nc=0/g" > dial_2_params.txt
cat ${PARAM_BASE} \
| sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" \
| sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=${DIAL1VALM2}/g" \
| sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" \
| sed "s:@target/C.txt:@${TARGET_PARAMS}:g" \
| sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" \
| sed -e "s/dyn_mec_cc =0/dyn_mec_cc=1/g" \
| sed -e "s/dyn_res_nc =1/dyn_res_nc=0/g" > dial_m2_params.txt


${NUWRO_ROOT}/src/vali/reweight/scripts/generate_reweight_vali_data.sh ${DIAL1NAME} ${DIAL2NAME}

#################################################
#  Add Plots
#################################################

echo "[INFO]: Building: Plot_${DIAL1NAME}_${DIAL2NAME}_Vars.cc"

cp ${NUWRO_ROOT}/src/vali/reweight/PlotDialVars.cc.in Plot_${DIAL1NAME}_${DIAL2NAME}_Vars.cc

#pmu
echo "VarToPlot1D_2Dial FS_mu_p; FS_mu_p.XAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\"; FS_mu_p.XVariableName = \"pmu\"; FS_mu_p.XVariablePrettyName = \"#it{p}^{#mu}\";  FS_mu_p.Dial1Name = \"${DIAL1NAME}\"; FS_mu_p.Dial1PrettyName = \"${DIAL1PRETTYNAME}\"; FS_mu_p.Dial1Units = \"${DIAL1UNITS}\"; FS_mu_p.Dial2Name = \"${DIAL2NAME}\"; FS_mu_p.Dial2PrettyName = \"${DIAL2PRETTYNAME}\"; FS_mu_p.Dial2Units = \"${DIAL2UNITS}\"; FS_mu_p.PlotString = \"HMFSLepton_4Mom.Vect().Mag()\"; FS_mu_p.SelectionString = \"(HMFSLepton_PDG==13)&&((NeutConventionReactionCode==1)||(NeutConventionReactionCode==2))\"; FS_mu_p.Dial1Valm2 = \"${DIAL1VALM2}\"; FS_mu_p.Dial1Val0 = \"${DIAL1VAL0}\"; FS_mu_p.Dial1Val2 = \"${DIAL1VAL2}\"; FS_mu_p.Dial2Valm2 = \"${DIAL2VALM2}\"; FS_mu_p.Dial2Val0 = \"${DIAL2VAL0}\"; FS_mu_p.Dial2Val2 = \"${DIAL2VAL2}\"; FS_mu_p.NXBins = 50; FS_mu_p.XBinMin = 0; FS_mu_p.XBinMax = 2.5; FS_mu_p.LegX1 = 0.45; FS_mu_p.LegY1 = 0.6; FS_mu_p.LegX2 = 0.925; FS_mu_p.LegY2 = 0.925; FS_mu_p.LogHist = false; FS_mu_p.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D_2Dial(FS_mu_p);" >> Plot_${DIAL1NAME}_${DIAL2NAME}_Vars.cc
#thetamu
echo "VarToPlot1D_2Dial FS_mu_ct; FS_mu_ct.XAxisTitle = \"cos#it{#theta}^{#mu}\"; FS_mu_ct.XVariableName = \"ctmu\"; FS_mu_ct.XVariablePrettyName = \"cos#it{#theta}^{#mu}\";  FS_mu_ct.Dial1Name = \"${DIAL1NAME}\"; FS_mu_ct.Dial1PrettyName = \"${DIAL1PRETTYNAME}\"; FS_mu_ct.Dial1Units = \"${DIAL1UNITS}\"; FS_mu_ct.Dial2Name = \"${DIAL2NAME}\"; FS_mu_ct.Dial2PrettyName = \"${DIAL2PRETTYNAME}\"; FS_mu_ct.Dial2Units = \"${DIAL2UNITS}\"; FS_mu_ct.PlotString = \"HMFSLepton_4Mom.CosTheta()\"; FS_mu_ct.SelectionString = \"(HMFSLepton_PDG==13)&&((NeutConventionReactionCode==1)||(NeutConventionReactionCode==2))\"; FS_mu_ct.Dial1Valm2 = \"${DIAL1VALM2}\"; FS_mu_ct.Dial1Val0 = \"${DIAL1VAL0}\"; FS_mu_ct.Dial1Val2 = \"${DIAL1VAL2}\"; FS_mu_ct.Dial2Valm2 = \"${DIAL2VALM2}\"; FS_mu_ct.Dial2Val0 = \"${DIAL2VAL0}\"; FS_mu_ct.Dial2Val2 = \"${DIAL2VAL2}\"; FS_mu_ct.NXBins = 50; FS_mu_ct.XBinMin = -1; FS_mu_ct.XBinMax = 1; FS_mu_ct.LegX1 = 0.175; FS_mu_ct.LegY1 = 0.6; FS_mu_ct.LegX2 = 0.675; FS_mu_ct.LegY2 = 0.925; FS_mu_ct.LogHist = true; FS_mu_ct.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D_2Dial(FS_mu_ct);" >> Plot_${DIAL1NAME}_${DIAL2NAME}_Vars.cc

#pq2
echo "VarToPlot1D_2Dial FS_q2; FS_q2.XAxisTitle = \"#it{Q}^{2} (GeV^2/#it{c}^{2})\"; FS_q2.XVariableName = \"q2\"; FS_q2.XVariablePrettyName = \"#it{Q}^{2}\";  FS_q2.Dial1Name = \"${DIAL1NAME}\"; FS_q2.Dial1PrettyName = \"${DIAL1PRETTYNAME}\"; FS_q2.Dial1Units = \"${DIAL1UNITS}\"; FS_q2.Dial2Name = \"${DIAL2NAME}\"; FS_q2.Dial2PrettyName = \"${DIAL2PRETTYNAME}\"; FS_q2.Dial2Units = \"${DIAL2UNITS}\"; FS_q2.PlotString = \"FourMomentum_Transfer.Mag2()\"; FS_q2.SelectionString = \"(HMFSLepton_PDG==13)&&((NeutConventionReactionCode==1)||(NeutConventionReactionCode==2))\"; FS_q2.Dial1Valm2 = \"${DIAL1VALM2}\"; FS_q2.Dial1Val0 = \"${DIAL1VAL0}\"; FS_q2.Dial1Val2 = \"${DIAL1VAL2}\"; FS_q2.Dial2Valm2 = \"${DIAL2VALM2}\"; FS_q2.Dial2Val0 = \"${DIAL2VAL0}\"; FS_q2.Dial2Val2 = \"${DIAL2VAL2}\"; FS_q2.NXBins = 50; FS_q2.XBinMin = -4; FS_q2.XBinMax = 0; FS_q2.LegX1 = 0.175; FS_q2.LegY1 = 0.6; FS_q2.LegX2 = 0.675; FS_q2.LegY2 = 0.925; FS_q2.LogHist = false; FS_q2.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D_2Dial(FS_q2);" >> Plot_${DIAL1NAME}_${DIAL2NAME}_Vars.cc

#phmp
echo "VarToPlot1D_2Dial FS_hmp_p; FS_hmp_p.XAxisTitle = \"#it{p}^{p,Highest} (GeV/#it{c})\"; FS_hmp_p.XVariableName = \"phmp\"; FS_hmp_p.XVariablePrettyName = \"#it{p}^{p,Highest}\";  FS_hmp_p.Dial1Name = \"${DIAL1NAME}\"; FS_hmp_p.Dial1PrettyName = \"${DIAL1PRETTYNAME}\"; FS_hmp_p.Dial1Units = \"${DIAL1UNITS}\"; FS_hmp_p.Dial2Name = \"${DIAL2NAME}\"; FS_hmp_p.Dial2PrettyName = \"${DIAL2PRETTYNAME}\"; FS_hmp_p.Dial2Units = \"${DIAL2UNITS}\"; FS_hmp_p.PlotString = \"HMProton_4Mom.Vect().Mag()\"; FS_hmp_p.SelectionString = \"(HMProton_PDG!=0)&&((NeutConventionReactionCode==1)||(NeutConventionReactionCode==2))\"; FS_hmp_p.Dial1Valm2 = \"${DIAL1VALM2}\"; FS_hmp_p.Dial1Val0 = \"${DIAL1VAL0}\"; FS_hmp_p.Dial1Val2 = \"${DIAL1VAL2}\"; FS_hmp_p.Dial2Valm2 = \"${DIAL2VALM2}\"; FS_hmp_p.Dial2Val0 = \"${DIAL2VAL0}\"; FS_hmp_p.Dial2Val2 = \"${DIAL2VAL2}\"; FS_hmp_p.NXBins = 50; FS_hmp_p.XBinMin = 0; FS_hmp_p.XBinMax = 2.5; FS_hmp_p.LegX1 = 0.45; FS_hmp_p.LegY1 = 0.6; FS_hmp_p.LegX2 = 0.925; FS_hmp_p.LegY2 = 0.925; FS_hmp_p.LogHist = false; FS_hmp_p.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D_2Dial(FS_hmp_p);" >> Plot_${DIAL1NAME}_${DIAL2NAME}_Vars.cc
#thetahmp
echo "VarToPlot1D_2Dial FS_hmp_ct; FS_hmp_ct.XAxisTitle = \"cos#it{#theta}^{p,Highest}\"; FS_hmp_ct.XVariableName = \"cthmp\"; FS_hmp_ct.XVariablePrettyName = \"cos#it{#theta}^{p,Highest}\";  FS_hmp_ct.Dial1Name = \"${DIAL1NAME}\"; FS_hmp_ct.Dial1PrettyName = \"${DIAL1PRETTYNAME}\"; FS_hmp_ct.Dial1Units = \"${DIAL1UNITS}\"; FS_hmp_ct.Dial2Name = \"${DIAL2NAME}\"; FS_hmp_ct.Dial2PrettyName = \"${DIAL2PRETTYNAME}\"; FS_hmp_ct.Dial2Units = \"${DIAL2UNITS}\"; FS_hmp_ct.PlotString = \"HMProton_4Mom.CosTheta()\"; FS_hmp_ct.SelectionString = \"(HMProton_PDG!=0)&&((NeutConventionReactionCode==1)||(NeutConventionReactionCode==2))\"; FS_hmp_ct.Dial1Valm2 = \"${DIAL1VALM2}\"; FS_hmp_ct.Dial1Val0 = \"${DIAL1VAL0}\"; FS_hmp_ct.Dial1Val2 = \"${DIAL1VAL2}\"; FS_hmp_ct.Dial2Valm2 = \"${DIAL2VALM2}\"; FS_hmp_ct.Dial2Val0 = \"${DIAL2VAL0}\"; FS_hmp_ct.Dial2Val2 = \"${DIAL2VAL2}\"; FS_hmp_ct.NXBins = 50; FS_hmp_ct.XBinMin = -1; FS_hmp_ct.XBinMax = 1; FS_hmp_ct.LegX1 = 0.175; FS_hmp_ct.LegY1 = 0.6; FS_hmp_ct.LegX2 = 0.675; FS_hmp_ct.LegY2 = 0.925; FS_hmp_ct.LogHist = true; FS_hmp_ct.LegendTitle=\"NuWro C-target, ND280 Flux\"; PlotVar1D_2Dial(FS_hmp_ct);" >> Plot_${DIAL1NAME}_${DIAL2NAME}_Vars.cc

##############
# Example Plot 1D
##############
# VarToPlot1D_2Dial vname;
# vname.XAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\";
# vname.XVariableName = \"#it{p}^{#mu}\";
# vname.XVariablePrettyName = \"pmu\";
# vname.Dial1Name = \"${DIAL1NAME}\";
# vname.Dial1PrettyName = \"${DIAL1PRETTYNAME}\";
# vname.Dial1Units = \"${DIAL1UNITS}\";
# vname.Dial2Name = \"${DIAL2NAME}\";
# vname.Dial2PrettyName = \"${DIAL2PRETTYNAME}\";
# vname.Dial2Units = \"${DIAL2UNITS}\";
# vname.PlotString = \"HMFSLepton_4Mom.Vect().Mag()\";
# vname.SelectionString = \"(HMFSLepton_PDG==14)&&(NeutConventionReactionCode==1)\";
# vname.Dial1Valm2 = \"1.4\";
# vname.Dial1Val0 = \"1.2\";
# vname.Dial1Val2 = \"1\";
# vname.Dial2Valm2 = \"1.4\";
# vname.Dial2Val0 = \"1.2\";
# vname.Dial2Val2 = \"1\";
# vname.NXBins = 100;
# vname.XBinMin = 0;
# vname.XBinMax = 4;
# vname.LegTextScale = 0.8;
# vname.LegX1 = 0.45;
# vname.LegY1 = 0.6;
# vname.LegX2 = 0.925;
# vname.LegY2 = 0.925;
# vname.LogHist = false;
# vname.LegendTitle = \"My Legend Title\"

echo "}" >> Plot_${DIAL1NAME}_${DIAL2NAME}_Vars.cc

if ! ${NUWRO_ROOT}/src/vali/reweight/scripts/build_reweight_vali_plotscript.sh "Plot_${DIAL1NAME}_${DIAL2NAME}_Vars.cc"; then exit 1; fi

./Plot_Vars saf_dial_m2_eventsout.root saf_dial_0_eventsout.root saf_dial_2_eventsout.root ${DIAL1NAME}_${DIAL2NAME}_weightsfile.root ${DIAL1NAME}_${DIAL2NAME}_valid.pdf
