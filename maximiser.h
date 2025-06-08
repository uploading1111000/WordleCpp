#pragma once
#include <queue>
#include <vector>

struct pair {
	float entropy;
	int index;
	
	// Comparison operator for priority queue (larger entropy comes first for max-heap)
	bool operator<(const pair& other) const;
};


class Maximiser
{
private:
	std::priority_queue<pair> pq;
	size_t max_size;

public:
	// Constructor to set the maximum number of elements to keep
	Maximiser(int n);
	
	// Add a new element, keeping only the N smallest
	void add(float entropy, int index);
	
	// Get all elements in descending order of entropy
	std::vector<int> get_all();
};

