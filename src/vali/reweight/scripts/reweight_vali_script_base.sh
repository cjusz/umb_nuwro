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

if ! hash SAF_Analysis 2>/dev/null; then
  echo "[ERROR]: This script requires SAF_Analysis to be built and accesible from the \$PATH variable."
  exit 1
fi

if [ ! "${NUWRO_ROOT}" ] || [ ! -e "${NUWRO_ROOT}" ]; then
  echo "[ERROR]: \$NUWRO_ROOT must point to the root of a NuWro build."
  exit 1
fi

if [ ! "${FLUX_HIST_NAME}" ]; then
  FLUX_HIST_NAME="t2k.txt"
fi

PARAM_BASE=$(readlink -f ${NUWRO_ROOT}/data/params.txt)
FLUX_HIST=$(readlink -f ${NUWRO_ROOT}/data/beam/${FLUX_HIST_NAME})
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
