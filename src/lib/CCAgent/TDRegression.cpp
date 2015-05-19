//
//  TDRegression.cpp
//  CC UCT
//
//  Created by Nathan Sturtevant on 4/18/15.
//  Copyright (c) 2015 Nathan Sturtevant. All rights reserved.
//

#include "CCAgent/TDRegression.h"
#include <iostream>
#include <algorithm>

#include <cstdlib>

double TDEval::eval(CCBoard &s, unsigned whichPlayer)
{

	int winner = s.winner();

	if (winner != -1){
		if (winner == whichPlayer){
			return 1000;
		} else {
			return -1000;
		}
	}

	GetFeatures(s, features, whichPlayer);
	double result = r.test(features);
	return result;
}

void TDEval::GetFeatures(const CCBoard &s, std::vector<double> &f, unsigned who)
{
	features.resize(0);
	features.resize(81 * 2);
	unsigned value;
	for (unsigned x = 0; x < 81; x++)
	{
		value = s.get(x);
		if (value == who)
		{
			if (who == 1)
				features[x] = 1;
			else
				features[80 - x] = 1;
		} else if (value != 0)
		{
			if (who == 1)
				features[81 + x] = 1;
			else
				features[81 + 80 - x] = 1;
		}
	}
	f = features;
}

bool TDEval::SetWeightsFromFile(std::string filename){
	std::ifstream input(filename);
	std::vector<double> weights(81 * 2 + 1);
	try{
		for (int i = 0; i < (81 * 2 + 1); i++)
			input >> weights.at(i);
	} catch ( ... ) {
		std::cerr << "FATAL ERROR - Failed to load weights from file" << std::endl;
		exit(1);
	}

	SetWeights(weights);
	return true;
}

bool TDEval::WriteWeightsToFile(std::string filename){
	std::ofstream output(filename);
	std::vector<double> w = r.GetWeights();
	try {
		for (auto i : w)
			output << i << std::endl;
	} catch ( ...  ){
		std::cerr << "FATAL ERROR - Failed to write weights to file" << std::endl;
		exit(1);
	}
	return true;
}

void TDRegression::GetFeatures(const CCBoard &s, std::vector<double> &features,
							   unsigned who)
{
	features.resize(0);
	features.resize(81 * 2);
	unsigned value;
	for (int x = 0; x < 81; x++)
	{
		value = s.get(x);
		if (value == who)
		{
			if (who == 1)
				features[x] = 1;
			else
				features[80 - x] = 1;
		} else if (value != 0)
		{
			if (who == 1)
				features[81 + x] = 1;
			else
				features[81 + 80 - x] = 1;
		}
	}
}

uint16_t TDRegression::GetNextAction(const CCBoard &s,
								 double &bestVal, double timeLimit, int)
{
	CCBoard state = s;
	double val = rand() % 100000;
	val /= 100000;
	if (val < epsilon) // select random move
		
	{
		return s.getRandomMove();
	}
	std::vector<uint16_t> moves;
	s.getMovesForward(moves);
	std::random_shuffle(moves.begin(), moves.end());

	uint16_t best = 0;
	bestVal = -1000;
	std::vector<double> features;
	for (uint16_t m : moves)
	{
		state.applyMove(m);
		GetFeatures(state, features, s.getPlayer());
		double value = r.test(features);
		if (value > bestVal && best == 0)
		{
			bestVal = value;
			best = m;
		}
		state.undoMove(m);
	}
	if (best == 0) // only sideways moves
	{
		return s.getRandomMove();
	}
	return best;
}

void TDRegression::Train(const std::vector<uint16_t> &trace)
{
	CCBoard s;
	for (auto m : trace)
	{
		s.applyMove(m);
	}

	// Game should be done at the end of the trace
	s.gameOver();

	assert(s.gameOver());
	int winner = s.winner();
	assert(winner != -1);
	
	std::vector<double> features;
	
	double winReward = 1.0;
	double loseReward = -1.0;
	
	// Step backwards through the code training with TD-Lambda
	for (int x = trace.size() - 1; x >= 0; x--)
	{
		
		s.undoMove(trace[x]);

		GetFeatures(s, features, s.getPlayer());

		if (static_cast<int>(s.getPlayer()) == winner){
			r.train(features, winReward);
			winReward = (1-lambda) * r.test(features) + lambda * winReward;
		} else {
			r.train(features, loseReward);	
			loseReward = (1-lambda) * r.test(features) + lambda * loseReward;
		}
	}
}

void TDRegression::Print()
{
	auto w = r.GetWeights();
	for (int x = 0; x < 2; x++)
	{
		if (x == 0)
			printf("  Us: ");
		if (x == 1)
			printf("Them: ");
		for (int y = 0; y < 81; y++)
		{
			printf("%1.3f ", w[y + 81 * x]);
		}
		printf("\n");
	}
}
