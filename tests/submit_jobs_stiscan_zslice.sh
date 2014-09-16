#!/bin/bash
#
# Requires at least one argument:
#
# submit_jobs_stiscan_zslice.sh JOB_TEMPLATE_XML
#
# JOB_TEMPLATE_XML is the full path to a job template xml file
#
# Usage example:
#
# submit_jobs_stiscan_zslice.sh ../path/to/job_template.xml
#

JOB_TEMPLATE_XML=$1

OUT_DIR=$OFFLINE_HFT_RESULTS_DIR

echo
echo STAR_VERSION    = $STAR_VERSION
echo OFFLINE_HFT_DIR = $OFFLINE_HFT_DIR
echo OUT_DIR         = $OUT_DIR
echo LOG_DIR         = $OUT_DIR
echo

for STISCAN_Z in {-30..30}
do
   JOB_RUN_FILE_NAME=$(printf "stiscan_zslice_%04d" $STISCAN_Z)
   echo
   echo "Submitting job for JOB_RUN_FILE_NAME: " $JOB_RUN_FILE_NAME
   echo "Submitting job for STISCAN_Z: " $STISCAN_Z
   star-submit-template -template $JOB_TEMPLATE_XML \
      -entities OUT_DIR=$OUT_DIR,CODE_DIR=$OFFLINE_HFT_DIR,STISCAN_Z=$STISCAN_Z,JOB_RUN_FILE_NAME=$JOB_RUN_FILE_NAME,STAR_VERSION=$STAR_VERSION,STAR_HOST_SYS=$STAR_HOST_SYS,LOG_DIR=$OUT_DIR
   echo
done
