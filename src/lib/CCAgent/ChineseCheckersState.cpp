//===------------------------------------------------------------*- C++ -*-===//
#include "CCAgent/ChineseCheckersState.h"

#include <algorithm>
#include <array>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

ChineseCheckersState::ChineseCheckersState() {
	reset();
	initZobrist();
}

Move ChineseCheckersState::getRandomMove() const {
	std::set<Move> moves;
	getMoves(moves);

	size_t choice = rand() % moves.size();
	
	auto m = moves.begin();
	std::advance(m, choice);
	return *m;
}


void ChineseCheckersState::getMoves(std::set<Move> &moves) const {
	moves.clear();
	if (currentPlayer == 1){
		for (unsigned i : p1Loc)
		{
			getMovesSingleStep(moves, i);
			getMovesJumps(moves, i, i);
		}	
	} else {
		for (unsigned i : p2Loc)
		{
			getMovesSingleStep(moves, i);
			getMovesJumps(moves, i, i);
		}
	}
}

void ChineseCheckersState::getMovesForward(std::set<Move> &moves) const {
	moves.clear();
	std::set<Move> allMoves;
	if (currentPlayer == 1){
		for (unsigned i : p1Loc)
		{
			getMovesSingleStep(allMoves, i);
			getMovesJumps(allMoves, i, i);
		}	
	} else {
		for (unsigned i : p2Loc)
		{
			getMovesSingleStep(allMoves, i);
			getMovesJumps(allMoves, i, i);
		}
	}
	if (currentPlayer == 1)
	{
		for (const auto &m : allMoves)
		{
			if ((m.from%9)+(m.from/9) < (m.to%9) + (m.to/9))
			{
				moves.insert(m);
			}
		}
	}
	if (currentPlayer == 2)
	{
		for (const auto &m : allMoves)
		{
			if ((m.from%9)+(m.from/9) > (m.to%9) + (m.to/9))
			{
				moves.insert(m);
			}
		}
	}
}

void ChineseCheckersState::getMovesNotBackwards(std::set<Move> &moves) const {
	moves.clear();
	std::set<Move> allMoves;
	if (currentPlayer == 1){
		for (unsigned i : p1Loc)
		{
			getMovesSingleStep(allMoves, i);
			getMovesJumps(allMoves, i, i);
		}	
	} else {
		for (unsigned i : p2Loc)
		{
			getMovesSingleStep(allMoves, i);
			getMovesJumps(allMoves, i, i);
		}
	}
	if (currentPlayer == 1)
	{
		for (const auto &m : allMoves)
		{
			if ((m.from%9)+(m.from/9) <= (m.to%9) + (m.to/9))
			{
				moves.insert(m);
			}
		}
	}
	if (currentPlayer == 2)
	{
		for (const auto &m : allMoves)
		{
			if ((m.from%9)+(m.from/9) >= (m.to%9) + (m.to/9))
			{
				moves.insert(m);
			}
		}
	}
}


bool ChineseCheckersState::applyMove(Move m) {
	// Apply the move
	std::swap(board[m.from], board[m.to]);
	
	if (currentPlayer == 1){
		for (size_t i = 0; i < 10; i++)
			if (p1Loc[i] == m.from){
				p1Loc[i] = m.to;
				break;
			}
	} else {
		for (size_t i = 0; i < 10; i++)
			if (p2Loc[i] == m.from){
				p2Loc[i] = m.to;
				break;
			}
	}

	zhash = applyHashMove(zhash, m, currentPlayer);
	
	// Update whose turn it is
	swapTurn();
	
	return true;
}

bool ChineseCheckersState::undoMove(Move m) {
	// Apply the move
	std::swap(board[m.from], board[m.to]);
	
	if (currentPlayer == 2){
		for (size_t i = 0; i < 10; i++)
			if (p1Loc[i] == m.to){
				p1Loc[i] = m.from;
				break;
			}
	} else {
		for (size_t i = 0; i < 10; i++)
			if (p2Loc[i] == m.to){
				p2Loc[i] = m.from;
				break;
			}
	}

	zhash = applyHashMove(zhash, {m.from, m.to} , 3 - currentPlayer);
	
	// Update whose turn it is
	swapTurn();

	return true;
}

bool ChineseCheckersState::gameOver() const {
	return player1Wins() || player2Wins();
}

int ChineseCheckersState::winner() const {
	if (player1Wins())
		return 1;
	if (player2Wins())
		return 2;
	return -1; // No one has won
}
bool ChineseCheckersState::reset() {
	board = {{1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0,
		0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 2}};
	currentPlayer = 1;


	size_t k = 0;
	size_t l = 0;

	for (int i = 0; i < 81; i++){
		if (board[i] == 1){
			p1Loc[k] = i;
			k = k + 1;
		} else if (board[i] == 2){
			p2Loc[l] = i;
			l = l + 1;
		}
	}

	zhash = reHash();

	return true;
	
}

bool ChineseCheckersState::loadState(const std::string &newState) {
	// Tokenize newState using whitespace as delimiter
	std::stringstream ss(newState);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> tokenized(begin, end);
	
	// Ensure the length
	if (tokenized.size() != 82)
		return false;
	
	// Validate first item, whose turn it is
	if (tokenized[0] != "1" && tokenized[0] != "2")
		return false;
	
	try
	{
		currentPlayer = std::stoi(tokenized[0]);
	}
	catch (std::invalid_argument ev)
	{
		return false;
	}
	catch (std::out_of_range ev)
	{
		return false;
	}
	
	// Ensure rest of tokens are valid
	for (size_t i = 1, e = tokenized.size(); i != e; ++i)
	{
		try
		{
			int val = std::stoi(tokenized[i]);
			if (0 <= val && val <= 2)
				board[i - 1] = val;
			else
				return false;
		}
		catch (std::invalid_argument ek)
		{
			return false;
		}
		catch (std::out_of_range ek)
		{
			return false;
		}
	}
	return true;
}

