#pragma once
#include <array>
#include <vector>
#include <string>

using Word = std::array<char, 5>;

Word stringToWord(const std::string& word);
std::string wordToString(const Word& word);

class Wordlist {
public:
	Wordlist(const std::string& filename);

	int getWordIndex(const Word& word) const;
	int getWordIndex(const std::string& word) const {return getWordIndex(stringToWord(word));};

	int size() const { return words.size(); }
	const Word& operator[](int index) const { return words[index]; }
protected:
	std::vector<Word> words;
	bool isless(const Word& lhs, const Word& rhs) const;
};

class Colours {
private:
	uint16_t data;
public:
	constexpr Colours() : data(0) {}
	constexpr Colours(std::array<int, 5> initial);
	constexpr void set(int index, int colour);
	constexpr int get(int index) const;
	std::string asString() const;
	constexpr uint16_t asInt() const { return data; }
	constexpr int asInd() const;

};

Colours findColours(const Word &answer, const Word &guess);

constexpr std::array<Colours, 243> generateAllColours();

inline constexpr auto ALL_COLOURS = generateAllColours();