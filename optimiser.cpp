#include "optimiser.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include <numeric>
#include "maximiser.h"
#include <fstream>


Optimiser::Optimiser(const std::string& WordlistPath, const std::string& FrequenciesPath)
    : wordlist(WordlistPath)
    , colourMatrix([&]() {
        std::cout << "Loaded " << wordlist.size() << " words\n";
        auto matrix = ColourMatrix(wordlist);
        std::cout << "Loaded colour matrix\n";
        return matrix;
    }())
    , indexMatrix([&]() {
        const auto initial = std::chrono::steady_clock::now();
        auto matrix = IndexMatrix(colourMatrix);
        const auto final = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(final - initial);
        std::cout << "Loaded index matrix in " << duration.count() << "ms\n";
        return matrix;
    }())
    , frequencies([&]() {
        auto freq = Frequencies(wordlist, FrequenciesPath);
        std::cout << "Loaded frequencies\n";
        return freq;
    }())
    , reducedMatrix([&]() {
    auto redMat = ReducedMatrix(indexMatrix, frequencies);
    std::cout << "Loaded reduced matrix\n";
    return redMat;
        }())
{
    for (int index = 0; index < 243; index++) {
        ALL_COLOURS[index].set(0, index % 3);
        ALL_COLOURS[index].set(1, (index / 3) % 3);
        ALL_COLOURS[index].set(2, (index / 9) % 3);
        ALL_COLOURS[index].set(3, (index / 27) % 3);
        ALL_COLOURS[index].set(4, (index / 81) % 3);
    }
    testN = 0;
}

struct min {
    int first;
    int second;
    float entropy;
};

