########################
# Libraries
########################

add_custom_command(OUTPUT params_all.h
  COMMAND echo '\#define PARAMS_ALL\(\)\\' > params_all.h
  COMMAND sed -f ${CMAKE_CURRENT_SOURCE_DIR}/params.sed ${CMAKE_CURRENT_SOURCE_DIR}/params.xml >> params_all.h
  COMMAND echo '' >> params_all.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/params.xml ${CMAKE_CURRENT_SOURCE_DIR}/params.sed)
add_custom_target(params_header DEPENDS params_all.h)

set(SRC_SUBDIRS
  dis
  sf
  reweight
  #rpa -- Not included by default
  )

if(NUWRO_BUILD_VALID)
  set(SRC_SUBDIRS ${SRC_SUBDIRS};vali)
endif()

set(EVENT_LIBS event)
if(NOT NUWRO_SINGLE_LIB)
  set(EVENT_LIBS dis sf ${EVENT_LIBS})
endif()

set(AEVENT_LIBS ${EVENT_LIBS} PARENT_SCOPE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

foreach (dir ${SRC_SUBDIRS})
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${dir})
  else()
    message(FATAL_ERROR "Directory ${CMAKE_CURRENT_SOURCE_DIR}/${dir} doesn't exist and it was expected to.")
  endif()
endforeach()

get_directory_property(incdirs INCLUDE_DIRECTORIES)
message(STATUS "Include Paths: ${incdirs}")

set(AINCDIRS ${incdirs} PARENT_SCOPE)

set(EVENT_SOURCES)
set(HEADERS_TO_INSTALL
  params.h
  vec.h
  dirs.h
  ${CMAKE_CURRENT_BINARY_DIR}/params_all.h
  particle.h
  generatormt.h
  jednostki.h
  pdg.h
  vect.h
  vecrand.h
  event1.h
  )

foreach (dir ${SRC_SUBDIRS})
  message(STATUS "Adding subdirectory src/${dir}")
  add_subdirectory(${dir})
endforeach()

set(EVENT_SOURCES ${EVENT_SOURCES}
  event1.cc
  ${CMAKE_CURRENT_BINARY_DIR}/event1Dict.cxx #The ROOT dictionary gen function appends .cxx
  generatormt.cc
  particle.cc
  pauli.cc
  cohevent2.cc
  cohdynamics2.cc
  qelevent1.cc
  mecdynamics.cc
  mecevent.cc
  mecevent_tem.cc
  mecevent_Nieves.cc
  mecdynamics2.cc
  mecevent2.cc
  qel_sigma.cc
  kinsolver.cc
  kinematics.cc
  pdg.cc
  target_mixer.cc
  nucleus.cc
  sfevent.cc
  ff.cc
  dirs.cc
  rpa_2013.cc
  nucleus_data.cc
  isotopes.cc
  elements.cc
  beam.cc
  nd280stats.cc
  beamHist.cc
  coh.cc
  fsi.cc
  pitab.cc
  scatter.cc
  kaskada7.cc
  Interaction.cc
  RooTrackerEvent.cc
  nuwro.cc)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(event STATIC ${EVENT_SOURCES})
else()
  add_library(event SHARED ${EVENT_SOURCES})
  set_target_properties(event PROPERTIES VERSION
    "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
endif()

add_dependencies(event params_header)

GenROOTDictionary(${CMAKE_CURRENT_BINARY_DIR}/event1Dict event1.h
  event1LinkDef.h)

install(TARGETS event DESTINATION lib)

########################
# Executables
########################

message(STATUS "Building libevent with: ${EVENT_SOURCES}")

add_executable(nuwro main.cc)
target_link_libraries(nuwro event)
if(NOT NUWRO_SINGLE_LIB)
  target_link_libraries(nuwro sf)
  target_link_libraries(nuwro dis)
endif()

set_target_properties(nuwro PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})
target_link_libraries(nuwro ${ROOTLIBS})

add_executable(nuwro2rootracker nuwro2rootracker.cc)
target_link_libraries(nuwro2rootracker event)
target_link_libraries(nuwro2rootracker ${ROOTLIBS})
set_target_properties(nuwro2rootracker PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

add_executable(dumpParams dumpParams.cc)
target_link_libraries(dumpParams event)
target_link_libraries(dumpParams ${ROOTLIBS})
set_target_properties(dumpParams PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})


install(TARGETS
  nuwro
  nuwro2rootracker
  dumpParams
  DESTINATION bin)

install(FILES ${HEADERS_TO_INSTALL} DESTINATION include/nuwro)

if(ISROOT6)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/event1Dict_rdict.pcm DESTINATION lib)
endif()
