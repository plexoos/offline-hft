
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

    # Checkout the modules which need to be patched

    cvs checkout -r HEAD StRoot/StBFChain
    cvs checkout -r HEAD StRoot/StiMaker
    cvs checkout -r HEAD StRoot/Sti
    cvs checkout -r HEAD StRoot/StEvent

    patch -p1 -d StRoot/StBFChain < offline/hft/StRoot/StBFChain.patch
    patch -p1 -d StRoot < offline/hft/StRoot/Sti.patch
    patch -p1 -d StRoot < offline/hft/StRoot/StiMaker.patch
    patch -p1 -d StRoot < offline/hft/StRoot/StEvent.patch

    # The following new submodules can also be copied to your local StRoot but
    # you may chose to link them instead

    cd StRoot
    ln -s ../offline/hft/StRoot/StIstCalibrationMaker
    ln -s ../offline/hft/StRoot/StIstClusterMaker
    ln -s ../offline/hft/StRoot/StIstFastSimMaker
    ln -s ../offline/hft/StRoot/StIstHitMaker
    ln -s ../offline/hft/StRoot/StIstQAMaker
    ln -s ../offline/hft/StRoot/StIstRawHitMaker
    ln -s ../offline/hft/StRoot/StPxlMonMaker
    ln -s ../offline/hft/StRoot/StiTest
    ln -s ../offline/hft/StRoot/StHftPool

    cd ..

    cons


How to compile standalone executables
=====================================

Set the environment OFFLINE_HFT_DIR variable to contain the path to your local
working directory, e.g.

    setenv OFFLINE_HFT_DIR ~/my_hft_test_dir
    mkdir build
    cd build
    cmake28 ../offline/hft
    make


How to run tests
================

The BFC chain options for data and simulation processing can be found in

    offline/hft/runBFC.sh


How to look at events with event display
========================================

Run a BFC chain with the HftMatTree option. See offline/hft/runBFC.sh for
example BFC options. A file named EventAAAAAAA.root will be created in the
current directory. To see the produced events in the event display execute the
following commands:

    mkdir output
    mv EventAAAAAAA.root output/Event_NNNNNN.root
    root -l offline/hft/tests/displayHft.C
    root.exe [1] hft_display(-1,NNNNNN);


How to add PXL pileup events
============================

You need to patch StPxlSimMaker with the pileup adder as follows:

First, checkout the patch (notice the -ko option):

    cvs co -ko offline/hft/StRoot/StPxlSimMaker_pileupAdder.patch
    cvs co -ko offline/hft/runPxlSimWithPileup.csh

Then checkout HEAD StPxlSimMaker from official STAR repo:

    cvs co StRoot/StPxlSimMaker

Now patch it:

    cd StRoot/
    patch -p1 < ../offline/hft/StRoot/StPxlSimMaker_pileupAdder.patch

to run it, look at:

    offline/hft/runPxlSimWithPileup.csh