void Optimiser::maximiseEntropy2Word()
{
    min m = min({ -1, -1, INFINITY });
    for (int i = 0; i < wordlist.size(); i++){
        min mm = min({ i, -1, INFINITY });
        for (int j = 0; j < wordlist.size(); j++){
            if (j == i) continue;
            float mi = Entropy2Word(i, j);
            if (mi < mm.entropy){
                mm.second = j;
                mm.entropy = mi;
            }
        }
        if (mm.entropy < m.entropy){
            m.first = mm.first;
            m.second = mm.second;
            m.entropy = mm.entropy;
        };
        std::cout << wordToString(wordlist[i]) << "(" << i << "/" << wordlist.size() << ") " << mm.entropy << " " << wordToString(wordlist[mm.second]) << "\n";
    }
    std::cout << "Min entropy: " << wordToString(wordlist[m.first]) << " " << wordToString(wordlist[m.second]) << " " << m.entropy << "\n";
}
void Optimiser::maximiseEntropy2WordMultiThreaded()
{
    min global_m = min({ -1, -1, INFINITY });
    std::mutex m_mutex;
    const unsigned int num_threads = std::thread::hardware_concurrency();
    std::atomic<int> current_i(0);  // Atomic counter for work distribution
    std::atomic<int> current_progress(0); //Atomic counter for progress
    const int total_words = wordlist.size();
    std::atomic<bool> should_continue{true};

    // Function to process a chunk of work
    auto thread_work = [&]() {
        min local_m = min({ -1, -1, INFINITY });
        
        while (should_continue) {
            // Atomically get and increment the counter
            int i = current_i++;
            
            if (i >= total_words) {
                break;
            }
            if (frequencies.getIndexFreq(i) == 0.0f) continue;
#

            min mm = min({ i, -1, INFINITY });
            std::array<std::vector<int>, 243>* firstGuessSet = reducedMatrix.getIndexGuessSetRef(i);
            
            for (int j = i + 1; j < total_words; j++) {
                if (frequencies.getIndexFreq(j) == 0.0f) continue;
                float mi = 0.0f;
                std::array<std::vector<int>, 243>* secondGuessSet = reducedMatrix.getIndexGuessSetRef(j);
                std::vector<int> workingSet;
                
                for (int index1 = 0; index1 < 243; index1++) {
                    for (int index2 = 0; index2 < 243; index2++) {
                        workingSet.clear();
                        std::set_intersection(
                            firstGuessSet->operator[](index1).begin(), firstGuessSet->operator[](index1).end(),
                            secondGuessSet->operator[](index2).begin(), secondGuessSet->operator[](index2).end(),
                            std::back_inserter(workingSet));
                        float p = frequencies.setProbability(workingSet);
                        if (p > 0.0f) {
                            mi += p * log2(p);
                        }
                    }
                }
                
                if (mi < mm.entropy) {
                    mm.second = j;
                    mm.entropy = mi;
                }
            }

            // Update local minimum if necessary
            if (mm.entropy < local_m.entropy) {
                local_m.first = mm.first;
                local_m.second = mm.second;
                local_m.entropy = mm.entropy;
            }

            // Thread-safe output
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                float progress = (float) current_progress++ / total_words;
                std::cout << wordToString(wordlist[i]) << "(" << i << "/" << total_words 
                         << ": " << progress * 100 * (2-progress) << "%) " 
                         << mm.entropy << " " << wordToString(wordlist[mm.second]) << "\n";
            }
        }

        // Update global minimum with thread's local minimum
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (local_m.entropy < global_m.entropy) {
                global_m = local_m;
            }
        }
    };

    // Launch threads
    std::vector<std::thread> threads;
    for (unsigned int t = 0; t < num_threads; t++) {
        threads.emplace_back(thread_work);
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Min entropy: " << wordToString(wordlist[global_m.first]) << " " 
              << wordToString(wordlist[global_m.second]) << " " << global_m.entropy << "\n";
}
void Optimiser::maximiseEntropy2WordValidMultiThreaded()
{
    min global_m = min({ -1, -1, INFINITY });
    std::mutex m_mutex;
    const unsigned int num_threads = std::thread::hardware_concurrency();
    std::atomic<int> current_i(0);  // Atomic counter for work distribution
    std::atomic<int> current_progress(0); //Atomic counter for progress
    const int total_words = wordlist.size();
    std::atomic<bool> should_continue{ true };

    // Function to process a chunk of work
    auto thread_work = [&]() {
        min local_m = min({ -1, -1, INFINITY });

        while (should_continue) {
            // Atomically get and increment the counter
            int i = current_i++;

            if (i >= total_words) {
                break;
            }
            if (frequencies.getIndexFreq(i) == 0.0f) continue;

            std::vector<int> workingSet;
            std::array<std::vector<int>, 243>* firstGuessSet = reducedMatrix.getIndexGuessSetRef(i);
            min mm = min({ i, -1, INFINITY });
            for (int index1 = 0; index1 < 243; index1++) {
                std::vector<int>* firstGuess = &firstGuessSet->operator[](index1);
                float totalP = frequencies.setProbability(*firstGuess);
                min mj = min({ i, -1, INFINITY });
                for (int j : *firstGuess) {
                    std::array<std::vector<int>, 243>* secondGuessSet = reducedMatrix.getIndexGuessSetRef(j);
                    float entropy = 0.0f;
                    for (int index2 = 0; index2 < 243; index2++) {
                        workingSet.clear();
                        std::set_intersection(
                            firstGuessSet->operator[](index1).begin(), firstGuessSet->operator[](index1).end(),
                            secondGuessSet->operator[](index2).begin(), secondGuessSet->operator[](index2).end(),
                            std::back_inserter(workingSet));
                        float p = frequencies.setProbability(workingSet) / totalP;
                        if (p > 0.0f) {
                            entropy += p * log2(p);
                        }
                    }
                    if (entropy < mj.entropy) {
                        mj.second = j;
                        mj.entropy = entropy;
                    }
                }
                if (mj.entropy < mm.entropy) {
                    mm.second = mj.second;
                    mm.entropy = mj.entropy;
                }
            }

            // Update local minimum if necessary
            if (mm.entropy < local_m.entropy) {
                local_m.first = mm.first;
                local_m.second = mm.second;
                local_m.entropy = mm.entropy;
            }

            // Thread-safe output
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                float progress = (float)current_progress++ / total_words;
                /*std::cout << wordToString(wordlist[i]) << "(" << i << "/" << total_words
                    << ": " << progress * 100 << "%) "
                    << mm.entropy << "\n";*/
            }
        }

        // Update global minimum with thread's local minimum
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (local_m.entropy < global_m.entropy) {
                global_m = local_m;
            }
        }
        };

    // Launch threads
    std::vector<std::thread> threads;
    for (unsigned int t = 0; t < num_threads; t++) {
        threads.emplace_back(thread_work);
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Min entropy: " << wordToString(wordlist[global_m.first]) << " "
        << wordToString(wordlist[global_m.second]) << " " << global_m.entropy << "\n";
}

