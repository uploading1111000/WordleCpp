#include "optimiser.h"
#include <iostream>
#include <thread>

int main() {
	Optimiser optimiser("allowed_words.txt","cheatlist.bin");
	optimiser.test();
	//optimiser.maximiseEntropy2WordValidMultiThreaded();
	//optimiser.maximiseEntropy2WordMultiThreaded();
	//toile carns is the optimum first 2 steps of wordle (assuming you do not want to use data from the first step)
	//std::cout << "Salon 1-step entropy: " << optimiser.entropy1Index(optimiser.stringIndex("salon")) << std::endl;
	//std::cout << "Trice 1-step entropy: " << optimiser.entropy1Index(optimiser.stringIndex("trice")) << std::endl;
	//std::cout << "Pesto 1-step entropy: " << optimiser.entropy1Index(optimiser.stringIndex("pesto")) << std::endl;*/
	//std::vector<int> POSSIBLE_WORDS = optimiser.POSSIBLE_WORDS();
	//int optimum = optimiser.minimiseEntropySet1Step(POSSIBLE_WORDS);
	
	//std::cout << "Optimum first step word: " << wordToString(optimiser.wordIndex(optimum)) << std::endl;
	//std::cout << "Entropy: " << optimiser.entropy1Index(optimum) << std::endl;
	//std::cout << "Scout 1-step entropy: " << optimiser.entropy1Index(optimiser.stringIndex("scout")) << std::endl;
	//std::cout << "Spout 1-step entropy: " << optimiser.entropy1Index(optimiser.stringIndex("spout")) << std::endl;
	//std::vector<int> words = { optimiser.stringIndex("polar"), optimiser.stringIndex("sport"), optimiser.stringIndex("drone"), optimiser.stringIndex("grift")};
	//std::vector<Colours> data = { Colours({1,1,0,0,0}), Colours({1,0,0,0,0}), Colours({1,0,1,0,2}), Colours({1,0,0,1,0})}; //Person A: "range"
	//Person B: "super"
	//Person C: "pesto"

	//std::vector<int> words = {  optimiser.stringIndex("grift"), optimiser.stringIndex("folio"), optimiser.stringIndex("alarm") };
	//std::vector<Colours> data = { Colours({0,0,0,0,2}), Colours({0,0,1,0,0}), Colours({0,0,0,0,0}) };
	//optimiser.reverseSearch(words, data);
	return 0;
}