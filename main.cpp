#include "utils.h"
#include "matrices.h"
#include <iostream>
#include <chrono>

int main() {
	Wordlist words("allowed_words.txt");
	std::cout << "Loaded " << words.size() << " words" << std::endl;

	auto start = std::chrono::high_resolution_clock::now();
	ColourMatrix matrix(words);
	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "Calculated full matrix in " << (float) duration.count() / 1000 << " seconds" << std::endl;
}