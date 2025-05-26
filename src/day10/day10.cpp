#include "day10.h"

#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <utility>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <string>
#include <queue>
#include <map>
#include <boost/functional/hash.hpp>

template<>
struct std::hash<std::pair<uint32_t, uint32_t>> {
	std::size_t operator()(const std::pair<uint32_t, uint32_t>& vec) const noexcept {
		// Hash the two components (x and y)
		std::size_t hash1 = std::hash<int>{}(vec.first);
		std::size_t hash2 = std::hash<int>{}(vec.second);

		// Combine the hashes using XOR and bit-shifting
		boost::hash_combine(hash1, hash2);
		return hash1;
	}
};

struct Graph {

	std::unordered_map<uint32_t, std::vector<uint32_t>> graph;
	std::unordered_set<uint32_t> heads;
	std::unordered_set<uint32_t> peaks;
	std::pair<uint32_t, uint32_t> dim;
	std::unordered_map<uint32_t, int32_t> indegree;
	std::unordered_map<std::pair<uint32_t, uint32_t>, uint8_t> height;
	uint32_t reachable = 0;
};

static Graph parseInput(const std::string& input) {
	Graph result;
	uint32_t cols = 0;
	uint32_t y = 0;
	std::ifstream read(input);
	auto getID = [&](uint32_t x, uint32_t y){return y * cols + x;};
	for(std::string line; std::getline(read, line);) {
		for(uint32_t x = 0; x < line.size(); x++) {
			if(line[x] != '.') {
				cols = line.size();
				uint32_t nodeid = getID(x, y);
				if (std::stoul(std::string(1, line[x])) == 0) result.heads.insert(nodeid);
				if (std::stoul(std::string(1, line[x])) == 9) result.peaks.insert(nodeid);
				result.height[std::make_pair(x, y)] = std::stoul(std::string(1, line[x]));
				result.graph[nodeid];
			}
		}
		y++;
	}
	result.dim = std::make_pair(cols, y);
	return result;
}

void buildGraph(Graph &input) {
	std::vector<std::pair<int8_t, int8_t>> directions = {{1, 0},{-1,0},{0,1},{0,-1}};
	auto getNode = [&](std::pair<int8_t, int8_t> vec){return vec.second * input.dim.first + vec.first;};
	auto getVec = [&](uint32_t node) {return std::make_pair(node % input.dim.first, node / input.dim.first);};
	for (const std::pair<uint32_t, std::vector<uint32_t>> &nodepair : input.graph) {
		for(auto &[dx, dy] : directions) {
			int32_t neigx = getVec(nodepair.first).first + dx;
			int32_t neigy = getVec(nodepair.first).second + dy;
			std::pair<int32_t, int32_t> neigvec = std::make_pair(neigx, neigy);


			if (neigx >= (int)0 && neigx < (int)input.dim.first
					&& neigy >= (int)0 && neigy < (int) input.dim.second
					&& input.graph.count(getNode(neigvec))) {
				auto neighbor_height = input.height.at(neigvec);
				auto height = input.height[getVec(nodepair.first)];
				if ((neighbor_height - height) == 1) {
					auto neighbor_node = getNode(neigvec);
					input.graph[nodepair.first].push_back(neighbor_node);
				}
			}
		}
	}

	for (auto &[node, neighbors] : input.graph) {
		for(auto &neighbor : neighbors) {
			input.indegree[neighbor]++;
		}
	}
	return;
}

std::unordered_set<uint32_t> MiniBFS(Graph& input, uint32_t start = 0, bool reach = false) {
	std::unordered_set<uint32_t> result;
	std::queue<uint32_t> toVisit;
	std::unordered_set<uint32_t> visited;

	if (reach) {
		for (auto head : input.heads) {
			toVisit.push(head);
			result.insert(head);
		}
	} else toVisit.push(start);

	while(!toVisit.empty()) {
		uint32_t current = toVisit.front();
		toVisit.pop();
		visited.insert(current);
		for (const auto &neighbor : input.graph[current]) {
			if(!visited.count(neighbor)) {
				if (!input.peaks.count(neighbor)) {
					if (reach) result.insert(neighbor);
					toVisit.push(neighbor);
				} else {
					result.insert(neighbor);
				}
			}
		}
	}
	return result;
}

std::vector<uint32_t> topoSort(Graph& input) {
	std::vector<uint32_t> sorted;
	std::unordered_map<uint32_t, int32_t> indegree;
	std::queue<uint32_t> tosort;
	std::unordered_set<uint32_t> reachable = MiniBFS(input, 0, true);

	for (const auto& node : input.graph) {
		indegree[node.first];
	}
	for(const auto& node : reachable) {
		for(auto& neighbor : input.graph.at(node)) {
			indegree[neighbor]++;
		}
	}
	for(const auto& head : input.heads) {
		tosort.push(head);
	}

	while(!tosort.empty()) {
		auto current = tosort.front();
		tosort.pop();
		sorted.push_back(current);
		for (auto& neighbor : input.graph.at(current)) {
			indegree[neighbor]--;
			if(!indegree[neighbor]) {
				tosort.push(neighbor);
			}
		}
	}
	return sorted;
}

uint32_t countWays(Graph& input) {
	std::unordered_map<uint32_t, uint32_t> ways;
	std::vector<uint32_t> order = topoSort(input);

	for(const auto& head : input.heads) {
		/* if(head.first == 48) {
		(//	ways[head.first] = 1;// 2 4 20 38 42 45 48 54 57
		}*/
		ways[head] = 1;
	}

	for (auto node : order) {
		for (auto& neighbor : input.graph[node]) {
			ways[neighbor] += ways[node];
		}
	}
	return std::transform_reduce(ways.begin(), ways.end(), 0l, std::plus{}, [&](std::pair<uint32_t, uint32_t> pair){
		uint32_t result = 0;
		if (input.peaks.count(pair.first)) {
			result += pair.second;
		}
		return result;
	});
}
static void partOne(Graph& input){
	//For all trailheads find reachable peaks
	uint32_t result = std::transform_reduce(input.heads.begin(), input.heads.end(), 0, std::plus{}, [&](uint32_t head) -> uint32_t {
		return MiniBFS(input, head).size();
	});
	std::cout << "Number of reachable Peaks: " << result << std::endl;
	return;
}

static void partTwo(Graph& input) {
	uint32_t result = countWays(input);
	std::cout << "Number of reachable Peaks: " << result << std::endl;
}

void day10_() {
	std::string input = "src/day10/realinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto terrain = parseInput(input);
	buildGraph(terrain);
	partOne(terrain);
	partTwo(terrain);
	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
}

