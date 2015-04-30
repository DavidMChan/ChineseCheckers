include_directories(${PROJECT_SOURCE_DIR}/include)

set(cc_sources
	Agent.cpp
	AlphaBeta.cpp
	Move.cpp
	CCBitboard.cpp
	OpeningBook.cpp
	LinearRegression.cpp
	TDRegression.cpp
	EndgameDatabase.cpp
	SixCCS.cpp
	uint128_t.cpp
	)

set_source_files_properties(${cc_sources} PROPERTIES COMPILE_FLAGS " -O3 -g -Wno-sign-conversion -Wno-reorder -Wno-conversion -Wno-old-style-cast -Wno-suggest-attribute=pure -Wno-unused-parameter -Wno-zero-as-null-pointer-constant")

if(Boost_FOUND)
  add_definitions(-BOOST_MP_DEF)
endif ()

add_library(CCAgent
	${cc_sources}
	)
