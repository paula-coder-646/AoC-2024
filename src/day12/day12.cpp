#include "day12.h"

#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <utility>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <string>
#include <map>
#include <cmath>
#include <thread>
#include <mutex>
#include <format>
#include "boost/algorithm/string.hpp"

namespace std {
template<typename T1, typename T2>
struct hash<std::pair<T1, T2>> {
	std::size_t operator()(const std::pair<T1, T2> p) const {
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);
		return h1 ^ (h2 < 1);
	}
};
}

std::pair<uint32_t, uint32_t> operator+(std::pair<uint32_t, uint32_t> a, std::pair<uint32_t, uint32_t> b) {
	return {a.first + b.first, a.second + b.second};
}

struct Garden {
	std::vector<std::vector<bool>> visited;
	std::vector<std::string> fields;
	std::pair<uint32_t, uint32_t> dim;
};


static Garden parseInput(const std::string& input) {
	Garden result;
	std::ifstream read(input);
	std::vector<uint64_t> numbers;
	int x = 0;
	int y = 0;
	for(std::string line; getline(read, line);) {
		x = line.size();
		y++;
		result.fields.push_back(line);
	}
	result.visited = std::vector<std::vector<bool>>(y, std::vector<bool>(x, false));
	result.dim = {y, x};
	return result;
}

std::pair<uint32_t, uint32_t> patchFlood(Garden& input, int y, int x, char type) {

	// Bounds check
	if (x < 0 || x >= input.dim.first ||  y < 0 || y >= input.dim.second) {
		return {0,1};
	}

	if (input.visited[y][x] && input.fields[y][x] == type) {
		return {0,0};
	}

	if (input.fields[y][x] != type) {
		return {0,1};
	}

	uint32_t area = 1;
	uint32_t circum = 0;

	input.visited[y][x] = true;
	for (auto [dy, dx] : std::vector<std::pair<int, int>>{{0,1}, {1,0}, {0,-1}, {-1,0}}) {
		auto [na, nc] = patchFlood(input, y + dy, x + dx, type);
		area += na;
		circum += nc;
	}
	return {area, circum};
}

std::pair<int32_t, int32_t> patchEdge(Garden& input, int y, int x, char type) {

	// Since number of edges and faces in a closed polygon in 2D is the same, count edges

	if (y < 0 || y >= input.dim.first || x < 0 || x >= input.dim.second) {
		return {0,0};
	}

	if (input.visited[y][x] && input.fields[y][x] == type) {
		return {0,0};
	}

	if (input.fields[y][x] != type) {
		return {0,0};
	}

	auto edgecount = 0;
	auto area = 1;
	bool borderh = false;
	bool borderv = false;
	input.visited[y][x] = true;

	for (auto [dy, dx] : std::vector<std::pair<int, int>>{{-1,1}, {1,1}, {-1,-1}, {1,-1}}) {

		borderh = false;
		borderv = false;
		auto ny = y + dy;
		auto nx = x + dx;

		if (nx < 0 || nx >= input.dim.second) borderv = true;
		if (ny < 0 || ny >= input.dim.first) borderh = true;

		// Neighboring the Field Borders -> Natural Edge
		if (borderv && borderh) edgecount++;

		// Only if not at a field border, check for neighbor border
		if (!borderh) {
			if (input.fields[ny][x] != type) {
				// Again, either natural border or neighbor border counts
				if (borderv) edgecount++;
				else {
					if (input.fields[y][nx] != type) edgecount++;
				}
			}
		}

		// Last case, but analogous
		if (borderh) {
			if (!borderv) {
				if (input.fields[y][nx] != type) {
					edgecount++;
				}
			}
		}

		// A concave corner
		// Can only happen if we have +1 Room towards side, we're checking
		if (!(borderv || borderh)) {
			if (input.fields[ny][x] == type && input.fields[y][nx] == type && input.fields[ny][nx] != type) {
				edgecount++;
			}
		}
	}

	for (auto [dy, dx] : std::vector<std::pair<int, int>>{{-1,0}, {0,1}, {1,0}, {0,-1}}) {
		auto [narea, nedges] = patchEdge(input, y+dy, x+dx, type);
		area += narea;
		edgecount += nedges;
	}
	return {area, edgecount};
}

static void partOne (Garden& input) {
	uint32_t result = 0;
	for (int y = 0; y < input.dim.first; ++y) {
		for (int x = 0; x < input.dim.second; ++x) {
			if(input.visited[y][x]) continue;
			auto ptype = input.fields[y][x];
			auto [area, circ] = patchFlood(input, y, x, ptype);
			result += area * circ;
		}
	}
	std::cout << "The price for the whole Garden is: " << result << "\n";
	return;
}

static void partTwo (Garden& input) {
	uint32_t result = 0;
	for (int y = 0; y < input.dim.first; ++y) {
		for (int x = 0; x < input.dim.second; ++x) {
			if(input.visited[y][x]) continue;
			auto ptype = input.fields[y][x];
			auto [area, edges] = patchEdge(input, y, x, ptype);
			result += area * edges;
		}
	}
	std::cout << "The discounted price for the whole Garden is: " << result << "\n";
	return;
}



void day12_() {
	std::string input = "src/day12/realinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto Garden = parseInput(input);
	//partOne(Garden);
	partTwo(Garden);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
}

