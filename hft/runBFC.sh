#!/bin/bash


# To read PXL raw data:
#
# Add pxlMon to produce QA histograms
#
# Current Run 14 production options with HFT systems. Add StiIdealGeom to switch between Sti
# geometry built with either default ROOT/XML or DB-based geometry transofrmations
#
#export BFC_OPTIONS='"DbV20140412 P2014a  mtd btof BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt StiIdealGeom"'
#
# As above Run 14 production options extended with options to use HFT hits
#
 export BFC_OPTIONS='"DbV20140412 P2014a  mtd btof BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt pxlRaw pxlCluster pxlHit istRaw istCluster istHit sstHit"'
#
# HftMatTree:   Creates custom tree. Can be used with offline/hft/tests/hft_display.C to produce event displays
#
# Standard production options for Run 13 extended with simple test geometries
#
#export BFC_OPTIONS='"DbV20140222 pp2013c mtd btof fmsDat fgt fgtPoint VFPPVnoCTB beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt pxlDb PxlIT StiPxlSimpleBox"'
#export BFC_OPTIONS='"DbV20140222 pp2013c mtd btof fmsDat fgt fgtPoint VFPPVnoCTB beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt pxlDb PxlIT StiPxlSimpleBox2"'
#export BFC_OPTIONS='"DbV20140222 pp2013c mtd btof fmsDat fgt fgtPoint VFPPVnoCTB beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt pxlDb PxlIT StiPxlSimplePlane"'
#export BFC_OPTIONS='"DbV20140222 pp2013c mtd btof fmsDat fgt fgtPoint VFPPVnoCTB beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt pxlDb PxlIT StiPxlSimpleTube"'


# To run over HFT simulation:
#
# Uncomment next starsim command to generate a simple simulation with 20 events.
# A mytest.fz file should be generated and it can be used as input for the
# sample simulation BFC chain below
#
#starsim -w 0 -b offline/hft/tests/singlepion.kumac
#
# For comparison purposes the next line shows the options used for the official 2011 production
#
# 2011 production chain     "tpcRS  y2011         MakeEvent ITTF                    NoSsdIt NoSvtIT Idst BAna l0 ftpc pmd Tree logger Sti genvtx tpcDB TpcHitMover TpxClu bbcSim btofsim tags emcY2 EEfs evout -dstout IdTruth geantout big fzin MiniMcMk clearmem"
# 
#export BFC_OPTIONS='"       tpcRS ry2014a AgML MakeEvent ITTF pxlFastSim StiHftC Idst BAna l0 Tree logger Sti tpcDB TpcHitMover TpxClu bbcSim btofsim tags emcY2 EEfs evout -dstout IdTruth geantout big fzin MiniMcMk clearmem pxlHit"'
#
#export BFC_INPFILE='"mytest.fz"'


# For IST
#
#export BFC_OPTIONS='"in,istRaw,istDb,istCluster,istHit,istQA"'


# For IST calibration
#
#export BFC_OPTIONS='"in,istRaw,istDb,istCalib"'


# Data files for tests
#
#export BFC_INPFILE='"/star/institutions/lbl_prod/hft/Run14/daq/038/15038019/st_cosmic_adc_15038019_raw_1000001.daq"'
#export BFC_INPFILE='"/star/institutions/lbl_prod/hft/Run14/daq/053/15053044/st_hft_15053044_raw_0500005.daq"'
#export BFC_INPFILE='"/star/data03/daq/2013/152/14152026/st_physics_14152026_raw_1920001.daq"'
 export BFC_INPFILE='"/star/data03/daq/2014/079/15079042/st_physics_15079042_raw_5500008.daq"'

root4star -b -q -l "bfc.C(1, 5, $BFC_OPTIONS, $BFC_INPFILE)"
