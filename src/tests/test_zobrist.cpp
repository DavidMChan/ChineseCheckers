#include <limits.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <iterator>
#include <set>
#include <string>

#include "CCAgent/Move.h"
#include "CCAgent/CCBitboard.h"
#include "gflags/gflags.h"
#include "gtest/gtest.h"

class StateZobristTest : public ::testing::Test {
 protected:
  virtual void SetUp() {

  }
  virtual void TearDown() {

  }
};

TEST_F(StateZobristTest, ApplyMove){
	CCBoard s;
	uint64_t hash = s.getHash();
	std::vector<uint16_t> moves;
	s.getMoves(moves);
	for (auto m : moves){
		s.applyMove(m);
		s.undoMove(m);
		EXPECT_EQ(hash, s.getHash());
	}
}