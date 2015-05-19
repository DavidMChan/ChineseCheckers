//UCB CPP file

#include "CCAgent/UCB.h"

/**
 * [Get a move using the random-full playout method]
 * @param  timelimit [The time that the player can take]
 * @return           [the move that should be made]
 */
uint16_t UCBPlayer::GetMove(double timelimit){

	//Start the timer
	bool time = false;
	std::thread *time_thread = new std::thread([&](){
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((timelimit-0.01)*1000))); time = true;
	});

	//Setup temporary variables
	uint16_t best_move = 0;
	double best_eval = 0;
	double temp_eval = 0;

	//Play out all moves once
	for (auto m : moves){
		PlayOut(m);
	}

	//Whilest we still have time on the clock
	while (!time){

		std::random_shuffle(moves.begin(), moves.end());

		for (auto m : moves){
			temp_eval = EvaluateStateUCB1(m); //Pick the best move to evaluate
			if (temp_eval > best_eval){
				best_move = m;
				best_eval = temp_eval;
			}
		}
		PlayOut(best_move); //Perform a random playout on that move
	}


	//Clean up the threading
	time_thread->join();
	delete time_thread;


	//Calculate the best move now that we are done playing out the games
	double average = 0;
	double best_average = 0;
	for (auto m : moves){
		average = (static_cast<double>(value[m])/static_cast<double>(plays[m]));
		if (average > best_average){
			best_average = average;
			best_move = m;
		}
	}

	if (FLAGS_v){
		for (auto m : moves)
		{
			std::cerr << "{" << plays[m] << "," << value[m] << "} ";
		}
		std::cerr << std::endl;

		std::cerr << "Performed " << total_plays << " full playouts." << std::endl;
		std::cerr << "Best average: " << best_average << std::endl <<std::endl;
	}

	return best_move;

}


/**
 * [The evaluation and depth based UCB player]
 * @param  timelimit [The time limit that the player has]
 * @param  eval      [The TDEval to use]
 * @param  depth     [The depth to explore to]
 * @return           [The move that should be made]
 */
uint16_t UCBPlayer::GetMove(double timelimit, TDEval &eval, unsigned depth){

	//Start the timer
	bool time = false;
	std::thread *time_thread = new std::thread([&](){
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((timelimit-0.01)*1000))); time = true;
	});

	//Setup temporary variables
	uint16_t best_move = 0;
	double best_eval = 0;
	double temp_eval = 0;

	//Play out all moves once
	for (auto m : moves){
		PlayOut(m, eval, depth);
	}

	
	//Whilest we still have time on the clock
	while (!time){
	
		std::random_shuffle(moves.begin(), moves.end());

		for (auto m : moves){
			temp_eval = EvaluateStateUCB1(m); //Pick the best move to evaluate
			if (temp_eval > best_eval){
				best_move = m;
				best_eval = temp_eval;
			}
		}
		PlayOut(best_move); //Perform a random playout on that move
	}

	//Clean up the threading
	time_thread->join();
	delete time_thread;


	//Calculate the best move now that we are done playing out the games
	double average = 0;
	double best_average = 0;
	for (auto m : moves){
		average = (static_cast<double>(value[m])/static_cast<double>(plays[m]));
		if (average > best_average){
			best_average = average;
			best_move = m;
		}
	}

	if (FLAGS_v){
		for (auto m : moves)
		{
			std::cerr << "{" << plays[m] << "," << value[m] << "} ";
		}
		std::cerr << std::endl;

		std::cerr << "Performed " << total_plays << " depth " << depth << " playouts." << std::endl;
		std::cerr << "Best average: " << best_average << std::endl <<std::endl;
	}

	return best_move;

}

/**
 * [Random epsilon greedy full game playout]
 * @param move [The move to begin with]
 */
