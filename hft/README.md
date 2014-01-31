
This directory (offline/hft in STAR CVS) contains development version of hft
related software. The subdirectories (StRoot, StarDb, ...) include new
submodules along with modified sources of the respective HEAD versions. Once
tested the new classes and changes to the existing ones will be moved to the
official StRoot area.


How to build development hft libraries
======================================

    starver dev
    cvs checkout -r HEAD offline/hft
    cvs checkout -r HEAD StarDb
    cvs checkout -r HEAD StRoot/StBFChain
    cvs checkout -r HEAD StRoot/StEvent
    cvs checkout -r HEAD StRoot/StEventUtilities
    cvs checkout -r HEAD StRoot/StiRnD

    # Checkout temporarily to compile localy as the official library is not yet
    # available
    cvs checkout -r HEAD StRoot/StPxlDbMaker
    cvs checkout -r HEAD StRoot/StPxlRawHitMaker
    cvs checkout -r HEAD StRoot/StPxlUtil

    cp -r offline/hft/StarDb/Calibrations StarDb/
    cp -r offline/hft/StarDb/Geometry StarDb/
    cp offline/hft/StRoot/StEvent/StIstHitCollection.cxx       StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstHitCollection.h         StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstHit.cxx                 StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstHit.h                   StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstLadderHitCollection.cxx StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstLadderHitCollection.h   StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstSensorHitCollection.cxx StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstSensorHitCollection.h   StRoot/StEvent/

    cd StRoot
    ln -s ../offline/hft/StRoot/StIstCalibrationMaker
    ln -s ../offline/hft/StRoot/StIstClusterMaker
    ln -s ../offline/hft/StRoot/StIstDbMaker
    ln -s ../offline/hft/StRoot/StIstFastSimMaker
    ln -s ../offline/hft/StRoot/StIstHitMaker
    ln -s ../offline/hft/StRoot/StIstQAMaker
    ln -s ../offline/hft/StRoot/StIstRawHitMaker
    ln -s ../offline/hft/StRoot/StIstUtil
    ln -s ../offline/hft/StRoot/StPxlClusterMaker
    ln -s ../offline/hft/StRoot/StPxlHitMaker
    ln -s ../offline/hft/StRoot/StPxlQAMaker

    cd ..
    patch -p0 < offline/hft/StRoot/StBFChain.patch
    patch -p0 < offline/hft/StRoot/StEvent.patch
    patch -p0 < offline/hft/StRoot/StEventUtilities.patch
    cons


How to run tests
================

To read PXL raw data:

    export BFC_OPTIONS='"pp2013b pxlRaw pxlDb pxlCluster pxlHit pxlQA mtd btof VFMinuit beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt"'
    export BFC_INPFILE='"/star/institutions/lbl_prod/hft/Run13/daq/14157027/daq/st_physics_14157027_raw_5480001.daq"'
    export BFC_INPFILE='"/star/institutions/lbl_prod/hft/Run13/daq/14157071/daq/st_pxl_adc_14157071_raw_6990001.daq"'

To run PXL simulation:

    export BFC_OPTIONS='"tpcRS y2014 MakeEvent ITTF StiRnD PixelIt NoSsdIt NoSvtIt pxlDb pxlFastSim Idst BAna l0 Tree logger Sti VFMC E tpcDB TpcHitMover TpxClu bbcSim btofsim tags emcY2 EEfs evout -dstout IdTruth geantout big fzin MiniMcMk clearmem"'
    export BFC_INPFILE='"test.fz"'

    root4star -b -q -l "bfc.C(1, 100, $BFC_OPTIONS, $BFC_INPFILE)"

For IST

    root4star -b -q -l 'bfc.C(1, 5000, "in,istRaw,istDb,istCluster,istHit,istQA", "st_physics_15016035_raw_1000001.daq")'

For IST calibration

    root4star -b -q -l 'bfc.C(1, 5000, "in,istRaw,istDb,istCalib", "st_physics_15016035_raw_1000001.daq")'


To do
=====

- Move development from StRoot/StiRnD/Hft to offline/hft/StRoot/StiPxl
- Rename StiPixel... classes to StiPxl... to be consistent with the other
detector subsystems
- Add instructions for how to run simulation with hft detectors
- Add location for test simulation and data files