std::string ChineseCheckersState::dumpState() const {
	std::stringstream out;
	out << currentPlayer;
	for (const auto i : board)
		out << " " << i;
	
	return out.str();
}

void ChineseCheckersState::getMovesSingleStep(std::set<Move> &moves, unsigned from) const {
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

void ChineseCheckersState::getMovesJumps(std::set<Move> &moves, unsigned from,
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


bool ChineseCheckersState::isValidMove(const Move &m) const {
	// Ensure from and to make sense
	if (board[m.from] != currentPlayer || board[m.to] != 0)
		return false;
	
	// NOTE: Checking validity in this way is inefficient
	
	// Get current available moves
	std::set<Move> moves;
	getMoves(moves);
	
	// Find the move among the set of available moves
	bool found = std::find(moves.begin(), moves.end(), m) != moves.end();
	
	return found;
}

Move ChineseCheckersState::translateToLocal(const std::vector<std::string> &tokens) const {
	// The numbers in the MOVE command sent by the moderator is already in the
	// format we need
	return Move{static_cast<unsigned>(std::stoi(tokens[2])),
		static_cast<unsigned>(std::stoi(tokens[4]))};
}

void ChineseCheckersState::swapTurn() {
	currentPlayer = currentPlayer == 1 ? 2 : 1;
}

bool ChineseCheckersState::player1Wins() const {
	// Wins by having all the bottom triangle filled and at least one is from the
	// first player
	
	bool p1inTriangle = false;
	for (const auto i : {53u, 61u, 62u, 69u, 70u, 71u, 77u, 78u, 79u, 80u})
	{
		if (board[i] == 0)
			return false;
		if (board[i] == 1)
			p1inTriangle = true;
	}
	
	return p1inTriangle;
}

bool ChineseCheckersState::player2Wins() const {
	// Wins by having all of top triangle filled and at least one is from the
	// second player
	
	bool p2inTriangle = false;
	for (const auto i : {0u, 1u, 2u, 3u, 9u, 10u, 11u, 18u, 19u, 27u})
	{
		if (board[i] == 0)
			return false;
		if (board[i] == 2)
			p2inTriangle = true;
	}
	
	return p2inTriangle;
}

void ChineseCheckersState::PrintASCII() {
	for (int x = 0; x < 9; x++)
	{
		for (int y = 0; y < 9; y++)
		{
			fprintf(stderr, "%d ", board[x+y*9]);
		}
		fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n");
}

uint64_t ChineseCheckersState::getHash() {
	return zhash;
}

int ChineseCheckersState::swapPlayer(){
	currentPlayer = 3 - currentPlayer;
	return currentPlayer;
}

int ChineseCheckersState::getPlayer(){
	return currentPlayer;
}

unsigned ChineseCheckersState::get(unsigned x) const{
	return board[x];
}


//---------------------------
//ZOBRIST HASHING
//---------------------------

void ChineseCheckersState::initZobrist(){
	//Initialize the move table
	for (size_t i = 0; i < 81; i++){
		for (size_t j = 0; j < 3; j++){
			uint64_t val = static_cast<uint64_t>(rand());
			val = val << 32;
			val |= static_cast<uint64_t>(rand());
			zobristMoveTable[i][j] = val;
		}
	}
}

bool ChineseCheckersState::LoadZobristTableFromFile(std::string filename){
	try{
		std::ifstream input(filename);
		for (size_t i = 0; i < 81; i++)
			for (size_t j = 0; j < 3; j++)
				input >> zobristMoveTable[i][j];
		input.close();
	}
	catch (std::ifstream::failure e){
		std::cerr << "Error loading zobrist table from file" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	return true;
}

bool ChineseCheckersState::SaveZobristTableToFile(std::string filename){
	try{
		std::ofstream output(filename);
		for (size_t i = 0; i < 81; i++)
			for (size_t j = 0; j < 3; j++)
				output << zobristMoveTable[i][j] << std::endl;
		output.close();
	}
	catch (std::ofstream::failure e){
		std::cerr << "Error saving zobrist table to file" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	return true;
}

uint64_t ChineseCheckersState::reHash(){
	uint64_t hash = 0ul;
	for (unsigned i = 0; i < 81; i++){
		hash ^= zobristMoveTable[i][board[i]];
	}
	return hash;
}

uint64_t ChineseCheckersState::applyHashMove(uint64_t hash, Move m, unsigned player){
	hash ^= zobristMoveTable[m.from][player];
	hash ^= zobristMoveTable[m.from][0];
	hash ^= zobristMoveTable[m.to][0];
	hash ^= zobristMoveTable[m.to][player];
	return hash;
}

std::string ChineseCheckersState::DumpZobrist(){
	std::stringstream ss;
	//Sanity check print the move table
	for (size_t i = 0; i < 82; i++){
		for (size_t j = 0; j < 3; j++){
			ss << zobristMoveTable[i][j] << " "; }
		ss << std::endl;
	}
	return ss.str();
}

