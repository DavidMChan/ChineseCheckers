//This will be an implementation for the bit board
//chinese checkers state representation. Hopefully it 
//will give me some more depths.

#include "CCAgent/CCBitboard.h"

//using namespace boost::multiprecision;

bool CCBoard::hasInitZobrist = false;
std::array<std::array<uint64_t, 3>,81> CCBoard::zobristMoveTable;

/**
 * Construct a chinese checkers board with the bitwise representation
 */
 CCBoard::CCBoard() : move_cache(20){

		reset();

 		if (!hasInitZobrist)
 			initZobrist();

 		zhash = reHash();

 	}

/**
 * [Apply a move to the checkers board]
 * @param  move [The move that should be applied to the game board]
 * @return      [True if successful]
 */
 bool CCBoard::applyMove(uint16_t move, bool undo){
 	uint128_t temp = static_cast<uint128_t>(1);

 	unsigned short from = (move & 0xFF00) >> 8;
 	unsigned short to = move & 0xFF;

 	uint128_t tf = (temp << from);
 	uint128_t tt = (temp << to);

 	if ((boardp1 & (tf)) > 0){
 		boardp1 &= (~(tf));
 		boardp1 |= (tt);
 	} else {
 		boardp2 &= (~(tf));
 		boardp2 |= (tt);
 	}

 	board &= (~(tf));
 	board |= (tt);

 	zhash = applyHashMove(zhash, move, undo ? player : !player);

 	player = !player;

 	return true;
 }

/**
 * [Undo a move on the underlying bit-board]
 * @param  move [The move to undo]
 * @return      [True if successful]
 */
 bool CCBoard::undoMove(uint16_t move){
 	unsigned short from = (move & 0xFF00) >> 8;
 	unsigned short to = move & 0xFF;
 	uint16_t newmove = 0;
 	newmove |= from;
 	newmove |= to << 8;
 	return applyMove(newmove, true);
 }

/**
 * [Save a list of moves that can be made to the passed in vector]
 * @param moves [A vector that will be overwritten with the moves that can be made from this state]
 */
 void CCBoard::getMoves(std::vector<uint16_t> &moves) const{
 	moves.clear();
 	uint128_t temp = static_cast<uint128_t>(1);
 	uint128_t loop = static_cast<uint128_t>(1);

 	uint128_t visited;

 	uint128_t boa = player ? boardp1 : boardp2;

 	unsigned work;
 	unsigned row;
 	unsigned col;

 	for (unsigned i = 0; i < 81; i++){
 		if ((boa & (loop)) != 0){
				//Let's get the adjacent moves first

 			row = i / 9;
 			col = i % 9;

 			if (col > 0 && (board & (temp << (i - 1))) == 0){
 				moves.push_back(moveify(i, i-1));
 			}
 			if (row > 0 && (board & (temp << (i - 9))) == 0){
 				moves.push_back(moveify(i, i-9));
 			}
 			if (col > 0 && row < 8 && (board & (temp << (i + 8))) == 0){
 				moves.push_back(moveify(i, i + 8));
 			}
 			if (col < 8 && row > 0 && (board & (temp << (i - 8))) == 0){
 				moves.push_back(moveify(i, i-8));
 			}
 			if (row < 8 && (board & (temp << (i + 9))) == 0){
 				moves.push_back(moveify(i, i+9));
 			}
 			if (col < 8 && (board & (temp << (i + 1))) == 0){
 				moves.push_back(moveify(i, i + 1));
 			}


				//Ok, now for the jump moves

 			visited = static_cast<uint128_t>(0);
 			q.push(i);

 			while(!q.empty()){
 				work = q.front();
 				q.pop();

 				visited |= (temp << work);

 				row = work / 9;
 				col = work % 9;

					//Up Left
 				if (col > 1 && (board & (temp << (work - 2))) == 0 && (board & (temp << (work-1))) > 0 && (visited & (temp << (work - 2))) == 0) {
 					q.push(work - 2);
 					moves.push_back(moveify(i, work - 2));
 				}

					//Up right
 				if (row > 1 && (board & (temp << (work - 18))) == 0 && (board & (temp << (work-9))) > 0 && (visited & (temp << (work - 18))) == 0) {
 					q.push(work - 18);
 					moves.push_back(moveify(i, work - 18));
 				}

					//Left
 				if (col > 1 && row < 7 && (board & (temp << (work + 16))) == 0 && (board & (temp << (work+8))) > 0 && (visited & (temp << (work + 16))) == 0) {
 					q.push(work + 16);
 					moves.push_back(moveify(i, work + 16));
 				}

					//Right
 				if (col < 7 && row > 1 && (board & (temp << (work - 16))) == 0 && (board & (temp << (work-8))) > 0 && (visited & (temp << (work - 16))) == 0) {
 					q.push(work - 16);
 					moves.push_back(moveify(i, work - 16));
 				}

					//Down Left
 				if (row < 7 && (board & (temp << (work + 18))) == 0 && (board & (temp << (work + 9))) > 0 && (visited & (temp << (work + 18))) == 0) {
 					q.push(work + 18);
 					moves.push_back(moveify(i, work + 18));
 				}

					//Down Right
 				if (col < 7 && (board & (temp << (work + 2))) == 0 && (board & (temp << (work + 1))) > 0 && (visited & (temp << (work + 2))) == 0) {
 					q.push(work + 2);
 					moves.push_back(moveify(i, work + 2));
 				}

				} //End While


			} //End if
			loop <<= 1;
		} //End For
}


