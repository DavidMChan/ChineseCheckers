//===------------------------------------------------------------*- C++ -*-===//
#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED

#include <cstdlib>
#include <iostream>
#include <iterator>
#include <sstream>
#include <istream>
#include <string>
#include <set>
#include <cmath>
#include <vector>
#include <ctime>

#include "CCAgent/CCBitboard.h"
#include "CCAgent/TDRegression.h"
#include "CCAgent/AlphaBeta.h"
#include "CCAgent/OpeningBook.h"
#include "gflags/gflags.h"

DECLARE_bool(v);
DECLARE_double(time_limit);
DECLARE_bool(ob);
DECLARE_string(weights);

class Agent {
public:
  Agent(std::string n, std::istream &in = std::cin, std::ostream &out = std::cout) : name(n), input(in), output(out) {
    my_player = 1u;
    current_player = 1u;
    my_book = new OpeningBook(my_player + 1, 0);
    
    if (FLAGS_weights == "none"){
      trainPlayer(); 
    } else {
      t_eval.SetWeightsFromFile(FLAGS_weights);
    }
  }
  bool ProcessCommand(std::string response, bool &suggestBreak);
  bool ProcessCommand(std::string response, bool &suggestBreak, std::stringstream &out);
  void playGame();

  int GetMyPlayer();
  ~Agent(){
    delete my_book;
  }

private:
  uint16_t nextMove();
  void printAndRecvEcho(const std::string &msg) const;
  std::string readMsg() const;
  std::vector<std::string> tokenizeMsg(const std::string &msg) const;
  bool waitForStart();
  void switchCurrentPlayer();
  void trainPlayer();

  bool isValidStartGameMessage(const std::vector<std::string> &tokens) const;
  bool isValidMoveMessage(const std::vector<std::string> &tokens) const;


  CCBoard state;
  unsigned my_player;
  unsigned current_player;
  std::string name;
  std::string opp_name;
  std::istream &input;
  std::ostream &output;
  OpeningBook* my_book;

  TDEval t_eval;

};


#endif
