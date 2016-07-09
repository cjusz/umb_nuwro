set(RPA_SOURCES
  dens_test.cc
  rpa_lib.cc
)

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
  add_library(rpa STATIC ${RPA_SOURCES})
else()
  add_library(rpa SHARED ${RPA_SOURCES})
  set_target_properties(rpa PROPERTIES VERSION
    "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
endif()
add_dependencies(rpa params_header)

target_include_directories(rpa PUBLIC ${PROJECT_INCLUDE_PATHS})

install(TARGETS rpa DESTINATION lib)
