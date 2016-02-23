#!/bin/sh
if [ "${1}" == "clean" ]; then
  rm -f *.root *.par *.cc *.txt Plot* random_seed
  exit 0
fi

if ! hash nuwro 2>/dev/null; then
  echo "[ERROR]: This script requires nuwro to be built and accesible from the \$PATH variable."
  exit 1
fi

if ! hash nuwro2rootracker 2>/dev/null; then
  echo "[ERROR]: This script requires nuwro2rootracker to be built and accesible from the \$PATH variable."
  exit 1
fi

if ! hash test_reweighter 2>/dev/null; then
  echo "[ERROR]: This script requires test_reweighter to be built and accesible from the \$PATH variable."
  exit 1
fi

if ! hash DumpEvInfo 2>/dev/null; then
  echo "[ERROR]: This script requires DumpEvInfo to be built and accesible from the \$PATH variable."
  exit 1
fi

if [ ! "${NUWRO_ROOT}" ] || [ ! -e "${NUWRO_ROOT}" ]; then
  echo "[ERROR]: \$NUWRO_ROOT must point to the root of a NuWro build."
  exit 1
fi

PARAM_BASE=$(readlink -f ${NUWRO_ROOT}/data/params.txt)
FLUX_HIST=$(readlink -f ${NUWRO_ROOT}/data/beam/t2k.txt)
TARGET_PARAMS=$(readlink -f ${NUWRO_ROOT}/data/target/C.txt)

if [ ! -e "${PARAM_BASE}" ]; then
  echo "[ERROR]: Expected to find PARAM_BASE at \"${PARAM_BASE} = ${NUWRO_ROOT}/data/params.txt\"."
  exit 1
fi

if [ ! -e "${FLUX_HIST}" ]; then
  echo "[ERROR]: Expected to find FLUX_HIST at \"${FLUX_HIST} = ${NUWRO_ROOT}/data/beam/t2k.txt\"."
  exit 1
fi

if [ ! -e "${TARGET_PARAMS}" ]; then
  echo "[ERROR]: Expected to find TARGET_PARAMS at \"${TARGET_PARAMS} = ${NUWRO_ROOT}/data/target/C.txt\"."
  exit 1
fi

DIALNAME="kNuwro_MaCCQE"
DIALPRETTYNAME="M_{A}^{QE}"
NEVS="50000"

echo "[INFO]: Using ${PARAM_BASE}, specialised with ${FLUX_HIST}, and ${TARGET_PARAMS}, generating ${NEVS} events to test dial: ${DIALNAME}"

cat ${PARAM_BASE} | sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" | sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=1200/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_qel_nc =1/dyn_qel_nc=0/g" > dial_0_params.txt
cat ${PARAM_BASE} | sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" | sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=1400/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_qel_nc =1/dyn_qel_nc=0/g" > dial_2_params.txt
cat ${PARAM_BASE} | sed "s/number_of_events   = 100000/number_of_events = ${NEVS}/g" | sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=1000/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_qel_nc =1/dyn_qel_nc=0/g" > dial_m2_params.txt

#dial m2 -- generated
if [ ! -e dial_m2_eventsout.root ]; then
  echo "[INFO]: Generating events for dial value m2"
  nuwro -i dial_m2_params.txt -o dial_m2_eventsout.root &> /dev/null
fi
if [ ! -e nutrac_dial_m2_eventsout.root ]; then
  nuwro2rootracker -x dial_m2_eventsout.root nutrac_dial_m2_eventsout.root
fi
if [ ! -e saf_dial_m2_eventsout.root ]; then
  DumpEvInfo nutrac_dial_m2_eventsout.root saf_dial_m2_eventsout.root
fi
echo "[INFO]: Done generation for dial value m2"

#dial 0
if [ ! -e dial_0_eventsout.root ]; then
  echo "[INFO]: Generating events for dial value 0"
  nuwro -i dial_0_params.txt -o dial_0_eventsout.root &> /dev/null
fi
if [ ! -e nutrac_dial_0_eventsout.root ]; then
  nuwro2rootracker -x dial_0_eventsout.root nutrac_dial_0_eventsout.root
