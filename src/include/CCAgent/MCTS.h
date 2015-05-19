//Header file for monte carlo tree search
//
#ifndef MCTS_H_INCLUDED
#define MCTS_H_INCLUDED

//Library includes
#include <cstdint>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <future>
#include <iomanip>
#include <iostream>
#include <map>
#include <unordered_map>
#include <memory>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>

//Game specific includes
#include "CCAgent/CCBitboard.h"
#include "CCAgent/TDRegression.h"
#include "gflags/gflags.h"

DECLARE_bool(v);
DECLARE_double(MCTSExplore);
DECLARE_int32(MCTSEpsilon);
DECLARE_int32(MCTSVisitLimit);
DECLARE_int32(MCTSPlayoutDepth);

class MCTSNode {
	public:

		//Constructor and destructor
		MCTSNode(const CCBoard &state);
		~MCTSNode();

		bool hasUnexploredMoves() const; //Returns if the node has moves that have not been explored yet
		uint16_t getUnexploredMove(std::mt19937 &gen) const; //Get a move that needs to be explored

		bool hasChildren() const;
		MCTSNode* selectChild(std::mt19937 &gen, bool max) const;
		MCTSNode* addChild(uint16_t move, const CCBoard &state);
		void updateValue(double value);


		const uint16_t move;
		MCTSNode* const parent;
		const unsigned player;

		double node_value;
		double visits;

		std::vector<uint16_t> moves;
		std::vector<MCTSNode*> children;

	private:
		MCTSNode (const CCBoard& state, uint16_t move, MCTSNode* parent);

		MCTSNode(const MCTSNode&);
		MCTSNode& operator = (const MCTSNode&);

		double UCTscore;
};


namespace MCTS {
	uint16_t getMove(CCBoard &state, double time_limit, TDEval &eval, unsigned num_threads); //Get a move for a given state
	double playOut(CCBoard &state, TDEval &eval, int player, std::mt19937 &gen); //Play out the board
	void compute_mcts_tree(const CCBoard root_state, bool &time, TDEval eval, MCTSNode* n_out); //Build a tree for the given state
	static std::mutex output_lock;
}

#endif