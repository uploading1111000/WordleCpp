#include "optimiser.h"
#include <iostream>
#include <thread>

int main() {
	Optimiser optimiser("allowed_words.txt","five_letter_words.bin");
	//optimiser.maximiseEntropy2WordValidMultiThreaded();
	//optimiser.maximiseEntropy2WordMultiThreaded();
	//toile carns is the optimum first 2 steps of wordle (assuming you do not want to use data from the first step)
	std::cout << "Coast 1-step entropy: " << optimiser.entropy1Index(optimiser.stringIndex("coast")) << std::endl;
	std::cout << "Liner 1-step entropy: " << optimiser.entropy1Index(optimiser.stringIndex("liner")) << std::endl;
	std::cout << "Pesto 1-step entropy: " << optimiser.entropy1Index(optimiser.stringIndex("pesto")) << std::endl;
	std::vector<int> POSSIBLE_WORDS = optimiser.POSSIBLE_WORDS();
	int optimum = optimiser.minimiseEntropySet1Step(POSSIBLE_WORDS);
	std::cout << "Optimum first step word: " << wordToString(optimiser.wordIndex(optimum)) << std::endl;
	std::cout << "Entropy: " << optimiser.entropy1Index(optimum) << std::endl;
}