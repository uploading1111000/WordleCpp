#pragma once
#include <set>
#include <array>
#include "utils.h"
class ColourMatrix
{
public:
	ColourMatrix(Wordlist& words);
	Colours getColour(int answer, int guess) const;
	Colours getColour(Word answer, Word guess) const { return getColour(wordlist->getWordIndex(answer), wordlist->getWordIndex(guess)); };
	Wordlist* getWordlist() const { return wordlist; }
protected:
	std::vector<Colours> flatMatrix;
	Wordlist* wordlist;
	void generateMatrix();
	void saveMatrix();
	void loadMatrix();
};

class IndexMatrix
{
public:
	IndexMatrix(ColourMatrix& matrix);
	std::vector<int> getIndexSet(int guess, Colours colours) const {return matrix[guess][colours.asInd()];};
protected:
	std::vector<std::array<std::vector<int>,243>> matrix;
	Wordlist* wordlist;
	void generateMatrix(ColourMatrix& colourMatrix);
	void saveMatrix();
	void loadMatrix();
};
