
This directory (offline/hft in STAR CVS) contains development version of hft
related software. The subdirectories (StRoot, StarDb, ...) include new
submodules along with modified sources of the respective HEAD versions. Once
tested the new classes and changes to the existing ones will be moved to the
official StRoot area.


How to build development hft libraries
======================================

    starver dev
    mkdir myhft
    cd myhft
    cvs checkout -r HEAD offline/hft
    cvs checkout -r HEAD StarDb
    cvs checkout -r HEAD StRoot/StBFChain
    cvs checkout -r HEAD StRoot/StEvent
    cvs checkout -r HEAD StRoot/StEventUtilities
    cvs checkout -r HEAD StRoot/StiMaker

    # The following package is in the official CVS StRoot area but not yet in
    # $STAR/StRoot. Check it out to compile locally as the official library is
    # not yet available

    cvs checkout -r HEAD StRoot/StPxlRawHitMaker

    cp -r offline/hft/StarDb/Calibrations StarDb/
    cp -r offline/hft/StarDb/Geometry StarDb/

    # Copy new files to the official StRoot area

    cp offline/hft/StRoot/StEvent/StIstHitCollection.cxx       StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstHitCollection.h         StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstHit.cxx                 StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstHit.h                   StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstLadderHitCollection.cxx StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstLadderHitCollection.h   StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstSensorHitCollection.cxx StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstSensorHitCollection.h   StRoot/StEvent/

    # The following new submodules can aslo be copied to your local StRoot but
    # you may chose to link them instead

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
    ln -s ../offline/hft/StRoot/StPxlMonMaker
    ln -s ../offline/hft/StRoot/StiPxl

    # Apply changes to existing files

    cd ..
    patch -p0 < offline/hft/StRoot/StBFChain.patch
    patch -p0 < offline/hft/StRoot/StEvent.patch
    patch -p0 < offline/hft/StRoot/StEventUtilities.patch
    patch -p0 < offline/hft/StRoot/StiMaker.patch
    cons


How to create patches for packages in official StRoot
=====================================================

In the directory containing StRoot execute the following commands:

    cvs update -r HEAD StRoot/StBFChain
    cvs update -r HEAD StRoot/StEvent
    cvs update -r HEAD StRoot/StEventUtilities
    cvs update -r HEAD StRoot/StiMaker

    cvs diff -u -r HEAD StRoot/StBFChain > offline/hft/StRoot/StBFChain.patch
    cvs diff -u -r HEAD StRoot/StEvent > offline/hft/StRoot/StEvent.patch
    cvs diff -u -r HEAD StRoot/StEventUtilities > offline/hft/StRoot/StEventUtilities.patch
    cvs diff -u -r HEAD StRoot/StiMaker > offline/hft/StRoot/StiMaker.patch


How to run tests
================

The BFC chain options for data and simulation processing can be found in

    offline/hft/runBFC.sh


To do
=====

- <del>Move development from StRoot/StiRnD/Hft to offline/hft/StRoot/StiPxl</del>
- <del>Rename StiPixel... classes to StiPxl... to be consistent with the other
detector subsystems</del>
- Add instructions for how to run simulation with hft detectors
- Add location for test simulation and data files
- <del>Need a patch for StiMaker due to StRoot/StiRnD/Hft -> offline/hft/StRoot/StiPxl change </del>
