// -*- C++ -*-
//  TDRegression.h
//  CC UCT
//
//  Created by Nathan Sturtevant on 4/18/15.
//  Copyright (c) 2015 Nathan Sturtevant. All rights reserved.
//

#ifndef TD_REGRESSION_H
#define TD_REGRESSION_H

#include <stdio.h>
#include <assert.h>
#include <string>
#include <fstream>
#include <iostream>

#include "CCAgent/CCBitboard.h"
#include "CCAgent/LinearRegression.h"

class TDEval
{
public:
	TDEval() :r(81 * 2, 0.01) {}
	double eval(CCBoard &s, unsigned whichPlayer);
	const char *GetName() { return "TD"; }
	void SetWeights(const std::vector<double> &w) { r.SetWeights(w); }
	bool SetWeightsFromFile(std::string filename);
	bool WriteWeightsToFile(std::string filename);
private:
	void GetFeatures(const CCBoard &s, std::vector<double> &features, unsigned who);
	LinearRegression r;
	std::vector<double> features;
};

class TDRegression
{
public:
	TDRegression(double e, double l) : epsilon(e), lambda(l), r(81 * 2, 0.001) {}
	uint16_t GetNextAction(const CCBoard &s, double &,
					   double timeLimit, int depthLimit = 100000);
	void Train(const std::vector<uint16_t> &trace);
	void Print();
	const char *GetName() { return "TD-Regression"; }
	const std::vector<double> &GetWeights() { return r.GetWeights(); }
	
private:
	void GetFeatures(const CCBoard &s, std::vector<double> &features, unsigned who);
	double epsilon;
	double lambda;
	LinearRegression r;
};

#endif
