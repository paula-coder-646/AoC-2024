#include "day2.h"
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <numeric>
#include <chrono>


// Only Links within this file
static std::vector<std::vector<int>> parseInput(std::string input) {
	std::vector<std::string> splitVec;
	std::vector<int> splitVecInt;
	std::vector<std::vector<int>> result;

	std::ifstream read(input);
	for(std::string line; std::getline(read, line);) {
		boost::split(splitVec, line, boost::is_any_of(" "), boost::token_compress_on);

		std::transform(splitVec.begin(), splitVec.end(), std::back_inserter(splitVecInt), [](std::string a) -> uint32_t {
			return std::stoi(a);
		});
		result.push_back(splitVecInt);
		splitVecInt.clear();
		splitVec.clear();
	}
	return result;
}

bool checkIsSafe(std::vector<int> input) {
	std::vector<int> differences;
	// Back Inserter automatically adds elements to the array if needed :D
	std::adjacent_difference(input.begin(), input.end(), std::back_inserter(differences));

	differences.erase(differences.begin());

	// Returns a pair of iterators (min, max) that point to the minimum and maximum element of the range
	auto minmax_iterators = std::minmax_element(differences.begin(), differences.end());

	if(*minmax_iterators.first > 0 and *minmax_iterators.second > 0 and *minmax_iterators.second <= 3) {
		return true;
	}
	if(*minmax_iterators.first < 0 and *minmax_iterators.second < 0 and *minmax_iterators.first >= -3) {
		return true;
	}
	return false;
}

void partOne(std::vector<std::vector<int>> input) {
	std::cout << std::count_if(input.begin(), input.end(), checkIsSafe) << std::endl;
}

void partTwo(std::vector<std::vector<int>> input) {
	std::vector<std::vector<int>> unsafe_levels;
	int damp_lvl = 0;
	std::for_each(input.begin(), input.end(), [&](std::vector<int> a){
		if (!checkIsSafe(a)) unsafe_levels.push_back(a);
	});

	for(uint32_t report = 0; report < unsafe_levels.size(); report++) {
		for(uint32_t lvl = 0; lvl < unsafe_levels[report].size(); lvl++) {
			std::vector<int> levelcopy = unsafe_levels[report];
			levelcopy.erase(levelcopy.begin() + lvl);
			if(checkIsSafe(levelcopy)) {
				damp_lvl++;
				break;
			}
		}
	}
	std::cout << std::count_if(input.begin(), input.end(), checkIsSafe) + damp_lvl<< std::endl;
}

void day2_ () {
	auto t1 = std::chrono::steady_clock::now();

	//std::string input = "src/day2/testinput.txt";
	std::string input = "src/day2/realinput.txt";
	std::vector<std::vector<int>> levels = parseInput(input);
	partOne(levels);
	partTwo(levels);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
}
