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

class StateTest : public ::testing::Test {
 protected:
  virtual void SetUp() {

  }
  virtual void TearDown() {

  }
};

TEST_F(StateTest, InitialSetup){
	CCBoard s;
	EXPECT_EQ(s.gameOver(), false);
}

TEST_F(StateTest, TimedStateTest){
	CCBoard s;
	std::vector<uint16_t> moves;
	for(int i =0; i < 1000000; i++){
		//s.applyMove(0x0204);
		//s.undoMove(0x0204);
    	s.getMoves(moves);
	}
}

