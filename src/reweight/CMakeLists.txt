set(REWEIGHT_SOURCES
  NuwroReWeight.cc
  NuwroReWeight_QEL.cc
  NuwroReWeight_MaRES_CA5.cc
  NuwroReWeight_FlagNorm.cc
  NuwroReWeight_BBBA07.cc
  NuwroSyst.cc
  NuwroSystSet.cc
  NuwroSystUncertainty.cc
  PythiaQuiet.f
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  add_library(reweight STATIC ${REWEIGHT_SOURCES})
  set(FOMP "-fopenmp")
  set_target_properties(reweight PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS} ${FOMP}" )
  set_target_properties(reweight PROPERTIES COMPILE_FLAGS "${FOMP} ")
else()
  add_library(reweight SHARED ${REWEIGHT_SOURCES})
  set_target_properties(reweight PROPERTIES VERSION
    "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
endif()

install(TARGETS reweight DESTINATION lib)
