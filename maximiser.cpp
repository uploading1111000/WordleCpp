#include "maximiser.h"

// Comparison operator for priority queue (larger entropy comes first for max-heap)
bool pair::operator<(const pair& other) const {
    return entropy < other.entropy;
}

// Constructor to set the maximum number of elements to keep
maximiser::maximiser(size_t n) : max_size(n) {}

// Add a new element, keeping only the N smallest
void maximiser::add(int entropy, int index) {
    pair p{entropy, index};
    
    if (pq.size() < max_size) {
        pq.push(p);
    } else if (entropy < pq.top().entropy) {
        pq.pop();
        pq.push(p);
    }
}

// Get all elements in descending order of entropy
std::vector<pair> maximiser::get_all() {
    std::vector<pair> result;
    while (!pq.empty()) {
        result.push_back(pq.top());
        pq.pop();
    }
    return result;
}
