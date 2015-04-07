//Chinese Checkers Board header
//Fast implementation of the board
//David Chan - 2015

#ifndef CHINESECHECKERS_BOARD_H_INCLUDED
#define CHINESECHECKERS_BOARD_H_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <algorithm>
#include <set>
#include <array>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include "Move.h"

class ChineseCheckersBoard {
	public: 
		enum HEXSTATE {EMPTY, P1, P2}; //Store the hex-state of some part of the board
		enum Player {PLAYER1, PLAYER2}; //Player enum
		struct BoardHex {
			unsigned num; //The "name" of the hex position
			HEXSTATE state; //The state of the board
			std::array<BoardHex*, 6> adjacent; //Pointers to adjacent board pieces 1 - up left, 2 - up right, 3 - left, 4 - right, 5 - down left, 6 - down right
			bool marked; //Marker for fast traversal
		};

		ChineseCheckersBoard(); //Constructor
		~ChineseCheckersBoard(); //Destructor
		
		void getValidMoves(std::set<Move> &moves); //Get the valid moves for the given player
		
		bool applyMove(Move m); //Apply a move. Returns true if successful
		bool undoMove(Move m); //Undo a move (make the reverse move). Returns true if successful
		bool applyMove_SAFE(Move m); //Safely apply a move (check to see if it is valid first). Returns true if successful
		bool undoMove_SAFE(Move m); //Safely undo a move (check to see if it is valid first). Returns true if successful
		bool isValidMove(Move m); //Returns if the given move is a valid move
		
		bool gameOver() const; //Return if the board is in a game over state
		int winner() const; //Return 1 if player 1, 2 if player 2, or 0 if neither
		bool reset(); //Resets the board to the original state. Returns true if successful. 
		bool player1Wins() const; 
		bool player2Wins() const; 

		std::string dumpState() const; //Dump the state of the board to std out
		bool loadState(std::string); //Load the board state from a string

		//Don't allow assignment
		ChineseCheckersBoard(const ChineseCheckersBoard&) = delete;
  		ChineseCheckersBoard(const ChineseCheckersBoard&&) = delete;
  		ChineseCheckersBoard &operator=(const ChineseCheckersBoard&) = delete;
  		ChineseCheckersBoard &operator=(const ChineseCheckersBoard&&) = delete;

  		Move translateToLocal(const std::vector<std::string> &tokens) const; //Translates a string of tokens to the local move syntax
  		Player swapPlayer(); //Swaps the player and returns the new current player on the board
  		int getPlayer();

	private:
		unsigned whitePieceLocations[10]; //Store the location of each of player 1's pieces 
		unsigned blackPieceLocations[10]; //Store the location of each of player 2's pieces
		std::array<BoardHex, 81> board; //Store the board
		//std::vector<Move> previous_moves; //Store all moves made in vector form
		Player current_player;
};

#endif