fi
if [ ! -e saf_dial_0_eventsout.root ]; then
  DumpEvInfo nutrac_dial_0_eventsout.root saf_dial_0_eventsout.root
fi
echo "[INFO]: Done generation for dial value 0"

#dial 2 -- generated
if [ ! -e dial_2_eventsout.root ]; then
  echo "[INFO]: Generating events for dial value 2"
  nuwro -i dial_2_params.txt -o dial_2_eventsout.root &> /dev/null
fi
if [ ! -e nutrac_dial_2_eventsout.root ]; then
  nuwro2rootracker -x dial_2_eventsout.root nutrac_dial_2_eventsout.root
fi
if [ ! -e saf_dial_2_eventsout.root ]; then
  DumpEvInfo nutrac_dial_2_eventsout.root saf_dial_2_eventsout.root
fi
echo "[INFO]: Done generation for dial value 2"

echo "[INFO]: Generating weight file"
if [ ! -e ${DIALNAME}_weightsfile.root ]; then
  if ! test_reweighter ${DIALNAME} dial_0_params.txt nutrac_dial_0_eventsout.root ${DIALNAME}_weightsfile.root; then
    echo "[ERROR]: Failed to generate weights file."
    exit 1
  fi
fi

#################################################
#  Add Plots
#################################################

echo "[INFO]: Building: Plot_${DIALNAME}_Vars.cc"

cp ${NUWRO_ROOT}/src/vali/reweight/PlotDialVars.cc.in Plot_${DIALNAME}_Vars.cc

#pmu
echo "VarToPlot FS_mu_p; FS_mu_p.XAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\"; FS_mu_p.VariableName = \"pmu\"; FS_mu_p.DialName = \"${DIALNAME}\"; FS_mu_p.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_mu_p.DialUnits = \"GeV\"; FS_mu_p.PlotString = \"HMFSLepton_4Mom.Vect().Mag()\"; FS_mu_p.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==1)\"; FS_mu_p.DialValm2 = \"1\"; FS_mu_p.DialVal0 = \"1.2\"; FS_mu_p.DialVal2 = \"1.4\"; FS_mu_p.NBins = 50; FS_mu_p.XBinMin = 0; FS_mu_p.XBinMax = 2.5; FS_mu_p.LegX1 = 0.36; FS_mu_p.LegY1 = 0.5; FS_mu_p.LegX2 = 0.89; FS_mu_p.LegY2 = 0.89; FS_mu_p.LogY = false; PlotVar(FS_mu_p);" >> Plot_${DIALNAME}_Vars.cc
#thetamu
echo "VarToPlot FS_mu_ct; FS_mu_ct.XAxisTitle = \"cos#it{#theta}^{#mu}\"; FS_mu_ct.VariableName = \"ctmu\"; FS_mu_ct.DialName = \"${DIALNAME}\"; FS_mu_ct.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_mu_ct.DialUnits = \"GeV\"; FS_mu_ct.PlotString = \"HMFSLepton_4Mom.CosTheta()\"; FS_mu_ct.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==1)\"; FS_mu_ct.DialValm2 = \"1\"; FS_mu_ct.DialVal0 = \"1.2\"; FS_mu_ct.DialVal2 = \"1.4\"; FS_mu_ct.NBins = 50; FS_mu_ct.XBinMin = -1; FS_mu_ct.XBinMax = 1; FS_mu_ct.LegX1 = 0.36; FS_mu_ct.LegY1 = 0.5; FS_mu_ct.LegX2 = 0.89; FS_mu_ct.LegY2 = 0.89; FS_mu_ct.LogY = true; PlotVar(FS_mu_ct);" >> Plot_${DIALNAME}_Vars.cc

#pq2
echo "VarToPlot FS_q2; FS_q2.XAxisTitle = \"#it{Q}^{2} (GeV^2/#it{c}^{2})\"; FS_q2.VariableName = \"q2\"; FS_q2.DialName = \"${DIALNAME}\"; FS_q2.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_q2.DialUnits = \"GeV\"; FS_q2.PlotString = \"FourMomentum_Transfer.Mag2()\"; FS_q2.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==1)\"; FS_q2.DialValm2 = \"1\"; FS_q2.DialVal0 = \"1.2\"; FS_q2.DialVal2 = \"1.4\"; FS_q2.NBins = 50; FS_q2.XBinMin = -4; FS_q2.XBinMax = 0; FS_q2.LegX1 = 0.36; FS_q2.LegY1 = 0.5; FS_q2.LegX2 = 0.89; FS_q2.LegY2 = 0.89; FS_q2.LogY = false; PlotVar(FS_q2);" >> Plot_${DIALNAME}_Vars.cc

