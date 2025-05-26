#pragma once
#include "frequencies.h"
#include "matrices.h"
#include "utils.h"
class Optimiser
{
public:
	Optimiser(const std::string &WordlistPath, const std::string &FrequenciesPath);
	void maximiseMutualInformation2Word();

private:
	Wordlist wordlist;
	ColourMatrix colourMatrix;
	IndexMatrix indexMatrix;
	Frequencies frequencies;
	float mutualInformation2Word(int first, int second);
};

