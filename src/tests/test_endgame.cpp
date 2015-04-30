#include <iostream>


#include "CCAgent/EndgameDatabaseElement.h"
#include "CCAgent/EndgameDatabase.h"

#include "gflags/gflags.h"
#include "gtest/gtest.h"

class EndgameTest : public ::testing::Test {
 protected:
  virtual void SetUp() {

  }
  virtual void TearDown() {

  }
};

TEST_F(EndgameTest, SixCCS_getMoves){
	SixCCS state;
	std::set<Move> moves;
	state.getMoves(moves);
	EXPECT_EQ(10, moves.size());
}

TEST_F(EndgameTest, SixCCS_apply_undo){
	SixCCS state;
	state.applyMove({1,3});
	state.undoMove({1,3});
	std::set<Move> moves;
	state.getMoves(moves);
	EXPECT_EQ(10, moves.size());
}


TEST_F(EndgameTest, Init){
	EndgameDatabase db("none", 324540216, 6);
}