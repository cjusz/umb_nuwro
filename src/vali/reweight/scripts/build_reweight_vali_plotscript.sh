#!/bin/sh

if [ ! -e ${NUWRO}/build/Linux/lib/libvali.a ] && [ ! -e ${NUWRO}/build/Linux/lib/libvali.so ]; then
  echo "[ERROR]: Trying to compile reweight validation plotter: ${1}, but ${NUWRO}/build/Linux/lib/libvali.{a,so} does not exist."
  exit 1
fi

COMPILECMD="g++ -g -O0 $(root-config --cflags) ${1} -o Plot_Vars -I${NUWRO}/src/vali -L${NUWRO}/build/Linux/lib -lvali -levent  $(root-config --glibs --ldflags) "

echo "[INFO]: ${COMPILECMD}"

${COMPILECMD} 2>&1 | tee build.log
if [ "${PIPESTATUS[0]}" != "0" ]; then
  echo "[ERROR]: Build failed."
  exit 1
fi