/**
 * [Get a random move]
 * @return [Returns the random move]
 */
uint16_t CCBoard::getRandomMove() const {
	getMoves(move_cache);
	size_t choice = rand() % move_cache.size();
	auto m = move_cache.begin();
	std::advance(m, choice);
	return *m;
}

/**
 * [Get the forward moves]
 * @param moves [The vector to store the moves in]
 */
void CCBoard::getMovesForward(std::vector<uint16_t> &moves) const {
	moves.clear();
	getMoves(move_cache);
	unsigned from;
	unsigned to;

	if (player)
	{
		for (const auto &m : move_cache)
		{
			unmovify(m, from, to);
			if ((from%9)+(from/9) < (to%9) + (to/9))
			{
				moves.push_back(m);
			}
		}
	}
	else
	{
		for (const auto &m : move_cache)
		{
			unmovify(m, from, to);
			if ((from%9)+(from/9) > (to%9) + (to/9))
			{
				moves.push_back(m);
			}
		}
	}
}

/**
 * [Get the moves that don't go backwards]
 * @param moves [The vector to store the moves in]
 */
void CCBoard::getMovesNotBackwards(std::vector<uint16_t> &moves) const {
	moves.clear();
	getMoves(move_cache);
	unsigned from;
	unsigned to;
	if (player)
	{
		for (const auto &m : move_cache)
		{
			unmovify(m, from, to);
			if ((from%9)+(from/9) <= (to%9) + (to/9))
			{
				moves.push_back(m);
			}
		}
	}
	else
	{
		for (const auto &m : move_cache)
		{
			unmovify(m, from, to);
			if ((from%9)+(from/9) >= (to%9) + (to/9))
			{
				moves.push_back(m);
			}
		}
	}
}


/**
 * [Get the zobrist hash of the state]
 * @return [the zobrist hash]
 */
 uint64_t CCBoard::getHash() const{
 	return zhash;
 }

/**
 * [Returns true if the game is over]
 * @return [True if the game has finished]
 */
 bool CCBoard::gameOver() const{
 	if ((board & gameOverp1Wins) == gameOverp1Wins){
 		if ((boardp1 & gameOverp1Wins ) > static_cast<uint128_t>(0)){
 			return true;
 		}
 	}
 	else if ((board & gameOverp2Wins) == gameOverp2Wins){
 		if ((boardp2 & gameOverp2Wins) > static_cast<uint128_t>(0)){
 			return true;
 		}
 	}
 	return false;
 }

