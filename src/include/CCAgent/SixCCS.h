//===------------------------------------------------------------*- C++ -*-===//
///
/// \file
/// \brief Defines the Chinese Checkers game state
///
/// Note: Many aspects of this State are inefficient to make the code clearer
///
//===----------------------------------------------------------------------===//
#ifndef SIXCCS_STATE_H_INCLUDED
#define SIXCCS_STATE_H_INCLUDED

#include "CCAgent/Move.h"

#include <array>
#include <ostream>
#include <set>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <ios>
#include <fstream>
#include <sstream>

class SixCCS {
public:
	// Initialize with the starting state for a 2 player game
	SixCCS();

	// dtor - default since we have nothing to clean up
	~SixCCS() = default;
	
	// Defaults for the rule of 5
	// copy ctor
	SixCCS(const SixCCS&) = default;
	// move ctor
	SixCCS(SixCCS&&) = default;
	// copy assignment
	SixCCS &operator=(const SixCCS&) = default;
	// move assignment
	SixCCS &operator=(SixCCS&&) = default;
	
	// Put all valid moves into the vector of moves passed in by reference
	void getMoves(std::set<Move> &moves) const;
	
	// Apply the move m, returning true if m is a valid move, false if not
	bool applyMove(Move m);
	
	// Undo the move m, returning true if m is a move that can be undone, false if not
	bool undoMove(Move m);
	
	// Reset the board to the initial state
	bool reset();

	// Reset the board to the initial state
	bool reset(std::vector<unsigned> values);

	//Get an element from the board
	unsigned get(unsigned x) const;
	
	
	mutable std::array<unsigned, 81> board;
	mutable std::array<unsigned,6> p1Loc;
	
private:
	void getMovesSingleStep(std::set<Move> &moves, unsigned from) const;
	void getMovesJumps(std::set<Move> &moves, unsigned from, unsigned current) const;
};

#endif
