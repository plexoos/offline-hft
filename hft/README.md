
This directory (offline/hft in STAR CVS) contains development version of hft
related software. The subdirectories (StRoot, ...) include new
submodules along with modified sources of the respective HEAD versions. Once
tested the new classes and changes to the existing ones will be moved to the
official StRoot area.


How to build development hft libraries
======================================

    starver dev
    mkdir myhft
    cd myhft
    cvs checkout -r HEAD -ko offline/hft
    cvs checkout -r HEAD StRoot/StBFChain
    cvs checkout -r HEAD StRoot/StEvent
    cvs checkout -r HEAD StRoot/StEventUtilities
    cvs checkout -r HEAD StRoot/StiMaker
    cvs checkout -r HEAD StRoot/Sti

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

    # Rollback the change in StarVMC to group pxl volumes in an assembly before
    # its fully tested

    cvs co StarVMC
    cvs update -r 1.4 StarVMC/Geometry/PixlGeo/PsupGeo.xml

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
    ln -s ../offline/hft/StRoot/StiIst

    # Apply changes to existing files

    patch -p1 < ../offline/hft/StRoot/StEvent.patch
    patch -p1 < ../offline/hft/StRoot/StEventUtilities.patch
    patch -p1 < ../offline/hft/StRoot/Sti_StiMaker.patch

    cd ..

    patch -p0 < offline/hft/StRoot/StBFChain.patch

    cons +StarVMC/Geometry
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


Patching for preview production release
=======================================

    # Setup the environment and prepare directories

    starver dev
    mkdir my_test_dir
    cd my_test_dir

    # Checkout the new code

    cvs checkout -r HEAD -ko offline/hft

    # Checkout the modules which need to be patched

    cvs checkout -r HEAD StRoot/StBFChain
    cvs checkout -r HEAD StRoot/StEvent
    cvs checkout -r HEAD StRoot/StEventUtilities
    cvs checkout -r HEAD StRoot/Sti
    cvs checkout -r HEAD StRoot/StiMaker
    cvs checkout -r HEAD StRoot/StiSsd

    # Patch the existing code

    patch -d StRoot/StBFChain -p1 < offline/hft/StRoot/StBFChain.patch
    patch -d StRoot -p1 < offline/hft/StRoot/Sti_StiMaker.patch
    patch -d StRoot -p1 < offline/hft/StRoot/StEvent.patch

    # Rollback the change in StarVMC that groups PXL volumes in an assembly
    # before its fully tested

    cvs co -r HEAD StarVMC
    cvs update -r 1.4 StarVMC/Geometry/PixlGeo/PsupGeo.xml

    # The following new submodules should be copied to local StRoot for future
    # transfer

    cp -r offline/hft/StRoot/StIstDbMaker StRoot/
    cp -r offline/hft/StRoot/StIstUtil StRoot/
    cp -r offline/hft/StRoot/StiPxl StRoot/
    cp -r offline/hft/StRoot/StiIst StRoot/

    cons +StarVMC/Geometry
    cons