/**
 * [Return who won the game - -1 otherwise]
 * @return [1,2,-1 depending on the winner]
 */
 int CCBoard::winner() const{
 	if ((board & gameOverp1Wins) == gameOverp1Wins){
 		if ((boardp1 & gameOverp1Wins ) > 0){
 			return 1;
 		}
 	}
 	else if ((board & gameOverp2Wins) == gameOverp2Wins){
 		if ((boardp2 & gameOverp2Wins) > 0){
 			return 2;
 		}
 	}
 	return -1;
 }

/**
 * [Get who owns a particular board location]
 * @param  x [The location to get]
 * @return   [Who owns that location]
 */
 unsigned CCBoard::get(unsigned x) const {
 	uint128_t temp = static_cast<uint128_t>(1);
 	temp <<= x;
 	return ((board & (temp)) != 0) ? (((boardp1 & (temp)) > 0) ? 1 : 2) : 0;
 }

/**
 * [Gets the current player in integer form]
 * @return [1 if player 1, 2 if player 2]
 */
 unsigned CCBoard::getPlayer() const{
 	return player ? 1 : 2;
 }

/**
 * [Takes a uint16_t move and turns it into a string]
 * @param  move [The move to stringify]
 * @return      [The string version of the move]
 */
 std::string CCBoard::moveToString(uint16_t move){
 	unsigned short from = (move & 0xFF00) >> 8;
 	unsigned short to = move & 0xFF;
 	std::stringstream ss;
 	ss << "{" << from << "," << to << "}";
 	return ss.str();
 }

/**
 * [Prints the board that is passed in]
 * @param  v [The board to print]
 * @return   [A string version of the board]
 */
 std::string CCBoard::printBoard(uint128_t v){
 	uint128_t temp = static_cast<uint128_t>(1);
 	std::stringstream ss;
 	for (int i = 0; i < 81; i++)
 		ss << (((v & (temp << i)) > 0) ? "1" : "0");
 	ss << std::endl;
 	return ss.str();
 }

/**
 * [Swaps the player that is to play on the current board]
 * @return [The current player]
 */
 unsigned CCBoard::swapPlayer(){
 	player = !player;
 	return player ? 1 : 2;
 }

/**
 * [returns the string version of the board, as is required by the Agent protocol]
 * @return [String version of the board]
 */
 std::string CCBoard::dumpState() const {
 	uint128_t temp = static_cast<uint128_t>(1);
 	std::stringstream out;
 	out << (player ? "1" : "2");
 	for (int i = 0; i < 81; i++)
 		out << " " << (((boardp1 & (temp << i)) > 0) ? "1" : (((boardp2 & (temp << i)) > 0) ? "2" : "0") );
 	return out.str();
 }

/**
 * [Load a board from a given string]
 * @param  newState [The string to apply to the board]
 * @return          [True if successful]
 */
 bool CCBoard::loadState(std::string newState){
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
  	player = std::stoi(tokenized[0]) == 1 ? true : false;
  } catch (std::invalid_argument e) {
  	return false;
  } catch (std::out_of_range e) {
  	return false;
  }

  // Ensure rest of tokens are valid
  board = static_cast<uint128_t>(0); //Clear the board
  boardp1 = static_cast<uint128_t>(0);
  boardp2 = static_cast<uint128_t>(0);
  uint128_t temp = static_cast<uint128_t>(1);

  for (unsigned i = 1, tok = static_cast<unsigned int>(tokenized.size()); i != tok; ++i) {
  	try {
  		int val = std::stoi(tokenized[i]);
  		if (0 <= val && val <= 2){
  			if (val == 1){
  				board |= (temp << (i - 1));
  				boardp1 |= (temp << (i - 1));
  			}
  			else if (val == 2){
  				board |= (temp << (i - 1));
  				boardp2 |= (temp << (i - 1));
  			}
  		}
  		else
  			return false;
  	} catch (std::invalid_argument err) {
  		return false;
  	} catch (std::out_of_range er) {
  		return false;
  	}
  }
  return true;
} 