void Optimiser::maximiseEntropy2WordFaster()
{
    min m = min({ -1, -1, INFINITY });
    for (int i = 0; i < wordlist.size(); i++) {
        min mm = min({ i, -1, INFINITY });
        std::array<std::vector<int>, 243>* firstGuessSet = indexMatrix.getIndexGuessSetRef(i);
        for (int j = i + 1; j < wordlist.size(); j++) {
            float mi = 0.0f;
            std::array<std::vector<int>, 243>* secondGuessSet = indexMatrix.getIndexGuessSetRef(j);
            std::vector<int> workingSet;
            for (int index1 = 0; index1 < 243; index1++) {
                for (int index2 = 0; index2 < 243; index2++) {
                    workingSet.clear();
                    std::set_intersection(firstGuessSet->operator[](index1).begin(), firstGuessSet->operator[](index1).end(), secondGuessSet->operator[](index2).begin(), secondGuessSet->operator[](index2).end(), std::back_inserter(workingSet));
                    float p = frequencies.setProbability(workingSet);
                    if (p > 0.0f) {
                        mi += p * log2(p);
                    }
                }
            }
            if (mi < mm.entropy) {
                mm.second = j;
                mm.entropy = mi;
            }
        }
        if (mm.entropy < m.entropy) {
            m.first = mm.first;
            m.second = mm.second;
            m.entropy = mm.entropy;
        };
        float progress = (i + 1) / (float)wordlist.size();
        std::cout << wordToString(wordlist[i]) << "(" << i << "/" << wordlist.size() << ": " << progress * 100 * (2-progress) << "%) " << mm.entropy << " " << wordToString(wordlist[mm.second]) << "\n";
        
    }
    std::cout << "Min entropy: " << wordToString(wordlist[m.first]) << " " << wordToString(wordlist[m.second]) << " " << m.entropy << "\n";
}


float Optimiser::Entropy2Word(int first, int second)
{
    float mi = 0.0f;
    std::vector<int> workingSet;
    std::vector<int>* setA = indexMatrix.getIndexSetRef(first, Colours());
    std::vector<int>* setB = indexMatrix.getIndexSetRef(second, Colours());
    for (auto colour1 : ALL_COLOURS) {
        setA = indexMatrix.getIndexSetRef(first, colour1);
        for (auto colour2 : ALL_COLOURS) {
            setB = indexMatrix.getIndexSetRef(second, colour2);
            workingSet.clear();
            std::set_intersection(setA->begin(), setA->end(), setB->begin(), setB->end(),std::back_inserter(workingSet));
            float p = frequencies.setProbability(workingSet);
            if (p > 0.0f) {
                mi += p * log2(p);
            }
        }
    }
    return mi;
}

float Optimiser::entropy1Index(int index)
{
    float mi = 0.0f;
    for (auto colour : ALL_COLOURS) {
        std::vector<int> set;
        indexMatrix.getIndexSet(index, colour,&set);
        float p = frequencies.setProbability(set);
        if (p > 0.0f) {
            mi += p * log2(p);
        }
    }
    return mi;
}

void Optimiser::sortByEntropy(std::vector<int>& set, std::vector<int>& returnable)
{
    Maximiser Max(set.size());
    for (int i : set) {
        Max.add(entropy1Index(i), i);
    }
    returnable = Max.get_all();
    std::reverse(returnable.begin(), returnable.end());
}

std::vector<int> Optimiser::getLowestEntropyWords(std::vector<int>& set, int n)
{
    Maximiser Max(n);
    for (int i = 0; i < wordlist.size(); i++) {
        std::array<std::vector<int>, 243>* setA = reducedMatrix.getIndexGuessSetRef(i);
        float entropy = 0.0f;
        std::vector<int> workingSet;
        for (int colourI = 0; colourI < 243; colourI++) {
            workingSet.clear();
            std::set_intersection(set.begin(), set.end(), setA->operator[](colourI).begin(), setA->operator[](colourI).end(), std::back_inserter(workingSet));
            float p = frequencies.setProbability(workingSet);
            if (p > 0.0f) {
                entropy += p * log2(p);
            }
        }
        Max.add(entropy,i);
    }
    return Max.get_all();
}

