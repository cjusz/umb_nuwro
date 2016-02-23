set(DIS_SOURCES
  alfa.cc
  charge.cc
  delta.cc
  dis2res.cc
  dis_cc_neutron.cc
  dis_cc_proton.cc
  dis_cr_sec.cc
  disevent.cc
  dis_nc.cc
  fragmentation.cc
  fragmentation_cc.cc
  fragmentation_nc.cc
  grv94_bodek.cc
  LeptonMass.cc
  parameters.cc
  resevent2.cc
  singlepion.cc
  singlepionhadr.cc
)
set(DIS_EXTRA
  dishadr.cc
  disweight.cc
  resevent.cc
  reshadr.cc
  resweight.cc
  )

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  add_library(dis STATIC ${DIS_SOURCES})
else()
  add_library(dis SHARED ${DIS_SOURCES})
  set_target_properties(dis PROPERTIES VERSION
    "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
endif()

target_include_directories(dis PUBLIC ${PROJECT_INCLUDE_PATHS})

install(TARGETS dis DESTINATION lib)