/**
 * [Reset the board to the beginning state]
 * @return [True if successful]
 */
 bool CCBoard::reset(){
	board = static_cast<uint128_t>(0); //Clear the board
  	boardp1 = static_cast<uint128_t>(0);
  	boardp2 = static_cast<uint128_t>(0);
 	uint128_t temp = static_cast<uint128_t>(1);
 	gameOverp1Wins = static_cast<uint128_t>(0);	
 	gameOverp2Wins = static_cast<uint128_t>(0);

 	unsigned barr[] = {1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0,
 		0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 		0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0,
 		0, 0, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 2};

 		for (int i = 0; i < 81; i++){
 			if (barr[i] == 1){
 				board |= (temp << i);
 				boardp1 |= (temp << i);
 				gameOverp2Wins |= (temp << i);
 			} else if (barr[i] == 2){
 				board |= (temp << i);
 				boardp2 |= (temp << i);
 				gameOverp1Wins |= (temp << i);
 			}
 		}

 		player = true;

 		return true;
 	}

/**
 * [Load the zobrist table from a file (for good transposition tables)]
 * @param  filename [The filepath to load from]
 * @return          [True if successful]
 */
 bool CCBoard::LoadZobristTableFromFile(std::string filename){
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

/**
 * [Save the zobrist table to a file]
 * @param  filename [The file to save to]
 * @return          [True if successful]
 */
 bool CCBoard::SaveZobristTableToFile(std::string filename){
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

/**
 * [Get the zobrist hash for a board state]
 * @return [description]
 */
 uint64_t CCBoard::reHash(){
 	uint64_t hash = 0ul;
 	uint128_t temp = 1;
 	for (unsigned i = 0; i < 81; i++){
 		hash ^= zobristMoveTable[i][(board & temp) > 0 ? ((boardp1 & temp) > 0 ? 1 : 2) : 1];
 		temp <<= 1;
 	}
 	return hash;
 }
/**
 * [Apply a move for zobrist hashing]
 * @param  hash   [The hash to start with]
 * @param  move   [The move to make]
 * @param  player [The player that is making the move]
 * @return        [The new hash value]
 */
 uint64_t CCBoard::applyHashMove(uint64_t hash, uint16_t move, bool p){
 	unsigned short from = (move & 0xFF00) >> 8;
 	unsigned short to = move & 0xFF;

 	hash ^= zobristMoveTable[from][p ? 1 : 2];
 	hash ^= zobristMoveTable[to][0];
 	hash ^= zobristMoveTable[from][0];
 	hash ^= zobristMoveTable[to][p ? 1 : 2];
 	return hash;
 }

/**
 * [Dump the Zobrist table to a string]
 * @return [The Zobrist string]
 */
 std::string CCBoard::DumpZobrist(){
 	std::stringstream ss;
	//Sanity check print the move table
 	for (size_t i = 0; i < 82; i++){
 		for (size_t j = 0; j < 3; j++){
 			ss << zobristMoveTable[i][j] << " "; }
 			ss << std::endl;
 		}
 		return ss.str();
 	}

/**
 * [Initialize the Zobrist table if it hasn't already been done]
 */
 void CCBoard::initZobrist(){
	//Initialize the move table
 	for (size_t i = 0; i < 81; i++){
 		for (size_t j = 0; j < 3; j++){
 			uint64_t val = static_cast<uint64_t>(rand());
 			val = val << 32;
 			val |= static_cast<uint64_t>(rand());
 			zobristMoveTable[i][j] = val;
 		}
 	}
 	hasInitZobrist = true;
 }

 uint16_t CCBoard::translateToLocal(const std::vector<std::string> &tokens) const {
	// The numbers in the MOVE command sent by the moderator is already in the
	// format we need
	return moveify(static_cast<unsigned>(std::stoi(tokens[2])),
		static_cast<unsigned>(std::stoi(tokens[4])));
}

