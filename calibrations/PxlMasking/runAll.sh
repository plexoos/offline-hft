#!/bin/bash
date
ifile=0
rm script/*$2*
rm log/$2*
    
    cp run.con run_$2_${ifile}.con

    echo "Arguments      = Mask.sh $1 $2" >> run_$2_${ifile}.con
    echo "Output         = condor/$2_${ifile}.out" >> run_$2_${ifile}.con
    echo "Error          = condor/$2_${ifile}.err" >> run_$2_${ifile}.con
    echo "Log            = condor/$2_${ifile}.log" >> run_$2_${ifile}.con
    echo "Executable   = /bin/sh" >> run_$2_${ifile}.con
    echo "Queue"         >> run_$2_${ifile}.con 
    condor_submit run_$2_${ifile}.con

    mv run_$2_${ifile}.con script/

    let "ifile+=1"

