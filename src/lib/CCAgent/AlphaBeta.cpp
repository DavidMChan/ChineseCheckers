//Implementation of the alpha beta code


#include "CCAgent/AlphaBeta.h"

uint16_t AB::AlphaBetaRoot(CCBoard &state, TDEval &eval, float timelimit, int player){

	bool time = false;
	std::thread *t = new std::thread([&](){std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(timelimit*1000))); time = true;});
	
	uint16_t saved_best = 0; //Establish variable for the saved best move
	double sbs = 0;
	short depth = 0; //Set the initial depth of the algorithm to one

	bool isp1 = (player == 0); //Set a boolean variable for easily determining player 1

	//Clear the transposition table
	AB::Transpositions.clear();
	
	//While time has not expired
	while(!time){

		//Get the valid moves from the set
		std::vector<uint16_t> moves;
		state.getMoves(moves);

		//Set the initial alpha and beta values for the search
		double alpha = -100000;
		double beta = 100000;

		double best_score = -10000; //This is the initial best value. If we're searching for player 2
												   //we want to act like a min node or if we're searching for P1 we wamt
												   //to act like a max node
		
		uint16_t best_move = 0; //We're going to use this vector to keep track of the moves that we might want to make, 
									  //and then deal with choosing a random best move from the table

		for (uint16_t m : moves){

			state.applyMove(m);
			double val = -AB::NegaScout(state, eval, depth, alpha, beta, !isp1, time);
			state.undoMove(m);

				if (val > best_score){
					best_move = m;
					best_score = val;
				}
			

			//If we have a move that we can take to win the game, we shoudl short circuit
			//our evaluation of the board and take the move immediately.
		}

		//If time isn't up - we can select a move from t
		if (!time){
			saved_best = best_move;
			sbs = best_score;
			if (best_score > 100)
				break;

			
			depth ++; //If we're not done - we increase the depth of the search and begin again
		}
	}

	t->join();
	delete t;

	//We log the minimax result and depth on output in order to give some debug insight
	if (FLAGS_v){
		std::cerr << "Explored NegaScout with transpositions to depth " << depth  << "." << std::endl;
		std::cerr << "Selected move: " << CCBoard::moveToString(saved_best) << std::endl;
		std::cerr << "Game score: " << sbs << std::endl;
	}

	return saved_best;
}


double AB::NegaScout(CCBoard &state, TDEval &eval,  short depth, double alpha, double beta, bool player, bool &time){

	
	auto iter = AB::Transpositions.find(state.getHash());
	if (iter != AB::Transpositions.end()){
		AB::TTEntry entry = AB::Transpositions[state.getHash()];
		if (entry.depth >= depth && entry.player == player){
			switch(entry.type){
				case AB::Node::EXACT:
					return entry.score;
				case AB::Node::UPPER:
					if (entry.score >= beta){
						return entry.score;
					}
					else
						break;
				case AB::Node::LOWER:
					if (entry.score <= alpha){
						return entry.score;
					}
					else
						break;
			}
		}
	}

	if(time){
		return 1; 
	}
	if (state.gameOver()){
		return (-10000.0 * 1.0/static_cast<double>(depth));
	}
    if (depth <= 0)
    {   
    	//double score = AB::Quiesence(state, eval, 1, player, time);
        double score = eval.eval(state, state.getPlayer());
        return score;
    }
        
    double local_alpha,local_beta,temp_score;  
    local_alpha = alpha;   
    local_beta = beta;
    bool firstmove = true;
  
    bool eval_is_exact = false;

    AB::vectorCache.at(depth).clear();
	state.getMovesForward(AB::vectorCache.at(depth));
	
	if (player)
		std::sort(AB::vectorCache.at(depth).begin(),
				std::min(AB::vectorCache.at(depth).begin() + 50, AB::vectorCache.at(depth).end()),
				[player](uint16_t i, uint16_t j){return AB::HistoryHeuristicPlayer1[i] > AB::HistoryHeuristicPlayer1[j];});
	else
		std::sort(AB::vectorCache.at(depth).begin(),
				std::min(AB::vectorCache.at(depth).begin() + 50, AB::vectorCache.at(depth).end()),
				[player](uint16_t i, uint16_t j){return AB::HistoryHeuristicPlayer2[i] > AB::HistoryHeuristicPlayer2[j];});

    for (auto m : AB::vectorCache.at(depth)){
        state.applyMove(m);
        temp_score = -AB::NegaScout(state, eval, depth-1 , -local_beta, -local_alpha, !player, time);
           
        if (temp_score > local_alpha && temp_score < beta &&  !firstmove)    
        {   
            local_alpha = -AB::NegaScout (state, eval, depth-1, -beta, -temp_score, !player, time); //Re-search if the move oridering is off   
            eval_is_exact = true;
   		} 
        state.undoMove(m);

        if (local_alpha < temp_score)   
        {   
            eval_is_exact = true;   
            local_alpha = temp_score;   
        }   
        if ( local_alpha >= beta )    
        {   
        	AB::Transpositions[state.getHash()] = {state.getHash(), depth, beta, player, AB::Node::UPPER, alpha, beta};
        	if (player)
        		AB::HistoryHeuristicPlayer1[m] += depth << 1;
        	else
        		AB::HistoryHeuristicPlayer2[m] += depth << 1;
            return local_alpha;
        }

        firstmove = false;
        local_beta = local_alpha + 0.01; //Setup a new null window  
    }

    if (eval_is_exact)
		AB::Transpositions[state.getHash()] = {state.getHash(), depth, local_alpha, player, AB::Node::EXACT, alpha, beta};
	else
		AB::Transpositions[state.getHash()] = {state.getHash(), depth, local_alpha, player, AB::Node::LOWER, alpha, beta};

    return local_alpha;   
}


double AB::Quiesence(CCBoard &state, TDEval &eval,  short depth, bool player, bool &time){
	if (depth == 0 || state.gameOver() || time){
		double score = eval.eval(state, state.getPlayer());  
		return score;
	}

	AB::vectorCache.at(20+depth).clear();
	state.getMovesForward(AB::vectorCache.at(20+depth));
	double best_score = INT_MIN;

	for (auto m : AB::vectorCache.at(20+depth)){
		state.applyMove(m);
		best_score = std::max(best_score, -AB::Quiesence(state, eval, static_cast<short>(depth - 1), !player, time));
		state.undoMove(m);
	}

	return best_score;
}