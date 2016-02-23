set(VALI_SOURCES SimpleAnalysisFormat.cc)

set(VALI_INCLUDE_PATHS ${VALI_INCLUDE_PATHS} ${CMAKE_CURRENT_SOURCE_DIR})

if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
  add_library(vali STATIC ${VALI_SOURCES})
else()
  add_library(vali SHARED ${VALI_SOURCES})
  set_target_properties(vali PROPERTIES VERSION
    "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
endif()
target_include_directories(vali PUBLIC ${VALI_INCLUDE_PATHS})

install(TARGETS vali DESTINATION lib)

add_executable(DumpEvInfo DumpEvInfo.cc SimpleAnalysisFormat.cc)
target_include_directories(DumpEvInfo PUBLIC ${VALI_INCLUDE_PATHS})

set_target_properties(DumpEvInfo PROPERTIES COMPILE_FLAGS " -DSAF_STRICT")

target_link_libraries(DumpEvInfo event)
set_target_properties(DumpEvInfo PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

install(TARGETS DumpEvInfo DESTINATION bin)
