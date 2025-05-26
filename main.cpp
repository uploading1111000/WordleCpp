#include "optimiser.h"
#include <iostream>

int main() {
	Optimiser optimiser("allowed_words.txt","five_letter_words.bin");
	optimiser.maximiseEntropy2Word();
}