#qhadrmasscut2
echo "VarToPlot hadrmass; hadrmass.XAxisTitle = \"#it{W} (GeV/#it{c}^2)\"; hadrmass.VariableName = \"hadrmass\"; hadrmass.DialName = \"${DIALNAME}\"; hadrmass.DialPrettyName = \"${DIALPRETTYNAME}\"; hadrmass.DialUnits = \"GeV\"; hadrmass.PlotString = \"FS_HadrMass\"; hadrmass.SelectionString = \"(HMFSLepton_PDG==13)&&(NeutConventionReactionCode==1)&&(FS_HadrMass<2.0)\"; hadrmass.DialValm2 = \"1\"; hadrmass.DialVal0 = \"1.2\"; hadrmass.DialVal2 = \"1.4\"; hadrmass.NBins = 50; hadrmass.XBinMin = 1; hadrmass.XBinMax = 2; hadrmass.LegX1 = 0.36; hadrmass.LegY1 = 0.5; hadrmass.LegX2 = 0.89; hadrmass.LegY2 = 0.89; hadrmass.LogY = false; PlotVar(hadrmass);" >> Plot_${DIALNAME}_Vars.cc


#phmpi
echo "VarToPlot FS_hmpi_p; FS_hmpi_p.XAxisTitle = \"#it{p}^{#pi,Highest} (GeV/#it{c})\"; FS_hmpi_p.VariableName = \"phmpi\"; FS_hmpi_p.DialName = \"${DIALNAME}\"; FS_hmpi_p.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_hmpi_p.DialUnits = \"GeV\"; FS_hmpi_p.PlotString = \"HMPion_4Mom.Vect().Mag()\"; FS_hmpi_p.SelectionString = \"(HMPion_PDG!=0)&&(NeutConventionReactionCode==1)\"; FS_hmpi_p.DialValm2 = \"1\"; FS_hmpi_p.DialVal0 = \"1.2\"; FS_hmpi_p.DialVal2 = \"1.4\"; FS_hmpi_p.NBins = 50; FS_hmpi_p.XBinMin = 0; FS_hmpi_p.XBinMax = 2.5; FS_hmpi_p.LegX1 = 0.36; FS_hmpi_p.LegY1 = 0.5; FS_hmpi_p.LegX2 = 0.89; FS_hmpi_p.LegY2 = 0.89; FS_hmpi_p.LogY = false; PlotVar(FS_hmpi_p);" >> Plot_${DIALNAME}_Vars.cc
#thetahmpi
echo "VarToPlot FS_hmpi_ct; FS_hmpi_ct.XAxisTitle = \"cos#it{#theta}^{#pi,Highest}\"; FS_hmpi_ct.VariableName = \"cthmpi\"; FS_hmpi_ct.DialName = \"${DIALNAME}\"; FS_hmpi_ct.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_hmpi_ct.DialUnits = \"GeV\"; FS_hmpi_ct.PlotString = \"HMPion_4Mom.CosTheta()\"; FS_hmpi_ct.SelectionString = \"(HMPion_PDG!=0)&&(NeutConventionReactionCode==1)\"; FS_hmpi_ct.DialValm2 = \"1\"; FS_hmpi_ct.DialVal0 = \"1.2\"; FS_hmpi_ct.DialVal2 = \"1.4\"; FS_hmpi_ct.NBins = 50; FS_hmpi_ct.XBinMin = -1; FS_hmpi_ct.XBinMax = 1; FS_hmpi_ct.LegX1 = 0.36; FS_hmpi_ct.LegY1 = 0.5; FS_hmpi_ct.LegX2 = 0.89; FS_hmpi_ct.LegY2 = 0.89; FS_hmpi_ct.LogY = true; PlotVar(FS_hmpi_ct);" >> Plot_${DIALNAME}_Vars.cc

