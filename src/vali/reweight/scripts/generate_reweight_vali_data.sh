#!/bin/sh

DIALNAMES=${@}
DIALSTRNAME=$(echo "${@}" | tr " " "_")

echo "[INFO]: Generating reweighting validation data for dial(s): ${DIALNAMES}"

#dial m2 -- generated, only need SAF
if [ ! -e saf_dial_m2_eventsout.root ]; then
  if [ ! -e dial_m2_eventsout.root ]; then
    echo "[INFO]: Generating events for dial value m2"
    if ! nuwro -i dial_m2_params.txt -o dial_m2_eventsout.root &> dial_m2.runlog; then
      exit 1
    fi
  fi
  SAF_Analysis -n dial_m2_eventsout.root saf_dial_m2_eventsout.root
  rm dial_m2_eventsout.root dial_m2.runlog
fi
echo "[INFO]: Done generation for dial value m2"

#dial 0
if [ ! -e dial_0_eventsout.root ]; then
  echo "[INFO]: Generating events for dial value 0"
  if ! nuwro -i dial_0_params.txt -o dial_0_eventsout.root &> dial_0.runlog; then
    exit 1
  fi
  rm dial_0.runlog
fi

if [ ! -e saf_dial_0_eventsout.root ]; then
  SAF_Analysis -n dial_0_eventsout.root saf_dial_0_eventsout.root
fi
echo "[INFO]: Done generation for dial value 0"

#dial 2 -- generated, only need SAF
if [ ! -e saf_dial_2_eventsout.root ]; then
  if [ ! -e dial_2_eventsout.root ]; then
    echo "[INFO]: Generating events for dial value 2"
    if ! nuwro -i dial_2_params.txt -o dial_2_eventsout.root &> dial_2.runlog; then
      exit 1
    fi
  fi
  SAF_Analysis -n dial_2_eventsout.root saf_dial_2_eventsout.root
  rm dial_2_eventsout.root dial_2.runlog
fi
echo "[INFO]: Done generation for dial value 2"

echo "[INFO]: Generating weight file"
if [ ! -e ${DIALSTRNAME}_weightsfile.root ]; then
  if ! SimpleReweighter dial_0_eventsout.root ${DIALSTRNAME}_weightsfile.root ${DIALNAMES}; then
    echo "[ERROR]: Failed to generate weights file."
    exit 1
  fi
fi
