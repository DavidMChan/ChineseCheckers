include_directories(${PROJECT_SOURCE_DIR}/include ${GFlags_INCLUDE_DIRS})

if(Boost_FOUND)
  add_definitions(-BOOST_MP_DEF)
endif ()

set(FireflyAgentSources
  main.cpp
  )

add_executable(Agent
  ${FireflyAgentSources})
target_link_libraries(Agent 
	CCAgent
	pthread
	gflags
	)