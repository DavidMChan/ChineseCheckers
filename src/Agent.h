//===------------------------------------------------------------*- C++ -*-===//
#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <ctime>

#include "Move.h"
#include "ChineseCheckersBoard.h"

class Agent {
public:
  Agent(std::string);
  void playGame();

private:
  Move nextMove();
  void printAndRecvEcho(const std::string &msg) const;
  std::string readMsg() const;
  std::vector<std::string> tokenizeMsg(const std::string &msg) const;
  void waitForStart();
  void switchCurrentPlayer();

  bool isValidStartGameMessage(const std::vector<std::string> &tokens) const;
  bool isValidMoveMessage(const std::vector<std::string> &tokens) const;


  ChineseCheckersBoard state;
  ChineseCheckersBoard::Player my_player;
  std::string name;
  std::string opp_name;
  std::vector<std::vector<unsigned>> rows;
};


#endif
