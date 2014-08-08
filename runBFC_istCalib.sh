#!/bin/sh
starver dev

rm istPedNoiseTable.dat
root4star -b -q -l 'bfc_calib.C(1,200,"in,istRaw,istDb,istCalib","/star/data03/daq/2014/075/15075079/st_physics_adc_15075079_raw_0500002.daq")'
root4star -b -q -l 'bfc_calib.C(1,1000,"in,istRaw,istDb,istCalib","/star/data03/daq/2014/075/15075079/st_physics_adc_15075079_raw_0500002.daq")'
