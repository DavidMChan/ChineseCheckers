//Header file for the implementation of UCB

#ifndef UCB_AGENT_H_INCLUDED
#define UCB_AGENT_H_INCLUDED

#include <cstdint>
#include <thread>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <cassert>

#include "gflags/gflags.h"
#include "CCAgent/CCBitboard.h"
#include "CCAgent/TDRegression.h"

DECLARE_bool(v);

class UCBPlayer {
public:
	UCBPlayer(CCBoard &s) : total_plays(0), state(s), moves(100), move_cache(100) { 
		state.getMovesForward(moves);
		for (auto m : moves) {
			plays[m] = 0;
			value[m] = 0;
		}
	}
	
	uint16_t GetMove(double timelimit); //Get the next move that the player should make 
										//given the proposed board

	uint16_t GetMove(double timelimit, TDEval &eval, unsigned depth); //Get the next move that the player should make 
																	  //given the proposed board


	double EvaluateStateUCB1(uint16_t move); //Give the UCB1 value of a particular move
	void PlayOut(uint16_t move); // Play out the current state from the given move
	void PlayOut(uint16_t move, TDEval &eval, unsigned max_depth); //Play the game out with an eval function

private:
	std::unordered_map<uint16_t, uint64_t> plays; //Hash map that keeps track of the number of times we have played a move
	std::unordered_map<uint16_t, double> value; //Hash map that keeps track of how many times that move has won

	uint64_t total_plays; //The total number of times we have played a game
	CCBoard &state; //The state from which we play (A copy is made every time)
	
	std::vector<uint16_t> moves;
	std::vector<uint16_t> move_cache;
};


#endif