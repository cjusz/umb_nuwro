#!/bin/sh

DIALNAMES=${@}
DIALSTRNAME=$(echo "${@}" | tr " " "_")

echo "[INFO]: Generating reweighting validation data for dial(s): ${DIALNAMES}"

#dial m2 -- generated
if [ ! -e dial_m2_eventsout.root ]; then
  echo "[INFO]: Generating events for dial value m2"
  nuwro -i dial_m2_params.txt -o dial_m2_eventsout.root &> /dev/null
fi
if [ ! -e nutrac_dial_m2_eventsout.root ]; then
  nuwro2rootracker -x dial_m2_eventsout.root nutrac_dial_m2_eventsout.root
fi
if [ ! -e saf_dial_m2_eventsout.root ]; then
  SAF_Analysis nutrac_dial_m2_eventsout.root saf_dial_m2_eventsout.root
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
  SAF_Analysis nutrac_dial_0_eventsout.root saf_dial_0_eventsout.root
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
  SAF_Analysis nutrac_dial_2_eventsout.root saf_dial_2_eventsout.root
fi
echo "[INFO]: Done generation for dial value 2"

echo "[INFO]: Generating weight file"
if [ ! -e ${DIALSTRNAME}_weightsfile.root ]; then
  if ! test_reweighter dial_0_params.txt nutrac_dial_0_eventsout.root ${DIALSTRNAME}_weightsfile.root ${DIALNAMES}; then
    echo "[ERROR]: Failed to generate weights file."
    exit 1
  fi
fi
