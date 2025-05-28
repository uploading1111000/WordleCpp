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
	void maximiseEntropy2WordValidMultiThreaded();
	float Entropy2Word(int first, int second);
	float entropy1Index(int index);

	int stringIndex(const std::string& word) { return wordlist.getWordIndex(stringToWord(word)); };
	Word wordIndex(int index) const { return wordlist[index]; };

	void test();
	std::vector<int> ALL_WORDS() const;
	std::vector<int> POSSIBLE_WORDS() const;
	
	int minimiseEntropySet1Step(std::vector<int>& set);

private:
	Wordlist wordlist;
	ColourMatrix colourMatrix;
	IndexMatrix indexMatrix;
	Frequencies frequencies;
	ReducedMatrix reducedMatrix;

	std::array<Colours, 243> ALL_COLOURS;
};

