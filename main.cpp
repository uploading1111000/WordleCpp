#include "optimiser.h"
#include <iostream>

int main() {
	Optimiser optimiser("allowed_words.txt","five_letter_words.bin");
	std::cout << "Carns 1-step entropy: " << optimiser.entropy1Index(optimiser.stringIndex("carns")) << std::endl;
	std::cout << "Toile 1-step entropy: " << optimiser.entropy1Index(optimiser.stringIndex("toile")) << std::endl;
	//optimiser.maximiseEntropy2WordMultiThreaded();
	//toile carns is the optimum first 2 steps of wordle (assuming you do not want to use data from the first step)
}