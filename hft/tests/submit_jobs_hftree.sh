#!/bin/bash
#
# Requires at least one argument:
#
# submit_jobs_hftree.sh RUN_FILE_LIST
#
# RUN_FILE_LIST is the full path to a text file with a list of *.fz files
#
# Usage example:
#
# submit_jobs_hftree.sh ../path/to/filelist_fz.txt
#

RUN_FILE_LIST=$1

BFC_OPTIONS='tpcRS y2014a AgML MakeEvent ITTF StiHftC pxlFastSim istFastSim Idst BAna l0 Tree logger Sti tpcDB TpcHitMover TpxClu bbcSim btofsim tags emcY2 EEfs evout -dstout IdTruth geantout big fzin MiniMcMk clearmem HftMatTree'

# Replace all spaces with underscores
BFC_OPTIONS=`echo ${BFC_OPTIONS[*]} | sed 's/ /_/g'`

INP_DIR=$OFFLINE_HFT_RESULTS_DIR
OUT_DIR=$OFFLINE_HFT_RESULTS_DIR

echo
echo RUN_FILE_LIST   = $RUN_FILE_LIST
echo OFFLINE_HFT_DIR = $OFFLINE_HFT_DIR
echo INP_DIR         = $INP_DIR
echo OUT_DIR         = $OUT_DIR
echo STAR_VERSION    = $STAR_VERSION
echo BFC_OPTIONS     = $BFC_OPTIONS
echo LOG_DIR         = $OUT_DIR
echo

for JOB_RUN_FILE_NAME in `cat $RUN_FILE_LIST`
do
   JOB_RUN_BASE_NAME=`basename $JOB_RUN_FILE_NAME .fz`
   echo
   echo "Submitting job for JOB_RUN_FILE_NAME =" $JOB_RUN_FILE_NAME \($JOB_RUN_BASE_NAME\)
   star-submit-template -template $OFFLINE_HFT_DIR/offline/hft/tests/job_template_hftree.xml \
      -entities INP_DIR=$INP_DIR,OUT_DIR=$OUT_DIR,CODE_DIR=$OFFLINE_HFT_DIR,JOB_RUN_FILE_NAME=$JOB_RUN_FILE_NAME,JOB_RUN_BASE_NAME=$JOB_RUN_BASE_NAME,STAR_VERSION=$STAR_VERSION,STAR_HOST_SYS=$STAR_HOST_SYS,BFC_OPTIONS=$BFC_OPTIONS,LOG_DIR=$OUT_DIR
   echo
done
