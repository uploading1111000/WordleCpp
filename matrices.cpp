#include "matrices.h"
#include <fstream>
#include <filesystem>

ColourMatrix::ColourMatrix(Wordlist& words)
{
	wordlist = &words;

	if (std::filesystem::exists("cached_matrix.bin")) {
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
    std::ofstream outfile("cached_matrix.bin", std::ios::binary);

    size_t size = flatMatrix.size();
    outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));

    outfile.write(reinterpret_cast<const char*>(flatMatrix.data()), flatMatrix.size() * sizeof(Colours));

    outfile.close();
}


void ColourMatrix::loadMatrix() {
    std::ifstream infile("cached_matrix.bin", std::ios::binary);

    size_t size;
    infile.read(reinterpret_cast<char*>(&size), sizeof(size));
	if (size != wordlist->size() * wordlist->size()) {
		throw std::runtime_error("Cached matrix size does not match wordlist size");
	}

    flatMatrix.resize(size);
    infile.read(reinterpret_cast<char*>(flatMatrix.data()), size * sizeof(Colours));

    infile.close();
}

IndexMatrix::IndexMatrix(ColourMatrix& colourMatrix)
{
	wordlist = colourMatrix.getWordlist();
	if (std::filesystem::exists("index_matrix.bin")) {
		loadMatrix();
	}
	else {
		generateMatrix(colourMatrix);
		saveMatrix();
	}
	
}

void IndexMatrix::generateMatrix(ColourMatrix& colourMatrix)
{
	matrix.reserve(wordlist->size());
	for (int guess = 0; guess < wordlist->size(); guess++)
	{
		std::array<std::vector<int>, 243> sets;
		for (int answer = 0; answer < wordlist->size(); answer++)
		{
			Colours colours = colourMatrix.getColour(answer, guess);
			sets[colours.asInd()].push_back(answer);
		}
		matrix.push_back(sets);
	}
}

void IndexMatrix::saveMatrix()
{
	std::ofstream outfile("index_matrix.bin", std::ios::binary);

	size_t vectorSize = matrix.size();
	outfile.write(reinterpret_cast<const char*>(&vectorSize), sizeof(vectorSize));

	for (const auto& arrayOfSets : matrix) {
		for (const auto& set : arrayOfSets) {
			size_t setSize = set.size();
			outfile.write(reinterpret_cast<const char*>(&setSize), sizeof(setSize));

			outfile.write(reinterpret_cast<const char*>(set.data()), setSize * sizeof(int));
		}
	}
	outfile.close();
}

void IndexMatrix::loadMatrix()
{
	std::ifstream infile("index_matrix.bin", std::ios::binary);

	size_t vectorSize;
	infile.read(reinterpret_cast<char*>(&vectorSize), sizeof(vectorSize));

	matrix.resize(vectorSize);

	for (auto& arrayOfSets : matrix) {
		for (auto& set : arrayOfSets) {
			size_t setSize;
			infile.read(reinterpret_cast<char*>(&setSize), sizeof(setSize));

			set.resize(setSize);
			infile.read(reinterpret_cast<char*>(set.data()), setSize * sizeof(int));
		}
	}

	infile.close();
}
