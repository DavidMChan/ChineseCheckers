#include <limits.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <iterator>
#include <string>

#include "CCAgent/Agent.h"
#include "gflags/gflags.h"
#include "gtest/gtest.h"

class AgentTest : public ::testing::Test {
 protected:
  virtual void SetUp() {

  }
  virtual void TearDown() {

  }
};

TEST_F(AgentTest, InitializeAndIdentify){
	Agent a("name");
	std::stringstream ss;
	bool temp;
	EXPECT_EQ(true, a.ProcessCommand("IDENTIFY", temp, ss));
	EXPECT_EQ("#name name\n", ss.str());
	EXPECT_EQ(false, temp);
}

TEST_F(AgentTest, InitialDumpState){
	Agent a("name");
	std::stringstream ss;
	bool temp;
	EXPECT_EQ(true, a.ProcessCommand("DUMPSTATE", temp, ss));
	EXPECT_EQ("1 1 1 1 1 0 0 0 0 0 1 1 1 0 0 0 0 0 0 1 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 0 2 2 0 0 0 0 0 0 2 2 2 0 0 0 0 0 2 2 2 2\n", ss.str());
	EXPECT_EQ(false, temp);
}

TEST_F(AgentTest, InitialGetPlayer){
	Agent a("name");
	std::stringstream ss;
	bool temp;
	EXPECT_EQ(true, a.ProcessCommand("GETPLAYER", temp, ss));
	EXPECT_EQ("Current player is: 1\n", ss.str());
	EXPECT_EQ(false, temp);
}

TEST_F(AgentTest, SwapPlayer){
	Agent a("name");
	std::stringstream ss;
	bool temp;
	EXPECT_EQ(true, a.ProcessCommand("GETPLAYER", temp, ss));
	EXPECT_EQ("Current player is: 1\n", ss.str());
	EXPECT_EQ(false, temp);
	EXPECT_EQ(true, a.ProcessCommand("SWAPPLAYER", temp, ss));
	EXPECT_EQ("Current player is: 1\nCurrent player is: 2\n", ss.str());
	EXPECT_EQ(false, temp);
}

TEST_F(AgentTest, LoadState){
	Agent a("name");
	std::stringstream ss;
	bool temp;
	EXPECT_EQ(true, a.ProcessCommand("LOADSTATE 2 1 1 0 1 1 0 0 0 0 1 1 1 0 0 0 0 0 0 1 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 0 2 2 0 0 0 0 0 0 2 2 2 0 0 0 0 0 2 2 2 2", temp, ss));
	EXPECT_EQ(temp, false);
	EXPECT_EQ(true, a.ProcessCommand("DUMPSTATE", temp, ss));
	EXPECT_EQ(false, temp);
	EXPECT_EQ("2 1 1 0 1 1 0 0 0 0 1 1 1 0 0 0 0 0 0 1 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 0 2 2 0 0 0 0 0 0 2 2 2 0 0 0 0 0 2 2 2 2\n", ss.str());
}


TEST_F(AgentTest, NextMoveNotNull){
	Agent a("name");
	FLAGS_time_limit = 0.50f;
	std::stringstream ss;
	bool temp;
	EXPECT_EQ(true, a.ProcessCommand("NEXTMOVE", temp, ss));
	EXPECT_NE("", ss.str());
	EXPECT_EQ(false, temp);
}

TEST_F(AgentTest, GetHashNotNull){
	Agent a("name");
	std::stringstream ss;
	bool temp;
	EXPECT_EQ(true, a.ProcessCommand("GETHASH", temp, ss));
	EXPECT_NE("", ss.str());
	EXPECT_EQ(false, temp);
}


TEST_F(AgentTest, ApplyMove){
	Agent a("name");
	std::stringstream ss;
	bool temp;
	EXPECT_EQ(true, a.ProcessCommand("MOVE FROM 2 TO 4", temp, ss));
	EXPECT_EQ(false, temp);
	EXPECT_EQ(true, a.ProcessCommand("DUMPSTATE", temp, ss));
	EXPECT_EQ(false, temp);
	EXPECT_EQ("2 1 1 0 1 1 0 0 0 0 1 1 1 0 0 0 0 0 0 1 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 0 2 2 0 0 0 0 0 0 2 2 2 0 0 0 0 0 2 2 2 2\n", ss.str());
}

TEST_F(AgentTest, UndoMove){
	Agent a("name");
	std::stringstream ss;
	bool temp;
	EXPECT_EQ(true, a.ProcessCommand("MOVE FROM 2 TO 4", temp, ss));
	EXPECT_EQ(false, temp);
	EXPECT_EQ(true, a.ProcessCommand("MOVE FROM 4 TO 2", temp, ss));
	EXPECT_EQ(false, temp);
	EXPECT_EQ(true, a.ProcessCommand("DUMPSTATE", temp, ss));
	EXPECT_EQ("1 1 1 1 1 0 0 0 0 0 1 1 1 0 0 0 0 0 0 1 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 2 0 0 0 0 0 0 0 2 2 0 0 0 0 0 0 2 2 2 0 0 0 0 0 2 2 2 2\n", ss.str());
	EXPECT_EQ(false, temp);
}

TEST_F(AgentTest, Quit){
	Agent a("name");
	std::stringstream ss;
	bool temp;
	EXPECT_EQ(false, a.ProcessCommand("QUIT", temp, ss));
	EXPECT_EQ(false, temp);
}

