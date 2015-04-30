//===------------------------------------------------------------*- C++ -*-===//
#include "CCAgent/SixCCS.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

SixCCS::SixCCS() {
	reset();
}

void SixCCS::getMoves(std::set<Move> &moves) const {
	moves.clear();
	for (unsigned i : p1Loc)
	{
		getMovesSingleStep(moves, i);
		getMovesJumps(moves, i, i);
	}	
}

bool SixCCS::applyMove(Move m) {
	// Apply the move
	std::swap(board[m.from], board[m.to]);
	
	for (size_t i = 0; i < 6; i++)
		if (p1Loc[i] == m.from){
			p1Loc[i] = m.to;
			break;
		}

	return true;
}

bool SixCCS::undoMove(Move m) {
	// Apply the move
	std::swap(board[m.from], board[m.to]);
	for (size_t i = 0; i < 6; i++)
		if (p1Loc[i] == m.to){
			p1Loc[i] = m.from;
			break;
		}

	return true;
}

bool SixCCS::reset() {
	board = {{1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

	size_t k = 0;

	for (int i = 0; i < 81; i++){
		if (board[i] == 1){
			p1Loc[k] = i;
			k = k + 1;
		} 
	}

	return true;
}

bool SixCCS::reset(std::vector<unsigned> values) {
	board.fill(0);
	p1Loc.fill(0);

	unsigned count = 0;

	for(auto i : values){
		board[i] = 1;
		p1Loc[count] = i;
		count++;
	}

	return true;
}

void SixCCS::getMovesSingleStep(std::set<Move> &moves, unsigned from) const {
	unsigned row = from / 9;
	unsigned col = from % 9;
	
	// Up Left
	if (col > 0 && board[from - 1] == 0)
		moves.insert({from, from - 1});
	
	// Up Right
	if (row > 0 && board[from - 9] == 0)
		moves.insert({from, from - 9});
	
	// Left
	if (col > 0 && row < 8 && board[from + 8] == 0)
		moves.insert({from, from + 8});
	
	// Right
	if (col < 8 && row > 0 && board[from - 8] == 0)
		moves.insert({from, from - 8});
	
	// Down Left
	if (row < 8 && board[from + 9] == 0)
		moves.insert({from, from + 9});
	
	// Down Right
	if (col < 8 && board[from + 1] == 0)
		moves.insert({from, from + 1});
}

void SixCCS::getMovesJumps(std::set<Move> &moves, unsigned from,
										 unsigned current) const
{
	
	// If from != current we have a valid move to get here
	if (from != current)
	{
		auto isDupe = !moves.insert({from, current}).second;
		if (isDupe)
			return;
	}
	
	// Mark the current state as visited
	int originalCurrent = board[current];
	board[current] = -1;
	unsigned row = current / 9;
	unsigned col = current % 9;
	
	// Up Left
	if (col > 1 && board[current - 2] == 0)
	{
		int jumped = board[current - 1];
		if (jumped == 1 || jumped == 2)
			getMovesJumps(moves, from, current - 2);
	}
	
	// Up Right
	if (row > 1 && board[current - 18] == 0)
	{
		int jumped = board[current - 9];
		if (jumped == 1 || jumped == 2)
			getMovesJumps(moves, from, current - 18);
	}
	
	// Left
	if (col > 1 && row < 7 && board[current + 16] == 0)
	{
		int jumped = board[current + 8];
		if (jumped == 1 || jumped == 2)
			getMovesJumps(moves, from, current + 16);
	}
	
	// Right
	if (col < 7 && row > 1 && board[current - 16] == 0)
	{
		int jumped = board[current - 8];
		if (jumped == 1 || jumped == 2)
			getMovesJumps(moves, from, current - 16);
	}
	
	// Down Left
	if (row < 7 && board[current + 18] == 0)
	{
		int jumped = board[current + 9];
		if (jumped == 1 || jumped == 2)
			getMovesJumps(moves, from, current + 18);
	}
	
	// Down Right
	if (col < 7 && board[current + 2] == 0)
	{
		int jumped = board[current + 1];
		if (jumped == 1 || jumped == 2)
			getMovesJumps(moves, from, current + 2);
	}
	
	// Restore the current state
	board[current] = originalCurrent;
}

unsigned SixCCS::get(unsigned x) const{
	return board[x];
}
