//Evalutaion Header

#ifndef EVAL_H_INCLUDED
#define EVAL_H_INCLUDED

#include "Move.h"
#include "ChineseCheckersBoard.h"


namespace Eval{
	//Evaluate a chinese checkers board (Player enum is ChineseCheckersBoard::Player::PLAYER1 or ChineseCheckersBoard::Player::PKAYER2)
	float Evaluate(ChineseCheckersBoard &state, ChineseCheckersBoard::Player player);
}

#endif