std::vector<int> Optimiser::getLowestEntropyWordsOptimized(std::vector<int>& set, int n)
{
    Maximiser Max(n);
    for (int guess = 0; guess < wordlist.size(); guess++) {
        float entropy = 0.0f;
        std::vector<int> workingSet;
        std::array<std::vector<int>, 243> byColours;
        for (int answer : set) {
            Colours colour = colourMatrix.getColour(answer,guess);
            byColours[colour.asInd()].push_back(answer);
        }
        for (int colourI = 0; colourI < 243; colourI++) {
            float p = frequencies.setProbability(byColours[colourI]);
            if (p > 0.0f) {
                entropy += p * log2(p);
            }
        }
        Max.add(entropy, guess);
    }
    return Max.get_all();
}

int Optimiser::bruteForceLowestExpectedValue(std::vector<int>& set, int n, int depth)
{
    std::vector<int> wordsToTest = getLowestEntropyWordsOptimized(set, n);
    std::reverse(wordsToTest.begin(), wordsToTest.end());
    if (depth != 0) wordsToTest.insert(wordsToTest.end(), set.begin(), set.end());
    struct expMin {
        int index;
        float expectation;
    };
    expMin m = expMin({ -1, INFINITY });
    for (int word : wordsToTest) {
        float expectation = bruteForceRecurseExpectation(set,word, n, depth, m.expectation);
        if (expectation < m.expectation) {
            m.index = word;
            m.expectation = expectation;
        }
    }
    //std::cout << wordToString(wordlist[m.index]) << " " << m.expectation << std::endl;
    return m.index;
}

int Optimiser::bruteForceSecondGuess(std::vector<int>& set, int n)
{
    std::vector<int> wordsToTest;
    sortByEntropy(set, wordsToTest);
    struct expMin {
        int index;
        float expectation;
    };
    expMin m = expMin({ -1, INFINITY });
    for (int word : wordsToTest) {
        float expectation = bruteForceRecurseExpectation(set, word, n, 1, m.expectation);
        if (expectation < m.expectation) {
            m.index = word;
            m.expectation = expectation;
        }
    }
    return m.index;
}

float Optimiser::bruteForceRecurseExpectation(std::vector<int>& set, int word, int n, int depth, float alpha)
{
    if (depth == 6) {
        return INFINITY;
    }
    float totalProb = set.size();
    float E = 0.0f;
    std::array<std::vector<int>, 243> byColours;
    for (int answer : set) {
        Colours colour = colourMatrix.getColour(answer, word);
        byColours[colour.asInd()].push_back(answer);
    }
    for (int I = 0; I < 242; I++) { //242 as 243 corresponds to being correct i.e. 0 expectation
        
        if (byColours[I].size() == 0) {
            continue;
        }
        float p = byColours[I].size() / totalProb;
        if (byColours[I].size() == 1) {
            E += p;
        }
        else if (byColours[I].size() == 2) {
            E += 1.5f * p;
        }
        else {
            std::vector<int> EntropyWords = getLowestEntropyWordsOptimized(byColours[I], std::min(n,int(byColours[I].size())));

            float minExp = INFINITY;
            if (byColours[I].size() <= 9) { //very small sets then it makes sense to go for gold before checking for information
                for (int newWord : byColours[I]) {
                    float expectation = bruteForceRecurseExpectation(byColours[I], newWord, n, depth + 1, minExp);
                    if (expectation < minExp) {
                        minExp = expectation;
                    }
                }
            }
            for (int newWord : EntropyWords) {
                float expectation = bruteForceRecurseExpectation(byColours[I], newWord, n, depth + 1, minExp);
                if (expectation < minExp) {
                    minExp = expectation;
                }
            }
            /*if (byColours[I].size() > 9 and byColours[I].size() <= 33) { //small sets it makes sense to check if theres any impact by guessing, but do it after the (in most cases better) information plays
                for (int newWord : byColours[I]) {
                    float expectation = bruteForceRecurseExpectation(byColours[I], newWord, n, depth + 1, minExp);
                    if (expectation < minExp) {
                        minExp = expectation;
                    }
                }
            }*/
            E += p * (1 + minExp);
        }
        //if (E >= alpha) {
            //if (depth == 1) {
                //std::cout << testN++ << "\n";
            //}
            //return E;
        //}
    }
    //if (depth == 1) {
        //std::cout << testN++ << "\n";
    //}
    return E;
}

