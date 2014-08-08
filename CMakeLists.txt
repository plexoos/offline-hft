#
# Usage:
#
# cd <local dir>
# mkdir build
# cd build
# cmake <../>offline/hft
# make
#


cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
PROJECT(OFFLINE_HFT)

seT(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})

OPTION(FORCE_32BIT "compile 32-bit application" ON)
OPTION(Boost_NO_BOOST_CMAKE "Due to bug in boost cmake files in Scientific Linux" ON)

IF(FORCE_32BIT)
   SET_PROPERTY(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
ENDIF()

# Find boost headers and library locations
set(Boost_USE_MULTITHREADED OFF)
set(BOOST_INCLUDEDIR $ENV{OPTSTAR}/include)
set(BOOST_LIBRARYDIR $ENV{OPTSTAR}/lib)

find_package( Boost 1.54.0 COMPONENTS program_options regex)
if(NOT Boost_FOUND)
   message(FATAL_ERROR "Fatal error: Boost (version >= 1.54.0) not found in standard location")
endif()

find_package(MySQL)
find_package(ROOT)

include_directories(
   ${Boost_INCLUDE_DIR}
   ${ROOT_INCLUDE_DIR}
   ${ROOTSYS}
   $ENV{OFFLINE_HFT_DIR}/offline/hft/tests
   $ENV{OFFLINE_HFT_DIR}/StRoot
   /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/
   /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/StRoot
   /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/.$ENV{STAR_HOST_SYS}/include
   /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/StRoot/StJetMaker/tracks
)

link_directories(
   ${Boost_LIBRARY_DIR}
   ${ROOT_LIBRARY_DIR}
   $ENV{OFFLINE_HFT_DIR}/.$ENV{STAR_HOST_SYS}/lib
   $ENV{STAR_LIB}
   /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/.$ENV{STAR_HOST_SYS}/LIB
   $ENV{OPTSTAR}/lib
)

set( HFT_LIBRARIES
   StHftPoolEventT
   StHftPoolHftMatchedTree
)

set( STAR_LIBRARIES
   fastjet
   siscone
   siscone_spherical
   fastjetplugins
   St_base
   StChain
   StarRoot
   St_Tables
   StUtilities
   StTreeMaker
   StIOMaker
   StarClassLibrary
   StTriggerDataMaker
   StBichsel
   StEvent
   StEventUtilities
   StDbLib
   StEmcUtil
   StTofUtil
   StPmdUtil
   StPreEclMaker
   StStrangeMuDstMaker
   StMuDSTMaker
   StDaqLib
   StFtpcSlowSimMaker
   StFtpcTrackMaker
   StFtpcClusterMaker
   StFtpcTrackMaker
   StDAQMaker
   StDetectorDbMaker
   StTpcDb
   StDbUtilities
   StDbBroker
   St_db_Maker
   StEEmcUtil
   StEEmcDbMaker
   StTriggerFilterMaker
   StTriggerUtilities
   StSpinDbMaker
   StEmcRawMaker
   StEmcADCtoEMaker
   StJetSkimEvent
   StJets
   StSpinDbMaker
   StEmcTriggerMaker
   StTriggerUtilities
   StMCAsymMaker
   StRandomSelector
   StJetEvent
   StJetFinder
   StJetMaker
   StMcEvent
   StMcEventMaker
   StEmcSimulatorMaker
   StEEmcSimulatorMaker
   StEpcMaker
   StSvtClassLibrary
   Sti
   StiUtilities
   StIstDbMaker
   Geometry
   StarAgmlLib
   St_geant_Maker
   St_g2t
   StarGeneratorStep
)

add_executable( tevedisp tests/tevedisp.cc
      /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/.$ENV{STAR_HOST_SYS}/obj/asps/rexe/TGeant3/TGiant3.o
      /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/.$ENV{STAR_HOST_SYS}/obj/asps/rexe/root4star_Cint.o
      /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/.$ENV{STAR_HOST_SYS}/obj/asps/rexe/TGeant3/StarMC.o
      )
target_link_libraries( tevedisp ${HFT_LIBRARIES} ${Boost_LIBRARIES} ${STAR_LIBRARIES} ${ROOT_LIBRARIES} ${MYSQL_LIBRARY} Table Geom Eve)

add_executable( stiscan tests/stiscan.cc
      /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/.$ENV{STAR_HOST_SYS}/obj/asps/rexe/TGeant3/TGiant3.o
      /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/.$ENV{STAR_HOST_SYS}/obj/asps/rexe/root4star_Cint.o
      /afs/rhic.bnl.gov/star/packages/$ENV{STAR_VERSION}/.$ENV{STAR_HOST_SYS}/obj/asps/rexe/TGeant3/StarMC.o
      )
target_link_libraries( stiscan ${HFT_LIBRARIES} ${Boost_LIBRARIES} ${STAR_LIBRARIES} ${ROOT_LIBRARIES} Table Geom Eve)

if(FORCE_32BIT)
   set_target_properties(tevedisp PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32 -Wl,--warn-unresolved-symbols")
   set_target_properties(stiscan PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32 -Wl,--warn-unresolved-symbols")
endif()
