#pragma once
#include "frequencies.h"
#include "matrices.h"
#include "utils.h"
class Optimiser
{
public:
	Optimiser(const std::string& WordlistPath, const std::string& FrequenciesPath);
	void maximiseEntropy2Word();
	float Entropy2Word(int first, int second);
	int stringIndex(const std::string& word) { return wordlist.getWordIndex(stringToWord(word)); };
private:
	Wordlist wordlist;
	ColourMatrix colourMatrix;
	IndexMatrix indexMatrix;
	Frequencies frequencies;

	std::array<Colours, 243> ALL_COLOURS;
};

