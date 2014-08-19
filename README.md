<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Prerequisites: Setting up the environment](#prerequisites-setting-up-the-environment)
- [How to compile HFT libraries from STAR CVS repository](#how-to-compile-hft-libraries-from-star-cvs-repository)
- [How to compile HFT libraries from git repository](#how-to-compile-hft-libraries-from-git-repository)
- [How to run tests](#how-to-run-tests)
- [How to produce and reconstruct massive simulation for HFT ](#how-to-produce-and-reconstruct-massive-simulation-for-hft)
- [How to build and run standalone HFT tools](#how-to-build-and-run-standalone-hft-tools)
- [How to add PXL pileup events](#how-to-add-pxl-pileup-events)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->


This project contains the code developed to support the offline reconstruction
and analysis of the data collected by the STAR experiment at RHIC and by the
Heavy Flavor Tracker (HFT) in particular. In the STAR CVS repository the
corresponding code can be found in 'offline/hft'. The subdirectories (StRoot,
...) include new submodules along with modified sources of the respective HEAD
versions. Once tested the new classes and changes to the existing ones are moved
to the official StRoot area.


Prerequisites: Setting up the environment
=========================================

In the following we assume the work is being done in the standard STAR
environment available on the interactive RCAS nodes.

Some pieces of the package rely on a couple of additional environment variables:

`OFFLINE_HFT_DIR` should contain the path to the local working directory with
an StRoot subdirectory in it. And

`OFFLINE_HFT_RESULTS_DIR` can contain any path to where the output results will
be saved

For consistency between the two installation methods discussed below we will
assume that after the code has been checked out from either CVS or git
repositories a soft link `offline-hft` was created in the `OFFLINE_HFT_DIR`
directory to point to `offline/hft` or `star-offline-hft` subdirectories
respectively.


How to compile HFT libraries from STAR CVS repository
=====================================================

Setup the environment and prepare directories

    starver dev
    mkdir ~/my_hft_test_dir
    cd ~/my_hft_test_dir

    cvs checkout -ko -r HEAD offline/hft

Checkout the modules which need to be patched

    cvs checkout -r HEAD StRoot/StBFChain
    cvs checkout -r HEAD StRoot/StiMaker
    cvs checkout -r HEAD StRoot/Sti
    cvs checkout -r HEAD StRoot/StEvent

    patch -p1 -d StRoot/StBFChain < offline/hft/StRoot/StBFChain.patch
    patch -p1 -d StRoot < offline/hft/StRoot/Sti.patch
    patch -p1 -d StRoot < offline/hft/StRoot/StiMaker.patch
    patch -p1 -d StRoot < offline/hft/StRoot/StEvent.patch

The following new submodules can also be copied to your local StRoot but you may
chose to link them instead

    cd StRoot
    ln -s ../offline/hft/StRoot/StIstCalibrationMaker
    ln -s ../offline/hft/StRoot/StIstClusterMaker
    ln -s ../offline/hft/StRoot/StIstFastSimMaker
    ln -s ../offline/hft/StRoot/StIstHitMaker
    ln -s ../offline/hft/StRoot/StIstQAMaker
    ln -s ../offline/hft/StRoot/StIstRawHitMaker
    ln -s ../offline/hft/StRoot/StPxlMonMaker
    ln -s ../offline/hft/StRoot/StHftPool
    ln -s ../offline/hft/StRoot/StiSsd
    ln -s ../offline/hft/StRoot/StSsdDbMaker

    cd ..

    cons EXTRA_CXXFLAGS="-I${OPTSTAR}/include"

The 'cons' builder places the libraries in the local
`~/my_hft_test_dir/.slXX_gccXXX` directory.


How to compile HFT libraries from git repository
================================================

    git clone https://github.com/plexoos/star-soft.git
    cd star-soft
    git checkout hft-dev
    git submodule init
    git submodule update --remote
    git submodule foreach -q --recursive 'branch="$(git config -f $toplevel/.gitmodules submodule.$name.branch)"; git checkout $branch'
    ./instal.sh ./
    cons EXTRA_CXXFLAGS="-I${OPTSTAR}/include"


How to run tests
================

The local libraries compiled in the previous section can be used to reconstruct
simulated or raw STAR data:

    root4star -b -q -l 'bfc.C(1, <n_events>, "<bfc_options>", "<input_file>")'

A number of typical BFC option chains can be found in `offline-hft/runBFC.sh`

The reconstruction chains can be extended by running a maker from StHftPool
libary with the 'HftMatTree' option. The chain will create a root file
(\*.hftree.root) with a tree that can be later used to visualize tracks and
hits along with selected detector volumes or study Sti track losses in these
volumes. The following is a sample chain of BFC options to reconstruct an .fz
file:

    tpcRS y2014a AgML MakeEvent ITTF StiHftC pxlFastSim istFastSim Idst BAna l0 Tree logger Sti tpcDB TpcHitMover TpxClu bbcSim btofsim tags emcY2 EEfs evout -dstout IdTruth geantout big fzin MiniMcMk clearmem HftMatTree

For reference, we include a simple starsim kumac to produce events with tracks
in transverse planes. It can be found in `offline-hft/tests/singlepion.kumac`


How to produce and reconstruct massive simulation for HFT 
=========================================================




    offline-hft/tests/submit_jobs_stiscan_zslice.sh offline-hft/tests/job_template_stiscan_zslice.xml

    offline-hft/tests/submit_jobs_stiscan_zslice.sh offline-hft/tests/job_template_stiscan_zslice_macro.xml


    offline-hft/tests/submit_jobs_hftree.sh /path/to/filelist_fz


How to build and run standalone HFT tools
=========================================

To help with Sti geometry debugging and implementation we developed a number of
tools. This section explains how to build them as standalone executables.
Assuming the commands from the "How to build development hft libraries" section
above have been executed one can do:

    cd ~/my_hft_test_dir
    mkdir build
    cd build
    cmake ../offline-hft
    make

When make is done one should see two programs 'stiscan' and 'tevedisp' build
in the current directory. Both programs accept either a ROOT file with a hftree
TTree or a text file with a list of such ROOT files (one per line) as input.
The simplest command to display the events in hftree can look like this:

    tevedisp -f path/to/my.hftree.root

To produce a set of basic histograms with track energy lost in Sti volumes one
can do:

    stiscan -f path/to/my.hftree.root

By default, only sensitive PXL and IST layers are used in the energy loss
analysis. One can easily specify any other volume or a set of volumes to be
considered in the analysis by using regular PERL expressions. Here are a few
examples:

    tevedisp -f path/to/my.hftree.root -p "^.*IDSM_1/PXMO_1/PXLA_[\d]+/PXT[RML]_.*$" -g
    stiscan -f path/to/my_hftree_list -l path/to/my_volume_name_pattern_list -s 0.10 -g

Either a regex pattern or a text file with a list of regex patterns
(`my_volume_name_pattern_list` in the above example) can be provided. It is used
to match the names of TGeo volumes to select only specific physical volumes.

*Note:* The programs expect to find a ROOT file (y2014a.root) with the full
STAR geometry in the current directory. Such file can be produced with the
following command:

    root -l '$STAR/StarVMC/Geometry/macros/viewStarGeometry.C("y2014a")'


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