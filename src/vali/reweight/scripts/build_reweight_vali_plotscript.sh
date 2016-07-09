#!/bin/sh

if [ ! -e ${NUWRO_ROOT}/build/Linux/lib/libvali.a ] && [ ! -e ${NUWRO_ROOT}/build/Linux/lib/libvali.so ]; then
  echo "[ERROR]: Trying to compile reweight validation plotter: ${1}, but ${NUWRO_ROOT}/build/Linux/lib/libvali.{a,so} does not exist."
  exit 1
fi

COMPILECMD="g++ -g -O0 $(root-config --cflags) ${1} -o Plot_Vars -I${NUWRO_ROOT}/src/vali -L${NUWRO_ROOT}/build/Linux/lib -lvali -levent  $(root-config --glibs --evelibs --ldflags) -lEGPythia6 -L${PYTHIA6} -lPythia6"

echo "[INFO]: ${COMPILECMD}"

${COMPILECMD} 2>&1 | tee build.log
if [ "${PIPESTATUS[0]}" != "0" ]; then
  echo "[ERROR]: Build failed."
  exit 1
fi
