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
#Will not compile by default.
set(DIS_EXTRA
  dishadr.cc
  disweight.cc
  resevent.cc
  reshadr.cc
  resweight.cc
  )

if(NUWRO_SINGLE_LIB)
  message(STATUS "Adding DIS sources to event lib: ${DIS_SOURCES}")

  set(ABS_DIS_SOURCES "")
  foreach(src ${DIS_SOURCES})
    list(APPEND ABS_DIS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
  endforeach(src)

  set(EVENT_SOURCES ${EVENT_SOURCES} ${ABS_DIS_SOURCES} PARENT_SCOPE)
else(NUWRO_SINGLE_LIB)
  message(STATUS "Building libdis with: ${DIS_SOURCES}")
  if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_library(dis STATIC ${DIS_SOURCES})
  else()
    add_library(dis SHARED ${DIS_SOURCES})
    set_target_properties(dis PROPERTIES VERSION
      "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
  endif()
  add_dependencies(dis params_header)
  install(TARGETS dis DESTINATION lib)
endif(NUWRO_SINGLE_LIB)
