//implementation of alpha beta, for use with transposition tables.
//It should be nice for all of this.


#ifndef ALPHA_BETA_H_INCLUDED
#define ALPHA_BETA_H_INCLUDED

#include <thread>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <atomic>
#include <set>
#include <float.h>
#include <climits>
#include <sstream>
#include <tuple>
#include <unordered_map>

#include "CCAgent/Agent.h"
#include "CCAgent/CCBitboard.h"
#include "CCAgent/TDRegression.h"
#include "gflags/gflags.h"


namespace AB {
	enum Node { EXACT, UPPER, LOWER };
	struct TTEntry{
		uint64_t hash;
		short depth;
		double score;
		bool player;
		Node type;
		double alpha;
		double beta;
	};

	uint16_t AlphaBetaRoot(CCBoard &state, TDEval &eval, float time, int player); //The root node that is necessary
	static std::unordered_map<uint64_t, AB::TTEntry> Transpositions;
	static std::unordered_map<uint16_t, uint64_t> HistoryHeuristicPlayer1;
	static std::unordered_map<uint16_t, uint64_t> HistoryHeuristicPlayer2;
	static bool s_time = false;
	static std::vector<std::vector<uint16_t>> vectorCache(50);
	double NegaScout(CCBoard &state, TDEval &eval, short depth, double alpha, double beta, bool player, bool &time = AB::s_time);
	double Quiesence(CCBoard &state, TDEval &eval, short depth, bool player,  bool &time);
}

#endif
