#pragma once
#include "frequencies.h"
#include "matrices.h"
#include "utils.h"
class Optimiser
{
public:
	Optimiser(const std::string& WordlistPath, const std::string& FrequenciesPath);
	void maximiseEntropy2Word();
	void maximiseEntropy2WordFaster();
	void maximiseEntropy2WordMultiThreaded();
	float Entropy2Word(int first, int second);
	int stringIndex(const std::string& word) { return wordlist.getWordIndex(stringToWord(word)); };
	void test();
private:
	Wordlist wordlist;
	ColourMatrix colourMatrix;
	IndexMatrix indexMatrix;
	Frequencies frequencies;
	ReducedMatrix reducedMatrix;

	std::array<Colours, 243> ALL_COLOURS;
};

