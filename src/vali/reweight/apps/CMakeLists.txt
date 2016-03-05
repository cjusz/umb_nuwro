add_executable(SimpleReweighter SimpleReweighter.cc)

target_link_libraries(SimpleReweighter reweight;${EVENT_LIBS})
set_target_properties(SimpleReweighter PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

set(FOMP "-fopenmp")

add_executable(SimpleReweighterProfiler SimpleReweighterProfiler.cc)

target_link_libraries(SimpleReweighterProfiler reweight;${EVENT_LIBS})
set_target_properties(SimpleReweighterProfiler PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS} ${FOMP}" )
set_target_properties(SimpleReweighterProfiler PROPERTIES COMPILE_FLAGS "${FOMP} ")

add_executable(SimpleFitter SimpleFitter.cc)

target_link_libraries(SimpleFitter reweight;${EVENT_LIBS};vali)
set_target_properties(SimpleFitter PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS} ${FOMP} -lMinuit2")
set_target_properties(SimpleFitter PROPERTIES COMPILE_FLAGS "${FOMP} ")

add_executable(SimpleFitterProfiler SimpleFitterProfiler.cc)

target_link_libraries(SimpleFitterProfiler reweight;${EVENT_LIBS};vali)
set_target_properties(SimpleFitterProfiler PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS} ${FOMP} ")
set_target_properties(SimpleFitterProfiler PROPERTIES COMPILE_FLAGS "${FOMP} ")

add_executable(DumpDialTweaks DumpDialTweaks.cc)

target_link_libraries(DumpDialTweaks reweight;${EVENT_LIBS})
set_target_properties(DumpDialTweaks PROPERTIES LINK_FLAGS "${CMAKE_LINK_FLAGS}")


install(TARGETS
  SimpleReweighter
  SimpleReweighterProfiler
  SimpleFitter
  SimpleFitterProfiler
  DumpDialTweaks
  DESTINATION bin)
