add_subdirectory(reweight)

set(VALI_SOURCES
  SimpleAnalysisFormat.cc
  ValiPlotter.cc)

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  add_library(vali STATIC ${VALI_SOURCES})
else()
  add_library(vali SHARED ${VALI_SOURCES})
  set_target_properties(vali PROPERTIES VERSION
    "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
endif()

install(TARGETS vali DESTINATION lib)

#Want the validation converter to be strict. Build another SAF TU with -DSAF_STRICT.
add_executable(SAF_Analysis SAF_Analysis.cc SimpleAnalysisFormat.cc)

set_target_properties(SAF_Analysis PROPERTIES COMPILE_FLAGS " -DSAF_STRICT")

target_link_libraries(SAF_Analysis event)
set_target_properties(SAF_Analysis PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

install(TARGETS SAF_Analysis DESTINATION bin)
