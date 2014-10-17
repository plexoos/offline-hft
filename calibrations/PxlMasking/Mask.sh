#!/bin/bash
starver dev
echo $ROOTSYS
echo $LD_LIBRARY_PATH
cwd=$(pwd)
root4star -b -q $cwd/StRoot/StPxlMaskMaker/MaskFiles.C\($1,\"$2\",\"$cwd\",\"$3\"\)>log
mv *$2.C Tables/.
#mv *$2.C StarDb/Calibrations/pxl/.
mv ana_$2.root run_rootfiles/ana_$1.root
