#pragma once
#include "utils.h"
class Frequencies
{
public:
	Frequencies(Wordlist& words, const std::string& filename);
	float getIndexFreq(const int& index) {return freqs[index];};
	float getWordFreq(const Word& word) {return freqs[wordlist->getWordIndex(word)];};
protected:
	Wordlist* wordlist;
	std::vector<float> freqs;
};

