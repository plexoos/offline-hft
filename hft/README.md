
This directory (offline/hft in STAR CVS) contains development version of hft
related software. The subdirectories (StRoot, ...) include new
submodules along with modified sources of the respective HEAD versions. Once
tested the new classes and changes to the existing ones will be moved to the
official StRoot area.


How to build development hft libraries
======================================

    # Setup the environment and prepare directories

    starver dev
    mkdir my_hft_test_dir
    cd my_hft_test_dir

    cvs checkout -r HEAD offline/hft

    # The following package is in the official CVS StRoot area but not yet in
    # $STAR/StRoot. Check it out to compile locally as the official library is
    # not yet available (as of March 11, 2014)

    cvs checkout -r HEAD StRoot/StPxlRawHitMaker

    # According to Xin simulation jobs cannot use the pixel survey geometry
    # from the STAR database due to improper default setting of the simulation
    # timestamp... Hence, one need to use the local files from CVS:

    mkdir -p StarDb/Geometry
    cp -r offline/hft/StarDb/Geometry/pxl StarDb/Geometry
    cp -r offline/hft/StarDb/Geometry/ist StarDb/Geometry

    # The following new submodules can also be copied to your local StRoot but
    # you may chose to link them instead

    cd StRoot
    ln -s ../offline/hft/StRoot/StIstCalibrationMaker
    ln -s ../offline/hft/StRoot/StIstClusterMaker
    ln -s ../offline/hft/StRoot/StIstFastSimMaker
    ln -s ../offline/hft/StRoot/StIstHitMaker
    ln -s ../offline/hft/StRoot/StIstQAMaker
    ln -s ../offline/hft/StRoot/StIstRawHitMaker
    ln -s ../offline/hft/StRoot/StPxlClusterMaker
    ln -s ../offline/hft/StRoot/StPxlHitMaker
    ln -s ../offline/hft/StRoot/StPxlMonMaker

    cd ..

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


Modules required before dev production released
===============================================

    # Setup the environment and prepare directories

    starver dev
    mkdir my_hft_test_dir
    cd my_hft_test_dir

    # Checkout the new code

    cvs checkout -r HEAD offline/hft

    # Checkout the modules which need to be patched

    cvs checkout -r HEAD StRoot/StBFChain
    cvs checkout -r HEAD StRoot/StEvent
    cvs checkout -r HEAD StRoot/StEventUtilities
    cvs checkout -r HEAD StRoot/Sti
    cvs checkout -r HEAD StRoot/StiMaker
    cvs checkout -r HEAD StRoot/StiSsd
    cvs checkout -r HEAD StRoot/StIstDbMaker
    cvs checkout -r HEAD StRoot/StIstUtil
    cvs checkout -r HEAD StRoot/StiPxl
    cvs checkout -r HEAD StRoot/StiIst
