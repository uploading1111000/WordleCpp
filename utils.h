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
	Colours() : data(0) {}
	Colours(std::array<int, 5> initial);;
	void set(int index, int colour);;
	int get(int index) const { return (data >> (index * 2)) & 3; };
	std::string asString() const;
	uint16_t asInt() const { return data; }
	inline int asInd() const { return get(0) + 3 * get(1) + 9 * get(2) + 27 * get(3) + 81 * get(4); };
};

Colours findColours(const Word &answer, const Word &guess);
Colours asColours(int ind);