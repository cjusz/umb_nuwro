cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project (NUWRO)

set (NUWRO_VERSION_MAJOR 11)
set (NUWRO_VERSION_MINOR 18) #The q+1'th letter of the alphabet
set (NUWRO_VERSION_REVISION 0)

enable_language(Fortran)

set (VERBOSE TRUE)

set (CMAKE_SKIP_BUILD_RPATH TRUE)

if(NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE DEBUG)
endif()

set (NUWRO_SINGLE_LIB TRUE)

if(DEFINED ENV{INSTALL_PREFIX})
  set(CMAKE_INSTALL_PREFIX $ENV{INSTALL_PREFIX})
else()
  set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/${CMAKE_SYSTEM_NAME})
endif()

################################################################################
#                            Check Dependencies
################################################################################

##################################  ROOT  ######################################
if ( NOT DEFINED ENV{ROOTSYS} )
  message (FATAL_ERROR "$ROOTSYS is not defined, please set up root first.")
endif()

include ($ENV{ROOTSYS}/etc/cmake/FindROOT.cmake)

if ( NOT ROOT_FOUND )
  message (FATAL_ERROR "[ROOT]: FindROOT.cmake could not be found, or did not execute as expected is ROOT installed in $ROOTSYS = $ENV{ROOTSYS}?")
else()
  execute_process (COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process (COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process (COMMAND root-config --version OUTPUT_VARIABLE ROOT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  message ( STATUS "[ROOT]: root-config --version: " ${ROOT_VERSION})
  message ( STATUS "[ROOT]: root-config --cflags: " ${ROOT_CXX_FLAGS} )
  message ( STATUS "[ROOT]: root-config --libs: " ${ROOT_LD_FLAGS} )
endif()

if(${ROOT_VERSION} MATCHES "6.*")
  set(ISROOT6 TRUE)
  message( STATUS "[ROOT]: Using ROOT 6.")
else()
  set(ISROOT6 FALSE)
endif()

################################## Pythia6 ####################################

if(DEFINED ENV{PYTHIA6})
  set(PYTHIA6LIBLOC "-L$ENV{PYTHIA6}")
else()
  set(PYTHIA6LIBLOC)
endif()

################################## COMPILER ####################################

set(CXX_WARNINGS "-Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -Wno-unused-but-set-variable -Wno-reorder")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 ${ROOT_CXX_FLAGS} ${CXX_WARNINGS} ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -O3 ${ROOT_CXX_FLAGS} ${CXX_WARNINGS} ")

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS_DEBUG} ${ROOT_LD_FLAGS} -lEG -lEGPythia6 -lGeom -lMinuit ${PYTHIA6LIBLOC} -lPythia6")
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RELEASE")
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS_RELEASE} ${ROOT_LD_FLAGS} -lEG -lEGPythia6 -lGeom -lMinuit ${PYTHIA6LIBLOC} -lPythia6 -Wl,--no-as-needed")
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
else()
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS_RELEASE} ${ROOT_LD_FLAGS} -lEG -lEGPythia6 -lGeom -lMinuit  ${PYTHIA6LIBLOC} -lPythia6 -Wl,--no-as-needed")
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
endif()

if (VERBOSE)
  message (STATUS "C++ Compiler      : " ${CXX_COMPILER_NAME})
  message (STATUS "    Release flags : " ${CMAKE_CXX_FLAGS_RELEASE})
  message (STATUS "    Debug flags   : " ${CMAKE_CXX_FLAGS_DEBUG})
  message (STATUS "    Link Flags    : " ${CMAKE_LINK_FLAGS})
endif()


################################################################################
#                            Specify Target Subdirs
################################################################################

add_subdirectory(src)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/setup.sh
  "source $ENV{ROOTSYS}/bin/thisroot.sh\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/setup.sh "export NUWRO_ROOT=${PROJECT_SOURCE_DIR}\n")
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/setup.sh "export PATH=${CMAKE_INSTALL_PREFIX}/bin:\${PATH}\n")
if(DEFINED ENV{PYTHIA6})
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/setup.sh "export PYTHIA6=$ENV{PYTHIA6}\n")
  file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/setup.sh "export LD_LIBRARY_PATH=\${PYTHIA6}:\${LD_LIBRARY_PATH}\n")
endif()
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/setup.sh "export LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:\${LD_LIBRARY_PATH}")
