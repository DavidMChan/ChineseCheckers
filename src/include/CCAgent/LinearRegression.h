/*
 *  LinearRegression.h
 *  hog2
 *
 *  Created by Nathan Sturtevant on 6/1/06.
 *  Copyright 2006 Nathan Sturtevant. All rights reserved.
 *
 */

#ifndef LinearRegression_H
#define LinearRegression_H

#include <vector>
#include <assert.h>
#include <iostream>

class LinearRegression {
public:
	LinearRegression(int inputs, double learnrate);
	void train(std::vector<double> &input, double output);
	double test(const std::vector<double> &input);
	void setRate(double alpha) { rate = alpha; }
	double getRate() { return rate; }
	void Print();
	const std::vector<double> &GetWeights() { return weights; }
	void SetWeights(const std::vector<double> &w)
	{assert(w.size() == weights.size()); weights = w; }
private:
	std::vector<double> weights;
	int inputs;
	double output;
	double rate;
};

#endif
