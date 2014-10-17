#!/bin/csh
set lines=`cat auau200_runid_timestamp_minus_10s`
set i=1
while ( $i <= $#lines )
    set runid = `echo $lines[$i]`
    @ i = $i + 1
    set stamp = `echo $lines[$i]`
    echo $runid $stamp
    runAll.sh $runid $stamp
    @ i = $i + 1
end
