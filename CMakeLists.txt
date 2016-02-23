cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project (NUWRO)

set (NUWRO_VERSION_MAJOR 11)
set (NUWRO_VERSION_MINOR 18) #The q+1'th letter of the alphabet
set (NUWRO_VERSION_REVISION 0)

set (VERBOSE TRUE)

set (CMAKE_SKIP_BUILD_RPATH TRUE)

set (CMAKE_BUILD_TYPE DEBUG)

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
  message (FATAL_ERROR "root-config could not be found, is ROOT built in $ROOTSYS = $ENV{ROOTSYS}")
else()
  execute_process (COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process (COMMAND root-config --libs OUTPUT_VARIABLE ROOT_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
  message ( STATUS "root-config --cflags: " ${ROOT_CXX_FLAGS} )
  message ( STATUS "root-config --libs: " ${ROOT_LD_FLAGS} )
endif()

#Function for building dictionaries
function(gen_root_dicts TAR Header)
  get_filename_component(HeaderName ${Header} NAME_WE)
  string(REPLACE " " ";" LIST_CMAKE_CXX_FLAGS ${CURRENT_CMAKE_CXX_FLAGS})

  SET(LIST_PROJECT_INCLUDE_PATHS "")
   FOREACH(incp ${PROJECT_INCLUDE_PATHS})
      LIST(APPEND LIST_PROJECT_INCLUDE_PATHS "-I${incp}")
   ENDFOREACH()

  #Learn how to generate the Dict.cxx and Dict.hxx
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${HeaderName}Dict.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/${HeaderName}Dict.h"
    COMMAND rootcint
    ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/${HeaderName}Dict.cc -c
    -p ${LIST_PROJECT_INCLUDE_PATHS} ${LIST_CMAKE_CXX_FLAGS}
    ${CMAKE_CURRENT_SOURCE_DIR}/${HeaderName}.h
    ${CMAKE_CURRENT_SOURCE_DIR}/${HeaderName}LinkDef.h
    )

  add_custom_target(${HeaderName}Dict
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${HeaderName}Dict.cc"
            "${CMAKE_CURRENT_BINARY_DIR}/${HeaderName}Dict.h" )
  add_dependencies(${TAR} ${HeaderName}Dict)

  if(VERBOSE)
    message(STATUS "[ROOT DICT GEN]: Target: ${TAR} depends on generated ${CMAKE_CURRENT_BINARY_DIR}/${HeaderName}Dict.cc, ${CMAKE_CURRENT_BINARY_DIR}/${HeaderName}Dict.h")
  endif()
endfunction()

################################## COMPILER ####################################

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 ${ROOT_CXX_FLAGS} -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -Wno-unused-but-set-variable -Wno-reorder")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${ROOT_CXX_FLAGS} -fPIC")

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS_DEBUG} ${ROOT_LD_FLAGS} -lEG -lEGPythia6 -lGeom -lMinuit ${PYTHIA6LIBLOC} -lPythia6")
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RELEASE")
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS_RELEASE} ${ROOT_LD_FLAGS} -lEG -lEGPythia6 -lGeom -lMinuit ${PYTHIA6LIBLOC} -lPythia6")
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
else()
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS_RELEASE} ${ROOT_LD_FLAGS} -lEG -lEGPythia6 -lGeom -lMinuit  ${PYTHIA6LIBLOC} -lPythia6-Wl,--no-as-needed")
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
