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

	void sortByEntropy(std::vector<int>& set, std::vector<int>& returnable);

	std::vector<int> getLowestEntropyWords(std::vector<int>& set, int n);
	std::vector<int> getLowestEntropyWordsOptimized(std::vector<int>& set, int n);
	int bruteForceLowestExpectedValue(std::vector<int>& set, int n, int depth);
	int bruteForceSecondGuess(std::vector<int>& set, int n);

	int stringIndex(const std::string& word) { return wordlist.getWordIndex(stringToWord(word)); };
	Word wordIndex(int index) const { return wordlist[index]; };

	void maxWord2Prob();

	void test();
	void play();
	void precompute();

	std::vector<int> ALL_WORDS() const;
	std::vector<int> POSSIBLE_WORDS() const;
	
	int minimiseEntropySet1Step(std::vector<int>& set);
	int minimiseEntropySet1Step(std::vector<int>& set, std::vector<int>& options);

	void reverseSearch(std::vector<int> words, std::vector<Colours> data);

private:
	float bruteForceRecurseExpectation(std::vector<int>& set, int word, int n, int depth, float alpha);
	Wordlist wordlist;
	ColourMatrix colourMatrix;
	IndexMatrix indexMatrix;
	Frequencies frequencies;
	ReducedMatrix reducedMatrix;

	std::array<Colours, 243> ALL_COLOURS;
	int testN;
};

