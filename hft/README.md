
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

    # For simulation jobs you may need to use the local geometry files from CVS:

    mkdir -p StarDb/Geometry
    cp -r offline/hft/StarDb/Geometry/pxl StarDb/Geometry
    cp -r offline/hft/StarDb/Geometry/ist StarDb/Geometry

    # Checkout the modules which need to be patched

    cvs checkout -r HEAD StRoot/StBFChain
    patch -p1 -d StRoot/StBFChain < offline/hft/StRoot/StBFChain.patch

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


How to run tests
================

The BFC chain options for data and simulation processing can be found in

    offline/hft/runBFC.sh
