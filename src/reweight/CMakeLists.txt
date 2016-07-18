set(REWEIGHT_SOURCES
  NuwroReWeight.cc
  NuwroReWeight_QEL.cc
  NuwroReWeight_FlagNorm.cc
  NuwroReWeight_SPP.cc
  NuwroSyst.cc
  NuwroSystSet.cc
  NuwroSystUncertainty.cc
  PythiaQuiet.f
)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(reweight STATIC ${REWEIGHT_SOURCES})
else()
  add_library(reweight SHARED ${REWEIGHT_SOURCES})
  set_target_properties(reweight PROPERTIES VERSION
    "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
endif()

add_dependencies(reweight params_header)

set_target_properties(reweight PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS} ${XTRA_CFLAGS}" )
set_target_properties(reweight PROPERTIES COMPILE_FLAGS "${XTRA_LDFLAGS} ")

install(TARGETS reweight DESTINATION lib)
