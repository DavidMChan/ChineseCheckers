//MiniMax.h

#ifndef MINIMAX_H_INCLUDED
#define MINIMAX_H_INCLUDED

#include <thread>
#include <atomic>
#include <cstdint>

#include "Move.h"
#include "Eval.h"
#include "timer.h"
#include "ChineseCheckersBoard.h"


namespace Minimax{
	typedef unsigned short ushort; 

	std::atomic<bool> timout;

	//Move object struct
	struct MO{
		Move m;
		float score;
	};

	Move RunMinimax(ChineseCheckersBoard &state, int timelimit); //Run with a given timelimit

	MO Minimax(ChineseCheckersBoard &state, ushort depth); //Run the minimax algorithm

	MO Max(ChineseCheckersBoard &state, ushort depth); //Max node 
	MO Min(ChineseCheckersBoard &state, ushort depth); //Min node


};

#endif
