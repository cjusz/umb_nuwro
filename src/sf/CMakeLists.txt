set(SF_SOURCES
  CGaussianSF.cc
  CMomDistrib.cc
  CSFOptions.cc
  CSpectralFunc.cc
  FormFactors.cc
  gridfun2d.cc
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  add_library(sf STATIC ${SF_SOURCES})
else()
  add_library(sf SHARED ${SF_SOURCES})
  set_target_properties(sf PROPERTIES VERSION
    "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
endif()

target_include_directories(sf PUBLIC ${PROJECT_INCLUDE_PATHS})

install(TARGETS sf DESTINATION lib)
