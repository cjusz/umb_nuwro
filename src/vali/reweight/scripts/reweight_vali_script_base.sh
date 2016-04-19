#!/bin/sh
if [ "${1}" == "clean" ]; then
  rm -f *.root *.par *.cc *.txt Plot* random_seed build.log
  exit 0
fi

if ! hash nuwro 2>/dev/null; then
  echo "[ERROR]: This script requires nuwro to be built and accessible from the \$PATH variable."
  exit 1
fi

if ! hash DumpDialTweaks 2>/dev/null; then
  echo "[ERROR]: This script requires DumpDialTweaks to be built and accessible from the \$PATH variable."
  exit 1
fi

if ! hash dumpParams 2>/dev/null; then
  echo "[ERROR]: This script requires dumpParams to be built and accessible from the \$PATH variable."
  exit 1
fi

if ! hash SimpleReweighter 2>/dev/null; then
  echo "[ERROR]: This script requires SimpleReweighter to be built and accessible from the \$PATH variable."
  exit 1
fi

if ! hash SAF_Analysis 2>/dev/null; then
  echo "[ERROR]: This script requires SAF_Analysis to be built and accessible from the \$PATH variable."
  exit 1
fi

if [ ! "${NUWRO}" ] || [ ! -e "${NUWRO}" ]; then
  echo "[ERROR]: \$NUWRO must point to the root of a NuWro build."
  exit 1
fi

if [ ! "${FLUX_HIST_NAME}" ]; then
  FLUX_HIST_NAME="t2k.txt"
fi

PARAM_BASE=$(readlink -f ${NUWRO}/data/params.txt)
FLUX_HIST=$(readlink -f ${NUWRO}/data/beam/${FLUX_HIST_NAME})
TARGET_PARAMS=$(readlink -f ${NUWRO}/data/target/C.txt)

if [ ! -e "${PARAM_BASE}" ]; then
  echo "[ERROR]: Expected to find PARAM_BASE at \"${PARAM_BASE} = ${NUWRO}/data/params.txt\"."
  exit 1
fi

if [ ! -e "${FLUX_HIST}" ]; then
  echo "[ERROR]: Expected to find FLUX_HIST at \"${FLUX_HIST} = ${NUWRO}/data/beam/t2k.txt\"."
  exit 1
fi

if [ ! -e "${TARGET_PARAMS}" ]; then
  echo "[ERROR]: Expected to find TARGET_PARAMS at \"${TARGET_PARAMS} = ${NUWRO}/data/target/C.txt\"."
  exit 1
fi
