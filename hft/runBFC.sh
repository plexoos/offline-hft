#!/bin/tcsh

# To read PXL raw data:
#
#setenv BFC_OPTIONS '"pp2013b pxlRaw pxlDb pxlCluster pxlHit pxlMon mtd btof VFMinuit beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt"'

# To run over PXL simulation:
#
#starsim -w 0 -b offline/hft/tests/singlepion.kumac
# 2011 production chain     "tpcRS  y2011      MakeEvent ITTF                                                   NoSsdIt NoSvtIt Idst BAna l0 ftpc pmd Tree logger Sti genvtx tpcDB TpcHitMover TpxClu bbcSim btofsim tags emcY2 EEfs evout -dstout IdTruth geantout big fzin MiniMcMk clearmem"
#setenv BFC_OPTIONS '"Debug2 tpcRS ry2014 AgML MakeEvent ITTF pxlFastSim pxlRaw pxlDb pxlCluster pxlHit PixelIt NoSsdIt NoSvtIt Idst BAna l0          Tree logger Sti VFMCE  tpcDB TpcHitMover TpxClu bbcSim btofsim tags emcY2 EEfs evout -dstout IdTruth geantout big fzin MiniMcMk clearmem"'
setenv BFC_OPTIONS '"Debug2 tpcRS ry2014 AgML MakeEvent ITTF pxlFastSim pxlRaw pxlDb pxlCluster pxlHit PixelIt NoSsdIt NoSvtIt Idst BAna l0          Tree logger Sti  tpcDB TpcHitMover TpxClu bbcSim btofsim tags emcY2 EEfs evout -dstout IdTruth geantout big fzin MiniMcMk clearmem"'
#setenv BFC_OPTIONS '"Debug2 y2014 AgML MakeEvent ITTF pxlRaw pxlDb pxlCluster pxlHit PixelIt NoSsdIt NoSvtIt logger Sti fzin"'
setenv BFC_INPFILE '"mytest.fz"'

# For IST
#
#setenv BFC_OPTIONS '"in,istRaw,istDb,istCluster,istHit,istQA"'

# For IST calibration
#
#setenv BFC_OPTIONS '"in,istRaw,istDb,istCalib"'

# Data files for tests
#
#setenv BFC_INPFILE '"/star/institutions/lbl_prod/hft/Run14/daq/038/15038019/st_cosmic_adc_15038019_raw_1000001.daq"'
#setenv BFC_INPFILE '"/star/institutions/lbl_prod/hft/Run14/daq/053/15053044/st_hft_15053044_raw_0500005.daq"'

root4star -b -q -l "bfc.C(1, 10, $BFC_OPTIONS, $BFC_INPFILE)"
