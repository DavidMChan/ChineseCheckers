//===------------------------------------------------------------*- C++ -*-===//
///
/// \file
/// \brief Defines the Chinese Checkers game state
///
/// Note: Many aspects of this State are inefficient to make the code clearer
///
//===----------------------------------------------------------------------===//
#ifndef CHINESECHECKERS_STATE_H_INCLUDED
#define CHINESECHECKERS_STATE_H_INCLUDED

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

class ChineseCheckersState {
public:
	// Initialize with the starting state for a 2 player game
	ChineseCheckersState();
	
	// dtor - default since we have nothing to clean up
	~ChineseCheckersState() = default;
	
	// Defaults for the rule of 5
	// copy ctor
	ChineseCheckersState(const ChineseCheckersState&) = default;
	// move ctor
	ChineseCheckersState(ChineseCheckersState&&) = default;
	// copy assignment
	ChineseCheckersState &operator=(const ChineseCheckersState&) = default;
	// move assignment
	ChineseCheckersState &operator=(ChineseCheckersState&&) = default;
	
	// Put all valid moves into the vector of moves passed in by reference
	void getMoves(std::set<Move> &moves) const;
	
	// Put all valid moves into the vector of moves passed in by reference
	void getMovesForward(std::set<Move> &moves) const;

	// Put all valid moves into the vector of moves passed in by reference
	void getMovesNotBackwards(std::set<Move> &moves) const;

	// Put all valid moves into the vector of moves passed in by reference
	Move getRandomMove() const;
	
	// Apply the move m, returning true if m is a valid move, false if not
	bool applyMove(Move m);
	
	// Undo the move m, returning true if m is a move that can be undone, false if not
	bool undoMove(Move m);
	
	// Returns true iff the move m is valid
	bool isValidMove(const Move &m) const;
	
	// Returns true iff the game is over
	bool gameOver() const;
	
	// Return the player who won, assuming the game is over
	int winner() const;
	
	// Reset the board to the initial state
	bool reset();

	// Get the zobrist hash for the board
	uint64_t getHash();

  	//Swap who's turn it is
  	int swapPlayer();

  	//Get the player who's move it is
  	int getPlayer();
	
	// Loads the state stored in the string, returning true if it is a valid state, false if not
	bool loadState(const std::string &newState);
	
	// Dump out the current state, usable with loadState
	std::string dumpState() const;

	//Get an element from the board
	unsigned get(unsigned x) const;
	
	
	// Translates a sequence of tokens from the move format used to the local move type
	Move translateToLocal(const std::vector<std::string> &tokens) const;
	
	unsigned currentPlayer;

	mutable std::array<unsigned, 81> board;
	mutable std::array<unsigned,10> p1Loc;
	mutable std::array<unsigned,10> p2Loc;
	
	void PrintASCII();
	
	bool LoadZobristTableFromFile(std::string);
	bool SaveZobristTableToFile(std::string);
 	std::string DumpZobrist();


private:

	//Zobrist Hashing
	uint64_t zhash;
	uint64_t reHash();
	uint64_t applyHashMove(uint64_t hash, Move m, unsigned player);
	void initZobrist();
	std::array<std::array<uint64_t, 3>,81> zobristMoveTable;



	void getMovesSingleStep(std::set<Move> &moves, unsigned from) const;
	void getMovesJumps(std::set<Move> &moves, unsigned from, unsigned current) const;
	
	void swapTurn();
	
	bool player1Wins() const;
	bool player2Wins() const;

};

#endif
