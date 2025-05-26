#include "day14.h"

#include <chrono>
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <string>
#include "boost/algorithm/string.hpp"
#include "regex"
#include <unordered_set>
#include <cmath>
#include <iostream>
#include <numeric>
#include <eigen3/Eigen/Dense>
#include <optional>

// We know the gridsize is < 128 so 2⁷ = 128 bit-shifting is enough
namespace std {
template<typename T1, typename T2>
struct hash<std::pair<T1, T2>> {
	std::size_t operator()(const std::pair<T1, T2> p) const {
		return static_cast<size_t>(p.first) << 7 | static_cast<size_t>(p.second);
	}
};
}

typedef std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> robots;
typedef std::pair<std::pair<int, int>, std::pair<int, int>> robot;
typedef std::unordered_map<std::pair<int, int>, int> robopos;


static robots parseInput(const std::string& inputFile) {
    robots result;
    std::ifstream file(inputFile);
    std::regex pattern(R"(p=(-?\d+),(-?\d+)\s+v=(-?\d+),(-?\d+))");
;

    for (std::string line; std::getline(file, line);) {
        std::smatch match;
        if (std::regex_search(line, match, pattern)) {
            int x = std::stoi(match[1]);
            int y = std::stoi(match[2]);
            int vx = std::stoi(match[3]);
			int vy = std::stoi(match[4]);

			result.emplace_back(std::make_pair(y, x), std::make_pair(vy, vx));
        }
    }
    return result;
}


void plotTree(robots& input, std::pair<int, int> dim, int iteration) {
	std::vector<std::string> plot(dim.first, std::string(dim.second, '.'));
	std::vector<std::pair<int, int>> positions;

	for (robot robot : input){
		int y = ((robot.first.first + robot.second.first * iteration) % static_cast<int>(dim.first) + dim.first) % dim.first;
		int x = ((robot.first.second + robot.second.second * iteration) % static_cast<int>(dim.second) + dim.second) % dim.second;
		positions.push_back({y, x});
	}

	for(const auto& robot : positions) {
		plot[robot.first][robot.second] = '*';
	}

	for(const auto& row : plot) {
		std::cout << row << "\n";
	}
}



double Entropy(robopos& input) {
	// Guess that the max amount of Robots per Square will be 256
	std::vector<float> histogram(256, 0.0);
	int robocount = input.size();
	float entropy = 0.0;

	for(const auto& [pos, count] : input) {
		histogram[count]++;
	}

	for (float& count : histogram) {
		if (count == 0) continue;
		float p = count / robocount;
		entropy -= p * std::log2(p);
	}

	return entropy;
}

static void partOne (robots& input) {
	std::pair<uint32_t, uint32_t> dim = {103, 101};
	int timesteps = 100;
	
	std::vector<uint32_t> quadrants(4, 0);
	int safety = 0;
	for (robot robot : input){
		int quadres = 0b0;

		// Modulo needs to be of type int as x can x + vx can get negative -> casting 
		int y = ((robot.first.first + robot.second.first * timesteps) % static_cast<int>(dim.first) + dim.first) % dim.first;
		int x = ((robot.first.second + robot.second.second * timesteps) % static_cast<int>(dim.second) + dim.second) % dim.second;

		int dy = static_cast<int>((dim.first - 1) / 2);
		int dx = static_cast<int>((dim.second - 1) / 2);
		if (y > dy) quadres |= 0b10;
		if (x > dx) quadres |= 0b01;

		if (y == (dim.first/2) || x == (dim.second/2) ) continue;// Don't count along the center line
		quadrants.at(quadres)++; // Add robot to the right quadrant
	}

	safety = std::reduce(quadrants.begin(), quadrants.end(), uint32_t(1), std::multiplies{});
	std::cout << "The bathroom safety factor is: " << safety << "\n";
	return;
}


static void partTwo(robots& input) {
	std::pair<int, int> dim = {103, 101};
	std::vector<double> entropy;
	robots orig_input = input;
	int iterations = dim.first * dim.second;
	int result;

	// split position and velocities
	for (int i = 0; i < iterations; i++) {
		robopos state;
		state.reserve(input.size());

		for (robot& robot : input) {
			robot.first.first = ((robot.first.first + robot.second.first) % dim.first + dim.first) % dim.first;
			robot.first.second = ((robot.first.second + robot.second.second) % dim.second + dim.second) % dim.second;
			state[{robot.first.first, robot.first.second}]++;
		}
		entropy.push_back(Entropy(state));
	}
	result = std::min_element(entropy.begin(), entropy.end()) - entropy.begin();
	// The final state 
	std::cout << "Iterations till Easter Egg: " << 1 + result << " \n";
	std::cout << "The Tree: \n";
	plotTree(orig_input, dim, result + 1);
}


void day14_() {
	std::string input = "src/day14/realinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto robots = parseInput(input);
	partOne(robots);
	partTwo(robots);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
}

