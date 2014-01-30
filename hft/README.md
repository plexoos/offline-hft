
This directory (offline/hft in STAR CVS) contains development version of hft
related software. The subdirectories (StRoot, StarDb, ...) include new
submodules along with modified sources of the respective HEAD versions. Once
tested the new classes and changes to the existing ones will be moved to the
official StRoot area.


How to build development hft libraries
======================================

    cvs checkout -r HEAD offline/hft
    cvs checkout -r HEAD StRoot/StEvent
    cvs checkout -r HEAD StRoot/StRnD

    cp offline/hft/StRoot/StEvent/StIstHitCollection.cxx       StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstHitCollection.h         StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstHit.cxx                 StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstHit.h                   StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstLadderHitCollection.cxx StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstLadderHitCollection.h   StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstSensorHitCollection.cxx StRoot/StEvent/
    cp offline/hft/StRoot/StEvent/StIstSensorHitCollection.h   StRoot/StEvent/

    cd StRoot
    ln -s ../offline/hft/StRoot/StBFChain
    ln -s ../offline/hft/StRoot/StEventUtilities
    ln -s ../offline/hft/StRoot/StIstCalibrationMaker
    ln -s ../offline/hft/StRoot/StIstClusterMaker
    ln -s ../offline/hft/StRoot/StIstDbMaker
    ln -s ../offline/hft/StRoot/StIstFastSimMaker
    ln -s ../offline/hft/StRoot/StIstHitMaker
    ln -s ../offline/hft/StRoot/StIstQAMaker
    ln -s ../offline/hft/StRoot/StIstRawHitMaker
    ln -s ../offline/hft/StRoot/StIstUtil
    ln -s ../offline/hft/StRoot/StPxlClusterMaker
    ln -s ../offline/hft/StRoot/StPxlDbMaker
    ln -s ../offline/hft/StRoot/StPxlHitMaker
    ln -s ../offline/hft/StRoot/StPxlQAMaker
    ln -s ../offline/hft/StRoot/StPxlRawHitMaker
    ln -s ../offline/hft/StRoot/StPxlUtil

    cd ..
    patch -p0 < offline/hft/StRoot/StEvent.patch
    cons


To do
=====

- Move development from StRoot/StiRnD/Hft to offline/hft/StRoot/StiPxl
