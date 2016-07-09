set(SF_SOURCES
  CGaussianSF.cc
  CMomDistrib.cc
  CSFOptions.cc
  CSpectralFunc.cc
  FormFactors.cc
  gridfun2d.cc
)


if(NUWRO_SINGLE_LIB)
  message(STATUS "Adding SF sources to event lib: ${SF_SOURCES}")

  set(ABS_SF_SOURCES "")
  foreach(src ${SF_SOURCES})
    list(APPEND ABS_SF_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
  endforeach(src)

  set(EVENT_SOURCES ${EVENT_SOURCES} ${ABS_SF_SOURCES} PARENT_SCOPE)
else(NUWRO_SINGLE_LIB)
  message(STATUS "Building libsf with: ${SF_SOURCES}")
  if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    add_library(sf STATIC ${SF_SOURCES})
  else()
    add_library(sf SHARED ${SF_SOURCES})
    set_target_properties(sf PROPERTIES VERSION
      "${NUWRO_VERSION_MAJOR}.${NUWRO_VERSION_MINOR}.${NUWRO_VERSION_REVISION}")
  endif()
  add_dependencies(sf params_header)
  install(TARGETS sf DESTINATION lib)
endif(NUWRO_SINGLE_LIB)
