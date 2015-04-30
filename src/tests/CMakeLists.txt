include_directories(${PROJECT_SOURCE_DIR}/include ${GFlags_INCLUDE_DIRS})
include_directories(${gtest_SOURCE_DIR}/include)

set(test_sources
	test_all.cpp
	test_zobrist.cpp
	test_state.cpp
	test_agent.cpp
	test_endgame.cpp
)

set_source_files_properties(${test_sources} PROPERTIES COMPILE_FLAGS "-O3 -w")
if(Boost_FOUND)
  add_definitions(-BOOST_MP_DEF)
endif ()

add_executable(runTests ${test_sources})
target_link_libraries(runTests 
	gtest_main
	pthread
	gflags
	CCAgent
)

add_test(runTests runTests)
