<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [How to compile HFT libraries from STAR CVS repository](#how-to-compile-hft-libraries-from-star-cvs-repository)
- [How to compile HFT libraries from git repository](#how-to-compile-hft-libraries-from-git-repository)
- [How to run tests](#how-to-run-tests)
- [How to produce and reconstruct massive simulation for HFT ](#how-to-produce-and-reconstruct-massive-simulation-for-hft)
- [How to build and run standalone HFT tools](#how-to-build-and-run-standalone-hft-tools)
  - [Prerequisites: Setting up the environment](#prerequisites-setting-up-the-environment)
- [How to add PXL pileup events](#how-to-add-pxl-pileup-events)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->


This project is intended to support the offline reconstruction and analysis of
the data collected by the STAR experiment at RHIC and by the Heavy Flavor
Tracker (HFT) in particular. In the STAR CVS repository the corresponding codes
can be found in the 'StRoot/' and 'offline/hft/' directories. While the former
contains submodules which are regularly compiled and included in the official
STAR releases, the latter contains modules which are not yet included in the
official 'StRoot/' area and thus, need to be compiled by the user.


How to compile HFT libraries from STAR CVS repository
=====================================================

In order to use the code in the 'offline/hft/' modules one has to move it to
a local 'StRoot/' directory. Setup the environment and prepare directories

    starver dev
    mkdir star-soft
    cd star-soft

    cvs checkout -d star-offline-hft offline/hft

The following submodules can be copied to your local StRoot but you may chose to
link to them instead

    mkdir StRoot
    cd StRoot
    ln -s ../star-offline-hft/StRoot/StHftPool

If desired other CVS modules from `star-offline-hft/` can be linked in a similar
way. Build the libraries as:

    cd ..
    cons EXTRA_CXXFLAGS="-I${OPTSTAR}/include"

The `cons` builder will place the libraries in the local `.slXX_gccXXX` directory.


How to compile HFT libraries from git repository
================================================

    git clone https://github.com/plexoos/star-soft.git
    cd star-soft
    git submodule init
    git checkout hft-dev
    git submodule update --remote
    ./install.sh ./
    cons EXTRA_CXXFLAGS="-I${OPTSTAR}/include"

The `cons` builder will place the libraries in the local `.slXX_gccXXX` directory.


How to create HFT TTrees
========================

The local libraries compiled in the previous section can be used to reconstruct
simulated or raw STAR data:

    root4star -b -q -l 'bfc.C(1, <n_events>, "<bfc_options>", "<input_file>")'

A number of typical BFC option chains can be found in `star-offline-hft/runBFC.sh`

The reconstruction chains can be extended by running a maker from StHftPool
libary with the `HftMatTree` option. The chain will create a root file
(\*.hftree.root) with a tree that can be later used to visualize tracks and
hits along with selected detector volumes or study Sti track losses in these
volumes. The following is a sample chain of BFC options to reconstruct a \*.fz
file:

    tpcRS y2014a AgML MakeEvent ITTF StiHftC pxlFastSim istFastSim Idst BAna l0 Tree logger Sti tpcDB TpcHitMover TpxClu bbcSim btofsim tags emcY2 EEfs evout -dstout IdTruth geantout big fzin MiniMcMk clearmem HftMatTree


How to build and run standalone HFT tools
=========================================

To help with Sti geometry debugging and implementation we developed a number of
tools. This section explains how to build them as standalone executables.

In the following we assume the work is being done in the standard STAR
environment available on the interactive RCAS nodes.

Some pieces of the package rely on a couple of additional environment variables:

`OFFLINE_HFT_DIR` should contain the path to the local working directory with
an StRoot subdirectory in it. And

`OFFLINE_HFT_RESULTS_DIR` can contain any path to where the output results will
be saved

    cd StRoot
    ln -s ../star-offline-hft/StRoot/StHftPool

Assuming the commands from the "How to compile HFT libraries..." section above
have been executed one can do:

    cd star-soft
    mkdir build
    cd build
    cmake ../star-offline-hft
    make

When make is done one should see `tevedisp` in the current directory. The
program accepts either a ROOT file with a hftree TTree or a text file with
a list of such ROOT files (one per line) as input. The simplest command to
display events from hftree can look like this:

    tevedisp -f path/to/my.hftree.root
