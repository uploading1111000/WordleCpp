#pragma once
#include <queue>
#include <vector>

struct pair {
	int entropy;
	int index;
	
	// Comparison operator for priority queue (larger entropy comes first for max-heap)
	bool operator<(const pair& other) const;
};


class maximiser
{
private:
	std::priority_queue<pair> pq;
	size_t max_size;

public:
	// Constructor to set the maximum number of elements to keep
	maximiser(size_t n);
	
	// Add a new element, keeping only the N smallest
	void add(int entropy, int index);
	
	// Get all elements in descending order of entropy
	std::vector<pair> get_all();
};