void UCBPlayer::PlayOut(uint16_t move){

	//Make a copy of the state and apply the first move
	CCBoard board = state;
	int player = state.getPlayer();
	board.applyMove(move);


	//Setup the cached variables that we're going to be using during the playout
	unsigned epsilon = 10; //For now, we have an epsilon greedy factor of 10%
	uint16_t temp_best_move = 0;
	int temp_longest  = 0;
	int temp_distance = 0;
	unsigned from = 0;
	unsigned to = 0;

	//Perform the playout
	while (!board.gameOver()){

		//Make a random move based on epsilon
		if (rand() % epsilon == 0){
			board.applyMove(board.getRandomMove());
		} else {
			//Or get the moves
			board.getMoves(move_cache);

			//Randomly shuffle them to deal with ties (for the end-game mostly)
			std::random_shuffle(move_cache.begin(), move_cache.end());

			//Reset the distance  variables
			temp_longest = 0;
			temp_distance = 0;
			
			//Calculate the longest possible move
			for (auto m : move_cache){
				CCBoard::unmovify(m, from, to);
				if (board.getPlayer() == 1){
					temp_distance = (to / 9+ to % 9) - (from / 9 + from % 9);
				} else {
					temp_distance = -((to / 9 + to % 9) - (from / 9 + from % 9));
				}
				if (temp_distance > temp_longest){
					temp_longest = temp_distance;
					temp_best_move = m;
				}
			}

			//Apply that move to the game
			board.applyMove(temp_best_move);
		}
	}

	assert(board.winner() != -1);

	if (board.winner() == player){
		value[move] += 1;
		plays[move] += 1;
		total_plays += 1;
	} else {
		plays[move] += 1;
		total_plays += 1;
	}
}


/**
 * [Random epsilon greedy full game playout - depth based]
 * @param move [The move to begin with]
 */
void UCBPlayer::PlayOut(uint16_t move, TDEval &eval, unsigned max_depth){

	//Make a copy of the state and apply the first move
	CCBoard board = state;
	int player = state.getPlayer();
	board.applyMove(move);


	//Setup the cached variables that we're going to be using during the playout
	unsigned epsilon = 10; //For now, we have an epsilon greedy factor of 10%
	uint16_t temp_best_move;
	uint16_t temp_longest  = 0;
	uint16_t temp_distance = 0;
	unsigned from;
	unsigned to;
	unsigned depth = 0;

	//Perform the playout
	while (!board.gameOver() && depth <= max_depth){

		//Make a random move based on epsilon
		if (rand() % epsilon == 0){
			board.applyMove(board.getRandomMove());
		} else {
			//Or get the forward moves
			board.getMoves(move_cache);

			//Randomly shuffle them to deal with ties (for the end-game mostly)
			std::random_shuffle(move_cache.begin(), move_cache.end());

			//Reset the distance  variables
			temp_longest = 0;
			temp_distance = 0;
			
			//Calculate the longest possible move
			for (auto m : move_cache){
				CCBoard::unmovify(m, from, to);
				if (board.getPlayer() == 1){
					temp_distance = (to / 9+ to % 9) - (from / 9 + from % 9);
				} else {
					temp_distance = -((to / 9 + to % 9) - (from / 9 + from % 9));
				}
				if (temp_distance > temp_longest){
					temp_longest = temp_distance;
					temp_best_move = m;
				}
			}
			
			//Apply that move to the game
			board.applyMove(temp_best_move);
		}

		depth++;
	}

	//Evaluate the state and update the playout board
	value[move] += eval.eval(board, player);
	plays[move] += 1;
	total_plays += 1;
}

/**
 * [Evaluate a given move with the UCB1 parameters]
 * @param  m [The move to evaluate]
 * @return   [The UCB1 score of the move]
 */
double UCBPlayer::EvaluateStateUCB1(uint16_t m){
	double score = ((value[m] / static_cast<double>(plays[m])) + 
		sqrt((2.0 * log(static_cast<double>(total_plays)) / static_cast<double>(plays[m])))); 
	return score;
}