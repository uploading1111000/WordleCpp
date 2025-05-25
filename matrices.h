#pragma once
#include "utils.h"
class ColourMatrix
{
public:
	ColourMatrix(Wordlist& words);
	Colours getColour(int answer, int guess) const;
	Colours getColour(Word answer, Word guess) const { return getColour(wordlist->getWordIndex(answer), wordlist->getWordIndex(guess)); };
protected:
	std::vector<Colours> flatMatrix;
	Wordlist* wordlist;
	void generateMatrix();
	void saveMatrix();
	void loadMatrix();
};

