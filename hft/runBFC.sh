#!/bin/sh
starver dev

#root4star -b -q -l 'bfc.C(1,100,"in,pxlRaw,pxlDb,pxlCluster,pxlHit,PxlQA","st_pxl_14137092_raw_0120001.daq") > log '
root4star -b -q -l 'bfc.C(1,100,"pp2013a StiPulls pxlRaw pxlDb pxlCluster pxlHit pxlQA mtd btof VFMinuit beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt","st_pxl_14137092_raw_0120001.daq") > log '
