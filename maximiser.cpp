#include "maximiser.h"
#include <iostream>

// Comparison operator for priority queue (larger entropy comes first for max-heap)
bool pair::operator<(const pair& other) const {
    return entropy < other.entropy;
}

// Constructor to set the maximum number of elements to keep
Maximiser::Maximiser(int n) : max_size(n) {}

// Add a new element, keeping only the N smallest
void Maximiser::add(float entropy, int index) {
    pair p{entropy, index};
    
    if (pq.size() < max_size) {
        pq.push(p);
    } else if (entropy < pq.top().entropy) {
        pq.pop();
        pq.push(p);
    }
}

// Get all elements in descending order of entropy
std::vector<int> Maximiser::get_all() {
    std::vector<int> result;
    while (!pq.empty()) {
        result.push_back(pq.top().index);
        pq.pop();
    }
    return result;
}
