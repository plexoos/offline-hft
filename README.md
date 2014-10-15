<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [Prerequisites: Setting up the environment](#prerequisites-setting-up-the-environment)
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


How to compile HFT libraries from STAR CVS repository
=====================================================

Setup the environment and prepare directories

    starver dev
    mkdir star-soft
    cd star-soft

    cvs checkout -d star-offline-hft offline/hft

The following submodules can be copied to your local StRoot but you may chose to
link to them instead

    mkdir StRoot
    cd StRoot
    ln -s ../star-offline-hft/StRoot/StHftPool
    ln -s ../star-offline-hft/StRoot/StiScan

If desired other CVS modules from star-offline-hft can be linked in a similar
way. Some official modules in CVS have changes not in the repository. One can
copy them from the following locations:

    cp -r /star/institutions/bnl_me/smirnovd/public/star-soft/StRoot/StarGenerator .
    cp -r /star/institutions/bnl_me/smirnovd/public/star-soft/StRoot/StiMaker .
    cp -r /star/institutions/bnl_me/smirnovd/public/star-soft/StRoot/Sti .

    cd ..
    cons EXTRA_CXXFLAGS="-I${OPTSTAR}/include"

The `cons` builder will place the libraries in the local `.slXX_gccXXX` directory.


How to compile HFT libraries from git repository
================================================

    git clone https://github.com/plexoos/star-soft.git
    cd star-soft
    git submodule init
    git submodule update --remote
    ./install.sh ./
    cons EXTRA_CXXFLAGS="-I${OPTSTAR}/include"

The `cons` builder will place the libraries in the local `./.slXX_gccXXX` directory.

If desired one can switch to an experimental branch and recompile. The commands
are identical to the above except one need to checkout the `hft-dev` branch:

    git clone https://github.com/plexoos/star-soft.git
    cd star-soft
    git submodule init
    git checkout hft-dev
    git submodule update --remote
    ./install.sh ./
    cons EXTRA_CXXFLAGS="-I${OPTSTAR}/include"


How to run tests
================

The local libraries compiled in the previous section can be used to reconstruct
simulated or raw STAR data:

    root4star -b -q -l 'bfc.C(1, <n_events>, "<bfc_options>", "<input_file>")'

A number of typical BFC option chains can be found in `star-offline-hft/runBFC.sh`

The reconstruction chains can be extended by running a maker from StHftPool
libary with the 'HftMatTree' option. The chain will create a root file
(\*.hftree.root) with a tree that can be later used to visualize tracks and
hits along with selected detector volumes or study Sti track losses in these
volumes. The following is a sample chain of BFC options to reconstruct a \*.fz
file:

    tpcRS y2014a AgML MakeEvent ITTF StiHftC pxlFastSim istFastSim Idst BAna l0 Tree logger Sti tpcDB TpcHitMover TpxClu bbcSim btofsim tags emcY2 EEfs evout -dstout IdTruth geantout big fzin MiniMcMk clearmem HftMatTree

For reference, we include a simple starsim kumac to produce events with tracks
in transverse planes. It can be found in `star-offline-hft/tests/singlepion.kumac`


How to produce and reconstruct massive simulation for HFT 
=========================================================

A couple of shell scripts are provided to simplify the submission of condor jobs
to the farm. The following commands can be used as examples:

    star-offline-hft/tests/submit_jobs_stiscan_zslice.sh star-offline-hft/tests/job_template_stiscan_zslice.xml
    star-offline-hft/tests/submit_jobs_stiscan_zslice.sh star-offline-hft/tests/job_template_stiscan_zslice_macro.xml

    star-offline-hft/tests/submit_jobs_hftree.sh /path/to/filelist_fz

`submit_jobs_stiscan_zslice.sh` takes an xml template as the only parameter. In
our case the `starsim` command is executed to produce \*.fz files with simulated
events.

`submit_jobs_hftree.sh` utilizes the `offline/hft/tests/job_template_hftree.xml`
template to run over the \*.fz files specified in the input list. The jobs run
the standard reconstruction and in addition creates a ROOT file with an HFT
tree. The latter can be controlled by changing the BFC options hard-coded in the
script.


How to build and run standalone HFT tools
=========================================

To help with Sti geometry debugging and implementation we developed a number of
tools. This section explains how to build them as standalone executables.
Assuming the commands from the "How to compile HFT libraries..." section above
have been executed one can do:

    cd star-soft
    mkdir build
    cd build
    cmake ../star-offline-hft
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
