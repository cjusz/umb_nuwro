add_executable(test_reweighter test_reweighter.cc)

target_link_libraries(test_reweighter reweight;${EVENT_LIBS})
set_target_properties(test_reweighter PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})


add_executable(test_SimpleReweighter test_SimpleReweighter.cc)

target_link_libraries(test_SimpleReweighter reweight;${EVENT_LIBS})
set_target_properties(test_SimpleReweighter PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS} -fopenmp")
set_target_properties(test_SimpleReweighter PROPERTIES COMPILE_FLAGS -fopenmp)


if(ISROOT6) #Only have ROOT6 implementation at the moment
  add_executable(test_SimpleFitter test_SimpleFitter.cc)

  target_link_libraries(test_SimpleFitter reweight;${EVENT_LIBS})
  set_target_properties(test_SimpleFitter PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS} -fopenmp -lMinuit2")
  set_target_properties(test_SimpleFitter PROPERTIES COMPILE_FLAGS "-fopenmp -DROOT6 ")

  install (TARGETS test_SimpleFitter DESTINATION bin)
else()
  add_executable(test_SimpleFitter test_SimpleFitter.cc)

  target_link_libraries(test_SimpleFitter reweight;${EVENT_LIBS})
  set(FOMP "-fopenmp")
  set_target_properties(test_SimpleFitter PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS} ${FOMP} -lMinuit2")
  set_target_properties(test_SimpleFitter PROPERTIES COMPILE_FLAGS "${FOMP} ")

  install (TARGETS test_SimpleFitter DESTINATION bin)
endif()

install(TARGETS
  test_reweighter
  test_SimpleReweighter
  DESTINATION bin)
