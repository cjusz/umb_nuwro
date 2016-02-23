########################
# Libraries
########################

set(SRC_SUBDIRS
  dis
  sf
  reweight
  #rpa -- Not included by default
  )

set(PROJECT_INCLUDE_PATHS ${PROJECT_INCLUDE_PATHS} ${CMAKE_CURRENT_SOURCE_DIR})

foreach (dir ${SRC_SUBDIRS})
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
    set(PROJECT_INCLUDE_PATHS ${PROJECT_INCLUDE_PATHS} ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
  else()
    message(FATAL_ERROR "Directory ${CMAKE_CURRENT_SOURCE_DIR}/${dir} doesn't exist and it was expected to.")
  endif()
endforeach()

set(SUBDIRS vali ${SRC_SUBDIRS}) # don't add vali headers to main project
foreach (dir ${SUBDIRS})
  add_subdirectory(${dir})
endforeach()


message(STATUS "FULL INCLUDE PATH: ${PROJECT_INCLUDE_PATHS}")

set(EVENT_SOURCES
  event1.cc
  ${CMAKE_CURRENT_BINARY_DIR}/event1Dict.cc #Generated
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

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  add_library(event STATIC ${EVENT_SOURCES})
else()
  add_library(event SHARED ${EVENT_SOURCES})
  set_target_properties(event PROPERTIES VERSION
    "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
endif()

target_include_directories(event PUBLIC ${PROJECT_INCLUDE_PATHS})

gen_root_dicts(event event1.h)

install(TARGETS event DESTINATION lib)

########################
# Executables
########################

add_executable(nuwro main.cc)
target_link_libraries(nuwro event)
target_link_libraries(nuwro sf)
target_link_libraries(nuwro dis)
target_link_libraries(nuwro reweight) # for debugging
set_target_properties(nuwro PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

add_executable(nuwro2rootracker nuwro2rootracker.cc)
target_link_libraries(nuwro2rootracker event)
set_target_properties(nuwro2rootracker PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

add_executable(test_reweight_CCQE_rootracker test_reweight_CCQE_rootracker.cc)
target_link_libraries(test_reweight_CCQE_rootracker reweight)
target_link_libraries(test_reweight_CCQE_rootracker event)
target_link_libraries(test_reweight_CCQE_rootracker dis)
set_target_properties(test_reweight_CCQE_rootracker PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

add_executable(test_reweight_MaRES_rootracker test_reweight_MaRES_rootracker.cc)
target_link_libraries(test_reweight_MaRES_rootracker reweight)
target_link_libraries(test_reweight_MaRES_rootracker event)
target_link_libraries(test_reweight_MaRES_rootracker dis)
set_target_properties(test_reweight_MaRES_rootracker PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

add_executable(test_reweight_MaRES test_reweight_MaRES.cc)
target_link_libraries(test_reweight_MaRES reweight)
target_link_libraries(test_reweight_MaRES event)
target_link_libraries(test_reweight_MaRES dis)
set_target_properties(test_reweight_MaRES PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

install(TARGETS nuwro
  nuwro2rootracker
  test_reweight_CCQE_rootracker
  test_reweight_MaRES_rootracker
  test_reweight_MaRES
  DESTINATION bin)
