#!/bin/sh

if [ ! -e ${NUWRO_ROOT}/build/Linux/lib/libvali.a ] && [ ! -e ${NUWRO_ROOT}/build/Linux/lib/libvali.so ]; then
  echo "[ERROR]: Trying to compile reweight validation plotter: ${1}, but ${NUWRO_ROOT}/build/Linux/lib/libvali.{a,so} does not exist."
  exit 1
fi

g++ -g -O0 $(root-config --glibs --cflags --libs) ${1} -o Plot_Vars -I${NUWRO_ROOT}/src/vali -L${NUWRO_ROOT}/build/Linux/lib -lvali 2>&1 | tee build.log
if [ "${PIPESTATUS[0]}" != "0" ]; then
  echo "[ERROR]: Build failed."
  exit 1
fi