void Optimiser::maxWord2Prob()
{
    //P(getting it on 2) = sum(P(colour) * P(getting it on 2 | colour)) over colours  with P(colour) > 0
    // =sum((Number of words with colour / Total words) * (1 / Number of words with colour))
    // =sum(1/Number of words with colour) i.e. const, maximised by finding word that produces as many colours with P > 0
    Maximiser Max(10);
    std::vector<int> POSSIBLE = POSSIBLE_WORDS();
    std::vector<int> scores;
    scores.reserve(wordlist.size());
    for (int i = 0; i < wordlist.size(); i++) {
        std::array<bool, 243> hasColour = { false };
        for (int j = 0; j < POSSIBLE.size(); j++) {
            Colours colour = colourMatrix.getColour(POSSIBLE[j], i);
            hasColour[colour.asInd()] = true;
        }
        int count = std::count(hasColour.begin(), hasColour.end(), true);
        scores.push_back(count);
        Max.add(-count, i);
    }
    std::vector<int> maxWords = Max.get_all();
    std::reverse(maxWords.begin(), maxWords.end());
    for (int i : maxWords) {
        std::cout << wordToString(wordlist[i]) << " " << scores[i] << "\n";
    }
    std::cout << scores[stringIndex("pesto")] << "\n";
    std::cout << scores[stringIndex("super")] << "\n";
}

void Optimiser::test()
{
    std::vector<int> setA;
    setA = *reducedMatrix.getIndexSetRef(stringIndex("trice"), Colours({0,1,0,0,2}));
    std::vector<int> setB;
    setB = *reducedMatrix.getIndexSetRef(stringIndex("sargo"), Colours({ 1,0,1,0,0 }));
    std::vector<int> workingSet;
    std::set_intersection(setA.begin(), setA.end(), setB.begin(), setB.end(), std::back_inserter(workingSet));
    std::cout << "Valid:" << std::endl;
    for (int C : workingSet) {
        std::cout << wordToString(wordlist[C]) << "\n";
    }
    std::cout << "Optimal:" << std::endl;
    int optimal = minimiseEntropySet1Step(workingSet);
    std::cout << wordToString(wordlist[optimal]) << "\n";
}

void Optimiser::play()
{
    /*std::vector<int> initial = POSSIBLE_WORDS();
    std::vector<int> guesses = {stringIndex("trace"),stringIndex("slain"),stringIndex("flail") };
    std::vector<Colours> data = { Colours({ 0, 0, 2, 0, 0 }),Colours({ 0, 2, 2, 2, 0 }),Colours({ 0, 2, 2, 2, 0 }) };
    for (int i = 0; i < guesses.size(); i++) {
        std::vector<int> set;
        indexMatrix.getIndexSet(guesses[i], data[i], &set);
        std::vector<int> newSet;
        std::set_intersection(initial.begin(), initial.end(), set.begin(), set.end(), std::back_inserter(newSet));
        initial = newSet;
    }
    bruteForceLowestExpectedValue(initial, 100, guesses.size());*/
    /*std::vector<int> set;
    indexMatrix.getIndexSet(stringIndex("trace"), Colours({ 0,0,2,0,0 }), &set);
    std::vector<int> newSet;
    std::set_intersection(initial.begin(), initial.end(), set.begin(), set.end(), std::back_inserter(newSet));
    int optimum = minimiseEntropySet1Step(newSet, newSet);
    std::cout << wordToString(wordlist[optimum]) << "\n";*/
    std::ifstream infile("second_guesses.bin", std::ios::binary);
    std::array<int, 243> bestGuesses;
    infile.read(reinterpret_cast<char*>(&bestGuesses), sizeof(bestGuesses));
    std::cout << "trace" << std::endl;
    std::string rawInput;
    std::getline(std::cin, rawInput);
    Colours input;
    for (int i = 0; i < 5; i++) {
        input.set(i, rawInput[i] - '0');
    }
    std::vector<int>* firstSet = reducedMatrix.getIndexSetRef(stringIndex("trace"), input);
    std::vector<int> initial = *firstSet;
    int guess = bestGuesses[input.asInd()];
    std::cout << wordToString(wordlist[guess]) << std::endl;
    std::vector<int> newSet;
    while (true) {
        std::getline(std::cin, rawInput);
        for (int i = 0; i < 5; i++) {
            input.set(i, rawInput[i] - '0');
        }
        firstSet = reducedMatrix.getIndexSetRef(guess, input);
        newSet.clear();
        std::set_intersection(initial.begin(), initial.end(), firstSet->begin(), firstSet->end(), std::back_inserter(newSet));
        initial = newSet;
        if (initial.size() == 1) {
            std::cout << "Answer:" << wordToString(wordlist[initial[0]]) << std::endl;
            return;
        }
        else if (initial.size() == 0) {
            std::cout << "No possible words" << std::endl;
            return;
        }
        guess = bruteForceLowestExpectedValue(initial, 100, 2);
        std::cout << wordToString(wordlist[guess]) << std::endl;

    }
}

