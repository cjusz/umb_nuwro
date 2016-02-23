add_executable(test_reweighter test_reweighter.cc)
target_include_directories(test_reweighter PUBLIC ${VALI_INCLUDE_PATHS})

target_link_libraries(test_reweighter event)
target_link_libraries(test_reweighter reweight)
target_link_libraries(test_reweighter dis)
set_target_properties(test_reweighter PROPERTIES LINK_FLAGS ${CMAKE_LINK_FLAGS})

install(TARGETS test_reweighter DESTINATION bin)
