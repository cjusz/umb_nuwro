#!/bin/sh

if ! hash nuwro 2>/dev/null; then
  echo "This script requires nuwro to be built and accesible from the \$PATH variable."
  exit 1
fi

if ! hash nuwro 2>/dev/null; then
  echo "This script requires nuwro2rootracker to be built and accesible from the \$PATH variable."
  exit 1
fi


if [ ! -e Plot_MAQE_reweight_vali ]; then
  echo "[INFO]: Building: Plot_MAQE_reweight_vali"
  g++ $(root-config --glibs --cflags --libs) Plot_maqe_reweight_valid.cxx -o Plot_MAQE_reweight_vali
fi

PARAM_BASE=$(readlink -f ../../../data/params.txt)
FLUX_HIST=$(readlink -f ../../../data/beam/t2k.txt)
TARGET_PARAMS=$(readlink -f ../../../data/target/C.txt)

echo "[INFO]: Using ${PARAM_BASE}, specialising with ${FLUX_HIST}, and ${TARGET_PARAMS}"

mkdir -p output; cd output;

cat ${PARAM_BASE} | sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=1200/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_qel_nc =1/dyn_qel_nc=0/g" > C_nd280_maqe_1200_100kev.txt
cat ${PARAM_BASE} | sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=1400/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_qel_nc =1/dyn_qel_nc=0/g" > C_nd280_maqe_1400_100kev.txt
cat ${PARAM_BASE} | sed "s/qel_cc_axial_mass=.*$/qel_cc_axial_mass=1000/g" | sed "s:#@beam/ND280.txt:@${FLUX_HIST}:g" | sed "s:@target/C.txt:@${TARGET_PARAMS}:g" | sed -e "s/dyn_\([^q].\+_[nc]c\) =1/dyn_\1=0/g" | sed -e "s/dyn_qel_nc =1/dyn_qel_nc=0/g" > C_nd280_maqe_1000_100kev.txt

if [ ! -e C_nd280_maqe_1000_100kev_eventsout.root ]; then
  nuwro -i C_nd280_maqe_1000_100kev.txt -o C_nd280_maqe_1000_100kev_eventsout.root
fi
if [ ! -e nutrac_C_nd280_maqe_1000_100kev_eventsout.root ]; then
  nuwro2rootracker C_nd280_maqe_1000_100kev_eventsout.root nutrac_C_nd280_maqe_1000_100kev_eventsout.root
fi

if [ ! -e C_nd280_maqe_1200_100kev_eventsout.root ]; then
  nuwro -i C_nd280_maqe_1200_100kev.txt -o C_nd280_maqe_1200_100kev_eventsout.root
fi
if [ ! -e nutrac_C_nd280_maqe_1200_100kev_eventsout.root ]; then
  nuwro2rootracker C_nd280_maqe_1200_100kev_eventsout.root nutrac_C_nd280_maqe_1200_100kev_eventsout.root
fi

if [ ! -e C_nd280_maqe_1400_100kev_eventsout.root ]; then
  nuwro -i C_nd280_maqe_1400_100kev.txt -o C_nd280_maqe_1400_100kev_eventsout.root
fi
if [ ! -e nutrac_C_nd280_maqe_1400_100kev_eventsout.root ]; then
  nuwro2rootracker C_nd280_maqe_1400_100kev_eventsout.root nutrac_C_nd280_maqe_1400_100kev_eventsout.root
fi

test_reweight_CCQE_rootracker C_nd280_maqe_1200_100kev.txt nutrac_C_nd280_maqe_1200_100kev_eventsout.root weights_nom_maqe_1200.root &> /dev/null

../Plot_MAQE_reweight_vali nutrac_C_nd280_maqe_1000_100kev_eventsout.root nutrac_C_nd280_maqe_1200_100kev_eventsout.root nutrac_C_nd280_maqe_1400_100kev_eventsout.root weights_nom_maqe_1200.root

cd ../;
cp output/maqe_weights_valiplot.pdf .