#phmp
echo "VarToPlot FS_hmp_p; FS_hmp_p.XAxisTitle = \"#it{p}^{p,Highest} (GeV/#it{c})\"; FS_hmp_p.VariableName = \"phmp\"; FS_hmp_p.DialName = \"${DIALNAME}\"; FS_hmp_p.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_hmp_p.DialUnits = \"GeV\"; FS_hmp_p.PlotString = \"HMProton_4Mom.Vect().Mag()\"; FS_hmp_p.SelectionString = \"(HMProton_PDG!=0)&&(NeutConventionReactionCode==1)\"; FS_hmp_p.DialValm2 = \"1\"; FS_hmp_p.DialVal0 = \"1.2\"; FS_hmp_p.DialVal2 = \"1.4\"; FS_hmp_p.NBins = 50; FS_hmp_p.XBinMin = 0; FS_hmp_p.XBinMax = 2.5; FS_hmp_p.LegX1 = 0.36; FS_hmp_p.LegY1 = 0.5; FS_hmp_p.LegX2 = 0.89; FS_hmp_p.LegY2 = 0.89; FS_hmp_p.LogY = false; PlotVar(FS_hmp_p);" >> Plot_${DIALNAME}_Vars.cc
#thetahmp
echo "VarToPlot FS_hmp_ct; FS_hmp_ct.XAxisTitle = \"cos#it{#theta}^{p,Highest}\"; FS_hmp_ct.VariableName = \"cthmp\"; FS_hmp_ct.DialName = \"${DIALNAME}\"; FS_hmp_ct.DialPrettyName = \"${DIALPRETTYNAME}\"; FS_hmp_ct.DialUnits = \"GeV\"; FS_hmp_ct.PlotString = \"HMProton_4Mom.CosTheta()\"; FS_hmp_ct.SelectionString = \"(HMProton_PDG!=0)&&(NeutConventionReactionCode==1)\"; FS_hmp_ct.DialValm2 = \"1\"; FS_hmp_ct.DialVal0 = \"1.2\"; FS_hmp_ct.DialVal2 = \"1.4\"; FS_hmp_ct.NBins = 50; FS_hmp_ct.XBinMin = -1; FS_hmp_ct.XBinMax = 1; FS_hmp_ct.LegX1 = 0.36; FS_hmp_ct.LegY1 = 0.5; FS_hmp_ct.LegX2 = 0.89; FS_hmp_ct.LegY2 = 0.89; FS_hmp_ct.LogY = true; PlotVar(FS_hmp_ct);" >> Plot_${DIALNAME}_Vars.cc

##############
# Example Plot
##############
# VarToPlot vname;
# vname.XAxisTitle = \"#it{p}^{#mu} (GeV/#it{c})\";
# vname.VariableName = \"pmu\";
# vname.DialName = \"${DIALNAME}\";
# vname.DialPrettyName = \"${DIALPRETTYNAME}\";
# vname.DialUnits = \"GeV\";
# vname.PlotString = \"HMFSLepton_4Mom.Vect().Mag()\";
# vname.SelectionString = \"(HMFSLepton_PDG==14)&&(NeutConventionReactionCode==1)\";
# vname.DialValm2 = \"1.4\";
# vname.DialVal0 = \"1.2\";
# vname.DialVal2 = \"1\";
# vname.NBins = 100;
# vname.XBinMin = 0;
# vname.XBinMax = 4;
# vname.LegX1 = 0.36;
# vname.LegY1 = 0.5;
# vname.LegX2 = 0.89;
# vname.LegY2 = 0.89;
# vname.LogY = false;

echo "}" >> Plot_${DIALNAME}_Vars.cc

if ! g++ -g -O0 $(root-config --glibs --cflags --libs) Plot_${DIALNAME}_Vars.cc -o Plot_Vars; then
  echo "[ERROR] Build failed."
  exit 1
fi

./Plot_Vars saf_dial_m2_eventsout.root saf_dial_0_eventsout.root saf_dial_2_eventsout.root ${DIALNAME}_weightsfile.root ${DIALNAME}_valid.pdf
