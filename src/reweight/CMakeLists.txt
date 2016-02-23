set(REWEIGHT_SOURCES
  NuwroReWeight.cc
  NuwroReWeight_MaCCQE.cc
  NuwroReWeight_MaRES_CA5.cc
  NuwroReWeight_MECNorm.cc
  NuwroSyst.cc
  NuwroSystSet.cc
  NuwroSystUncertainty.cc
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  add_library(reweight STATIC ${REWEIGHT_SOURCES})
else()
  add_library(reweight SHARED ${REWEIGHT_SOURCES})
  set_target_properties(reweight PROPERTIES VERSION
    "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
endif()

target_include_directories(reweight PUBLIC ${PROJECT_INCLUDE_PATHS})

install(TARGETS reweight DESTINATION lib)