void Optimiser::precompute()
{
    std::array<std::vector<int>,243> guessSet = *reducedMatrix.getIndexGuessSetRef(stringIndex("trace"));
    std::array<int, 243> bestGuesses;
    for (int i = 0; i < 243; i++) {
        std::vector<int> initial = guessSet[i];
        bestGuesses[i] = bruteForceSecondGuess(initial, 50);
    }
    std::ofstream outfile("second_guesses.bin", std::ios::binary);
    outfile.write(reinterpret_cast<const char*>(&bestGuesses), sizeof(bestGuesses));
    outfile.close();
}

std::vector<int> Optimiser::ALL_WORDS() const
{
    std::vector<int> words(wordlist.size()); 
    std::iota(words.begin(), words.end(), 0);
    return words;
}

std::vector<int> Optimiser::POSSIBLE_WORDS() const
{
    std::vector<int> words;
    words.reserve(2601);
    for (int i = 0; i < wordlist.size(); i++) {
        if (frequencies.getIndexFreq(i) > 0.0f) {
            words.push_back(i);
        }
    }
    return words;
}

int Optimiser::minimiseEntropySet1Step(std::vector<int>& set)
{
    min m = min({ -1, -1, INFINITY });
    std::vector<int> workingSet;
    float reducedP = frequencies.setProbability(set);
    for (int index = 0; index < wordlist.size(); index++) {
        std::array<std::vector<int>, 243>* firstGuessSet = indexMatrix.getIndexGuessSetRef(index);
        float entropy = 0.0f;
        for (int colourI = 0; colourI < 243; colourI++) {
            std::vector<int>* newSet = &firstGuessSet->operator[](colourI);
            workingSet.clear();
            std::set_intersection(set.begin(), set.end(), newSet->begin(), newSet->end(), std::back_inserter(workingSet));
            float p = frequencies.setProbability(workingSet);
            if (p > 0.0f) {
                entropy += p * log2(p);
            }
        }
        if (entropy < m.entropy) {
            m.first = index;
            m.entropy = entropy;
        }
    }
    return m.first;
}

int Optimiser::minimiseEntropySet1Step(std::vector<int>& set, std::vector<int>& options)
{
    min m = min({ -1, -1, INFINITY });
    std::vector<int> workingSet;
    float reducedP = frequencies.setProbability(set);
    for (int index: options) {
        std::array<std::vector<int>, 243>* firstGuessSet = indexMatrix.getIndexGuessSetRef(index);
        float entropy = 0.0f;
        for (int colourI = 0; colourI < 243; colourI++) {
            std::vector<int>* newSet = &firstGuessSet->operator[](colourI);
            workingSet.clear();
            std::set_intersection(set.begin(), set.end(), newSet->begin(), newSet->end(), std::back_inserter(workingSet));
            float p = frequencies.setProbability(workingSet);
            if (p > 0.0f) {
                entropy += p * log2(p);
            }
        }
        if (entropy < m.entropy) {
            m.first = index;
            m.entropy = entropy;
        }
    }
    return m.first;
}

void Optimiser::reverseSearch(std::vector<int> words, std::vector<Colours> data)
{
    for (int i = 0; i < wordlist.size(); i++) {
        bool valid = true;
        for (int j = 0; j < words.size(); j++) {
            if (colourMatrix.getColour(words[j], i).asInt() != data[j].asInt()) {
                valid = false;
                break;
            }
        }
        if (valid) {
            std::cout << wordToString(wordlist[i]) << "\n";
        }
    }
}


