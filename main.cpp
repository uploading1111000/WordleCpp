#include "utils.h"
#include "matrices.h"
#include <iostream>
#include <set>
#include <chrono>
#include <algorithm>

int main() {
	Wordlist words("allowed_words.txt");
	std::cout << "Loaded " << words.size() << " words" << std::endl;

	ColourMatrix colourMatrix(words);
	std::cout << "Loaded colour matrix" << std::endl;

	auto initial = std::chrono::high_resolution_clock::now();
	IndexMatrix indexMatrix(colourMatrix);
	auto final = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(final - initial);
	std::cout << "Loaded index matrix in " << duration.count() << "ms" << std::endl;

	int dataA[5] = { 0,2,0,1,0 };
	Colours coloursA(dataA);
	std::set<int> setA = indexMatrix.getIndexSet(words.getWordIndex(stringToWord("crate")), coloursA);

	int dataB[5] = { 0,0,2,0,0 };
	Colours coloursB(dataB);
	std::set<int> setB = indexMatrix.getIndexSet(words.getWordIndex(stringToWord("soily")), coloursB);

	std::set<int> setC;
	std::set_intersection(setA.begin(), setA.end(), setB.begin(), setB.end(), std::inserter(setC, setC.begin()));

	for (int i : setC) {
		std::cout << wordToString(words[i]) << std::endl;
	}
}