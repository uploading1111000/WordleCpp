#include "matrices.h"
#include <fstream>
#include <filesystem>

ColourMatrix::ColourMatrix(Wordlist& words)
{
	wordlist = &words;

	if (std::filesystem::exists("cached_matrix.txt")) {
		loadMatrix();
	}
	else {
		generateMatrix();
		saveMatrix();
	}
}

Colours ColourMatrix::getColour(int answer, int guess) const
{
	int index = answer * wordlist->size() + guess;
	return flatMatrix[index];
}

void ColourMatrix::generateMatrix()
{
	flatMatrix.reserve(wordlist->size() * wordlist->size());
	for (int answer = 0; answer < wordlist->size(); answer++)
	{
		for (int guess = 0; guess < wordlist->size(); guess++)
		{
			flatMatrix.push_back(findColours((*wordlist)[answer], (*wordlist)[guess]));
		}
	}
}

void ColourMatrix::saveMatrix() {
    std::ofstream outfile("cached_matrix.txt", std::ios::binary);

    size_t size = flatMatrix.size();
    outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));

    outfile.write(reinterpret_cast<const char*>(flatMatrix.data()), flatMatrix.size() * sizeof(Colours));

    outfile.close();
}


void ColourMatrix::loadMatrix() {
    std::ifstream infile("cached_matrix.txt", std::ios::binary);

    size_t size;
    infile.read(reinterpret_cast<char*>(&size), sizeof(size));
	if (size != wordlist->size() * wordlist->size()) {
		throw std::runtime_error("Cached matrix size does not match wordlist size");
	}

    flatMatrix.resize(size);
    infile.read(reinterpret_cast<char*>(flatMatrix.data()), size * sizeof(Colours));

    infile.close();
}

