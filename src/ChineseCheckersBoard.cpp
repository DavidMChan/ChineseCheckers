//Implementation of the chinese checkers board
//David Chan - 2015

#include "ChineseCheckersBoard.h"

/**
 * Consteuctor for the chinese checkers board. This method sets up
 * the memory for the board, and the piece locations.
 */
ChineseCheckersBoard::ChineseCheckersBoard(){
	//board = new BoardHex[81]; //The purpose of the array is really to keep the graph structure in continuous memory
	current_player = Player::PLAYER1;

	//Set beginning piece locations for player 1
	whitePieceLocations[0] = 0;
	whitePieceLocations[1] = 1;
	whitePieceLocations[2] = 2;
	whitePieceLocations[3] = 3;
	whitePieceLocations[4] = 9;
	whitePieceLocations[5] = 10;
	whitePieceLocations[6] = 11;
	whitePieceLocations[7] = 18;
	whitePieceLocations[8] = 19;
	whitePieceLocations[9] = 27;

	//Set beginning piece locations for player 2
	blackPieceLocations[0] = 53;
	blackPieceLocations[1] = 61;
	blackPieceLocations[2] = 62;
	blackPieceLocations[3] = 69;
	blackPieceLocations[4] = 70;
	blackPieceLocations[5] = 71;
	blackPieceLocations[6] = 77;
	blackPieceLocations[7] = 78;
	blackPieceLocations[8] = 79;
	blackPieceLocations[9] = 80;


	//Initialize all of the board connections and pointers
	for (int i = 0; i < 81; ++i){
		board[i].num = i;
		board[i].state = HEXSTATE::EMPTY;
		board[i].marked = false;

		unsigned row = i / 9;
  		unsigned col = i % 9;

  		// Up Left
  		if (col > 0)
    		board[i].adjacent[0] = &board[i-1];
    	else
    		board[i].adjacent[0] = 0;

		// Up Right
		if (row > 0)
			board[i].adjacent[1] = &board[i-9];
		else
    		board[i].adjacent[1] = 0;

		// Left
		if (col > 0 && row < 8)
			board[i].adjacent[2] = &board[i+8];
		else
    		board[i].adjacent[2] = 0;

		// Right
		if (col < 8 && row > 0 )
			board[i].adjacent[3] = &board[i-8];
		else
    		board[i].adjacent[3] = 0;

		// Down Left
		if (row < 8)
			board[i].adjacent[4] = &board[i+9];
		else
    		board[i].adjacent[4] = 0;

		// Down Right
		if (col < 8)
			board[i].adjacent[5] = &board[i+1];
		else
    		board[i].adjacent[5] = 0;
	}

	//Update all of the locations in the board
	for (unsigned i = 0; i < 10; ++i){
		board[whitePieceLocations[i]].state = HEXSTATE::P1;
		board[blackPieceLocations[i]].state = HEXSTATE::P2;
	}

	//We're now ready to play!
}

ChineseCheckersBoard::~ChineseCheckersBoard(){
	//delete[] board; //Clean up the board array
}

/**
 * Method designed to return all of the possible moves in the array
 * @param moves std::set<Move> passed by reference which will store the returned moves when done
 */
void ChineseCheckersBoard::getValidMoves(std::set<Move> &moves){
	const unsigned int* marray = (current_player == Player::PLAYER1 ? whitePieceLocations : blackPieceLocations);
	std::deque<unsigned> q; //queue for getting jump moves

	for (unsigned i = 0; i < 10; ++i){

		//Push back all of the adjacent moves
		for (auto iter = board[marray[i]].adjacent.begin(); iter != board[marray[i]].adjacent.end(); ++iter){
			if (*iter != 0 && (*iter)->state == HEXSTATE::EMPTY)
				moves.insert({marray[i],(*iter)->num});
		}

		

		//Get the jump moves
		q.push_back(marray[i]);
		while (!q.empty()){
			unsigned current = q.front();
			q.pop_front();
			if (board[current].marked)
				continue;
			board[current].marked = true;
			for (size_t k = 0; k < 6; k++){
				if (board[current].adjacent[k] != 0 && board[current].adjacent[k]->state != HEXSTATE::EMPTY
					&& board[current].adjacent[k]->adjacent[k] != 0 && board[current].adjacent[k]->adjacent[k]->state == HEXSTATE::EMPTY 
					&& !board[current].adjacent[k]->adjacent[k]->marked){
						q.push_back(board[current].adjacent[k]->adjacent[k]->num);
						moves.insert({marray[i],q.back()});
				}	
			}
		}

		for (unsigned j = 0; j < 81; ++j)
			board[j].marked = false; //Clean up the marks	
		}


}

/**
 * Apply a move to the game board
 * @param  m The move to apply
 * @return   True if successful
 */
bool ChineseCheckersBoard::applyMove(Move m){
	if (board[m.from].state == HEXSTATE::P1){
		for (unsigned i = 0; i < 10; ++i)
			if (whitePieceLocations[i] == m.from)
				whitePieceLocations[i] = m.to;
	}
	else{
		for (unsigned i = 0; i < 10; ++i)
			if (blackPieceLocations[i] == m.from)
				blackPieceLocations[i] = m.to;
	}

	board[m.to].state = board[m.from].state;
	board[m.from].state = HEXSTATE::EMPTY;
	return true;
}

/**
 * Undo a move m
 * @param  m The move to undo
 * @return   True if successful
 */
bool ChineseCheckersBoard::undoMove(Move m){
	unsigned temp = m.from;
	m.from = m.to;
	m.to = temp;
	return applyMove(m);
}

