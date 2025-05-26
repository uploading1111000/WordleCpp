#include "optimiser.h"
#include <iostream>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <thread>
#include <mutex>
#include <vector>
#include <atomic>


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

            min mm = min({ i, -1, INFINITY });
            std::array<std::vector<int>, 243>* firstGuessSet = reducedMatrix.getIndexGuessSetRef(i);
            
            for (int j = i + 1; j < total_words; j++) {
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

void Optimiser::test()
{
}
