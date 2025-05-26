#include "utils.h"
#include <fstream>
#include <iostream>
#include <string>

Word stringToWord(const std::string& word)
{
	Word result;
	for (int i = 0; i < 5; i++){
		result[i] = word[i];
	}
	return result;

}

std::string wordToString(const Word& word)
{
	std::string result;
	for (int i = 0; i < 5; i++){
		result += word[i];
	}
	return result;
}

Colours findColours(const Word &answer, const Word &guess)
{
	Word rAnswer = answer;
	Word rGuess = guess;
	std::vector<char> greens;
	std::vector<char> yellows;
	for (int i = 0; i < 5; i++){
		if (rAnswer[i] == rGuess[i]){
			greens.push_back(i);
			rAnswer[i] = 0;
			rGuess[i] = 0;
		}
	}
	for (int i = 0; i < 5; i++){
		if (rGuess[i] == 0){
			continue;
		}
		for (int j = 0; j < 5; j++){
			if (rAnswer[j] == rGuess[i]){
				yellows.push_back(i);
				rAnswer[j] = 0;
				rGuess[i] = 0;
				break;
			}
		}
	}
	Colours result;
	for (auto i : greens) {
		result.set(i, 2);
	}
	for (auto i : yellows) {
		result.set(i, 1);
	}
	return result;
}

constexpr std::array<Colours, 243> generateAllColours()
{
	std::array<Colours, 243> allColours;
	for (int index = 0; index < 243; index++) {
		allColours[index].set(0, index % 3);
		allColours[index].set(1, (index / 3) % 3);
		allColours[index].set(2, (index / 9) % 3);
		allColours[index].set(3, (index / 27) % 3);
		allColours[index].set(4, (index / 81) % 3);

	}
	return allColours;
}

Wordlist::Wordlist(const std::string& filename)
{
	std::ifstream file(filename);
	std::string line;
	while (std::getline(file, line)){
		words.push_back(stringToWord(line));
	}
	words.shrink_to_fit();
}

int Wordlist::getWordIndex(const Word& word) const
{
	//binary search
	int left = 0;
	int right = words.size() - 1;
	while (left <= right){
		int mid = (left + right) / 2;
		if (words[mid] == word){
			return mid;
		}
		else if (isless(words[mid], word)){
			left = mid + 1;
		}
		else{
			right = mid - 1;
		}
	}
	return -1;
}

bool Wordlist::isless(const Word& lhs, const Word& rhs) const
{
	for (int i = 0; i < 5; i++){
		if (lhs[i] < rhs[i]){
			return true;
		}
		else if (lhs[i] > rhs[i]){
			return false;
		}
	}
	return false;
}
constexpr Colours::Colours(std::array<int, 5> initial)
{
    data = 0;
    for (int i = 0; i < 5; i++) {
        set(i, initial[i]);
    }
}

constexpr void Colours::set(int index, int colour)
{
    uint16_t mask = 3 << (index * 2);
    data &= ~mask;
    data |= colour << (index * 2);
}

constexpr int Colours::get(int index) const
{
    return (data >> (index * 2)) & 3;
}

std::string Colours::asString() const
{
    std::string result;
    for (int i = 0; i < 5; i++) {
        switch (get(i)) {
        case 0:
            result += "b";
            break;
        case 1:
            result += "y";
            break;
        case 2:
            result += "g";
            break;
        }
    }
    return result;
}

constexpr int Colours::asInd() const
{
    return get(0) + 3 * get(1) + 9 * get(2) + 27 * get(3) + 81 * get(4);
}