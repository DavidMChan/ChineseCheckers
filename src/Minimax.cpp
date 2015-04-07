//Minimax.cpp

#include "Minimax.h"

Move Minimax::RunMinimax(ChineseCheckersBoard &state, double timelimit){
	
	//Start the timer
	Minimax::timeout = false;
	std::thread* timer = new std::thread([&](){
		Timer t;
		while (t.getTime() < timelimit){
			//Spin
			std::this_thread::yield();
		}
		Minimax::timeout = true;
	});

	int depth = 1;
	while (true){
		MO move;
		if (Minimax::timeout)
			return move.m;
		else{
			
			depth ++;
		}
	}

}

float Minimax::Max(ChineseCheckersBoard &state, float(*eval)(ChineseCheckersBoard&),
					ushort depth, float alpha, float beta, MO &best_move)
{
	if (state.gameOver() || Minimax::timeout || depth == 0){
		best_move.score = (*eval)(state);
		return best_move.score;
	}
	else{
		std::set<Move> moves = state.getValidMoves();
		for (Move m : moves){
			state.applyMove(m);
			alpha = std::min(alpha, Minimax::Min(state, eval, ))
		}
	}
}

