// Chinese checkers state with a bit board implementation 
// AI - 
// David Chan - 2015


#ifndef CCBITBOARD_INCLUDE_H
#define CCBITBOARD_INCLUDE_H

//Utility
#include <cstdint>
#include <string>
#include <stdexcept>

//Containers
#include <array>
#include <vector>
#include <queue>
#include <iterator>

//Stream imports
#include <iostream>
#include <ios>
#include <fstream>
#include <ostream>
#include <sstream>

//Add support for the multi-precision libraries that we need for 128 bit ints
#ifdef BOOST_MP_DEF
#include <boost/multiprecision/cpp_int.hpp>
using uint128_t = boost::multiprecision::uint128_t;
#elif __GNUC__
using uint128_t = unsigned __int128;
#elif __clang__
using __uint128_t = uint128_t;
#else
#include "CCAgent/uint128_t.h"
#endif


class CCBoard{
	public:

		/*
		Rule of 5 (And the constructor)
		 */

		CCBoard(); //Constructor for the chinese checkers board
		~CCBoard() = default; //Default destructor because we have no memory to destroy
		CCBoard(const CCBoard&) = default; //Default copy constructor
		CCBoard(CCBoard&&) = default; //Default move constructor
		CCBoard &operator=(const CCBoard&) = default; //Default copy assignment
		CCBoard &operator=(CCBoard&&) = default; //Default move assignment operator


		/*
		Move management - UINT16_T FORM
		 */

		void getMoves(std::vector<uint16_t> &moves) const; //Get all moves
		void getMovesForward(std::vector<uint16_t> &moves) const; //Get forward moves
		void getMovesNotBackwards(std::vector<uint16_t> &moves) const; //Get moves that do not go backwards (lateral ok)
		uint16_t getRandomMove() const; //Get a random move

		bool applyMove(uint16_t m, bool undo = false); //Apply a move in uint16_t form
		bool undoMove(uint16_t m); //Undo a move in uint16_t form

		mutable std::vector<uint16_t> move_cache;


		/*
		State Information
		 */

		bool gameOver() const; //Get if the game is over
		int winner() const; //Get the winner (returns -1, 1, 2)
		unsigned get(unsigned x) const; //Get who owns a particular board location


		/*
		Player Information
		 */


		unsigned swapPlayer(); //Swap the current player
		unsigned getPlayer() const; //Get the current player
		

		/*
		State Management
		 */

		std::string dumpState() const; //Dump the state of the board to std out
		bool loadState(std::string); //Load the board state from a string

		/*
		Zobrist Hashing
		 */
		
		uint64_t getHash() const; //Get the Zobrist hash for the board state
		bool LoadZobristTableFromFile(std::string); //Load the zobrist table from a file
		bool SaveZobristTableToFile(std::string); //Save the zobrist table to a file
		std::string DumpZobrist(); //Dump the zobrist table

		/*
		Content Accessors
		 */

		uint128_t getBoard() const {return board;} //Get the bit-state representation for the board
		uint128_t getBoardP1() const {return boardp1;} //Get the bit-state representation for player 1's pieces
		uint128_t getBoardP2() const {return boardp2;} //Get the bit-state representation for player 2's pieces

		/*
		Utility
		 */

		uint16_t translateToLocal(const std::vector<std::string> &tokens) const; //Translate a set of strings to local
		bool reset(); //Reset the board state


		/*
		Static Methods
		 */
		static std::string moveToString(uint16_t m); //Convert a move to a string
		static std::string printBoard(uint128_t v); //Print a given binary string

		/*
		Move Management
		 */
		
		static inline uint16_t moveify(unsigned from, unsigned to); //Convert a move to a unsigned integer
		static inline void unmovify(uint16_t move, unsigned &from, unsigned &to); //Convert a move to from and to

	private:

		/*
		Zobrist Hashing
		 */
		uint64_t reHash(); //Get the hash for this board (generate it from scratch)
		uint64_t applyHashMove(uint64_t hash, uint16_t move, bool player); //Apply a move with zobrist

		//Static methods for dealing with the Zobrist table
		static void initZobrist(); //Initialize the move table for zobrist hashing
		static std::array<std::array<uint64_t, 3>,81> zobristMoveTable; //Zobrist move table
		static bool hasInitZobrist; //Indicator flag for initialization of the table

		/*
		State Representation
		 */

		uint128_t board; //Back-end board representation
		uint128_t boardp1; //Back-end player 1 representation
		uint128_t boardp2; //Back-end player 2 representation
		bool player; //Back-end player representation
		uint64_t zhash; //Zobrist hash for the board
		
		/*
		Cached Information
		 */

		mutable std::queue<unsigned> q; //Queue for managing jump moves (cached)
		uint128_t gameOverp1Wins; //Easy game over state win
		uint128_t gameOverp2Wins; //Easy game over state win

};

/**
 * [Turn a move into a uint16_t]
 * @param  from [The place where you come from]
 * @param  to   [The place where you go to]
 * @return      [The new move]
 */
 inline uint16_t CCBoard::moveify(unsigned from, unsigned to){
 	return static_cast<uint16_t>((((from & 0xFF) << 8) | (to & 0xFF)));
 }

/**
 * [Take a move and turn it into a from and to]
 * @param move [The move to work with]
 * @param from [Where to store the from]
 * @param to   [Where to store the to]
 */
 inline void CCBoard::unmovify(uint16_t move, unsigned &from, unsigned &to){
 	from = (move & 0xFF00) >> 8;
 	to = move & 0xFF;
 }

#endif