//Safely apply a move (check to see if it is valid first). Returns true if successful
bool ChineseCheckersBoard::applyMove_SAFE(Move m){ return false;}
//Safely undo a move (check to see if it is valid first). Returns true if successful
bool ChineseCheckersBoard::undoMove_SAFE(Move m) {return false;}

bool ChineseCheckersBoard::isValidMove(Move m){
	std::set<Move> moves;
	getValidMoves(moves);
	auto iter = std::find(moves.begin(), moves.end(), m);
	return iter != moves.end();
}

//Return if the board is in a game over state
bool ChineseCheckersBoard::gameOver() const{
	return !(winner() == 0);
} 

//Return 1 if player 1, 2 if player 2, or 0 if neither
int ChineseCheckersBoard::winner() const{
	return player1Wins() ? 1 : (player2Wins() ? 2 : 0);
} 

bool ChineseCheckersBoard::player1Wins() const {
  // Wins by having all the bottom triangle filled and at least one is from the
  // first player

  bool p1inTriangle = false;
  for (const auto i : {53u, 61u, 62u, 69u, 70u, 71u, 77u, 78u, 79u, 80u}) {
    if (board[i].state == HEXSTATE::EMPTY)
      return false;
    if (board[i].state == HEXSTATE::P1)
      p1inTriangle = true;
  }

  return p1inTriangle;
}

bool ChineseCheckersBoard::player2Wins() const {
  // Wins by having all of top triangle filled and at least one is from the
  // second player

  bool p2inTriangle = false;
  for (const auto i : {0u, 1u, 2u, 3u, 9u, 10u, 11u, 18u, 19u, 27u}) {
    if (board[i].state == HEXSTATE::EMPTY)
      return false;
    if (board[i].state == HEXSTATE::P2)
      p2inTriangle = true;
  }

  return p2inTriangle;
}

//Resets the board to the original state. Returns true if successful.
bool ChineseCheckersBoard::reset(){
	current_player = Player::PLAYER1;

	//Set beginning piece locations for player 1
	whitePieceLocations[0] = 0;
	whitePieceLocations[1] = 1;
	whitePieceLocations[2] = 2;
	whitePieceLocations[3] = 3;
	whitePieceLocations[4] = 9;
	whitePieceLocations[5] = 10;
	whitePieceLocations[6] = 11;
	whitePieceLocations[7] = 18;
	whitePieceLocations[8] = 19;
	whitePieceLocations[9] = 27;

	//Set beginning piece locations for player 2
	blackPieceLocations[0] = 53;
	blackPieceLocations[1] = 61;
	blackPieceLocations[2] = 62;
	blackPieceLocations[3] = 69;
	blackPieceLocations[4] = 70;
	blackPieceLocations[5] = 71;
	blackPieceLocations[6] = 77;
	blackPieceLocations[7] = 78;
	blackPieceLocations[8] = 79;
	blackPieceLocations[9] = 80;

	for (unsigned i = 0; i < 81; ++i){
		board[i].num = i;
		board[i].state = HEXSTATE::EMPTY;
		board[i].marked = false;
	}

	//Update all of the locations in the board
	for (unsigned i = 0; i < 10; ++i){
		board[whitePieceLocations[i]].state = HEXSTATE::P1;
		board[blackPieceLocations[i]].state = HEXSTATE::P2;
	}

	return true;
}  


//Load the board state from a string
bool ChineseCheckersBoard::loadState(std::string newState){
	  // Tokenize newState using whitespace as delimiter
  std::stringstream ss(newState);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> tokenized(begin, end);

  reset(); //Reset the board

  // Ensure the length
  if (tokenized.size() != 82)
    return false;

  // Validate first item, whose turn it is
  if (tokenized[0] != "1" && tokenized[0] != "2")
    return false;

  try {
    current_player = std::stoi(tokenized[0]) == 1 ? Player::PLAYER1 : Player::PLAYER2;
  } catch (std::invalid_argument e) {
    return false;
  } catch (std::out_of_range e) {
    return false;
  }

  // Ensure rest of tokens are valid
  int wc = 0, bc = 0;
  for (unsigned i = 1, e = tokenized.size(); i != e; ++i) {
    try {
      int val = std::stoi(tokenized[i]);
      if (0 <= val && val <= 2){
      	 board[i - 1].state = val == 0 ? HEXSTATE::EMPTY : (val == 1 ? HEXSTATE::P1 : HEXSTATE::P2);
      	 if (val == 1){
      	 	whitePieceLocations[wc] = i-1;
      	 	wc++;
      	 }
      	 else if (val == 2){
      	 	blackPieceLocations[bc] = i-1;
      	 	bc++;
      	 }
      }
      else
        return false;
    } catch (std::invalid_argument e) {
      return false;
    } catch (std::out_of_range e) {
      return false;
    }
  }
  return true;
} 

std::string ChineseCheckersBoard::dumpState() const {
  std::stringstream out;
  out << current_player + 1;
  for (const auto i : board)
    out << " " << (i.state);

  return out.str();
}

Move ChineseCheckersBoard::translateToLocal(const std::vector<std::string> &tokens) const {
  // The numbers in the MOVE command sent by the moderator is already in the
  // format we need
  return Move{static_cast<unsigned>(std::stoi(tokens[2])),
              static_cast<unsigned>(std::stoi(tokens[4]))};
}

ChineseCheckersBoard::Player ChineseCheckersBoard::swapPlayer(){
	if (current_player == ChineseCheckersBoard::Player::PLAYER1)
		current_player = ChineseCheckersBoard::Player::PLAYER2;
	else
		current_player = ChineseCheckersBoard::Player::PLAYER1;
	return current_player;
}

int ChineseCheckersBoard::getPlayer(){
	return current_player == ChineseCheckersBoard::Player::PLAYER1 ? 0 : 1;
}

