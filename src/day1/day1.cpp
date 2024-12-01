#include "day1.h"
#include <vector>
#include <string>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <numeric>
#include <chrono>

std::pair<std::vector<int>, std::vector<int>> parseInput(std::string input) {
	std::vector<int> lone;
	std::vector<int> ltwo;
	std::vector<std::string> splitVec;

	std::ifstream read(input);
	for (std::string line; std::getline(read,line);) {
		boost::split(splitVec, line, boost::is_any_of(" "), boost::token_compress_on);
		lone.push_back(std::stoi(splitVec.at(0)));
		ltwo.push_back(std::stoi(splitVec.at(1)));
	}
	return std::make_pair(lone, ltwo);
}

void partOne(std::pair<std::vector<int>, std::vector<int>> input) {
	int result = 0;

	std::sort(input.first.begin(), input.first.end());
	std::sort(input.second.begin(), input.second.end());

	result = std::transform_reduce(input.first.cbegin(),
			input.first.cend(), input.second.cbegin(), 0, std::plus{},
			[](int a, int b) -> int
			{
				return std::abs(a - b);
			});

	std::cout << result << std::endl;
}

// Learing Lambda Functions:
// [ ] Scope leer: Kein Scope / Keine externen Variablen
// [=] Pass by Value
// [&] Pass by Reference

void partTwo(std::pair<std::vector<int>, std::vector<int>> input) {
	std::unordered_map<int, int> occurences;
	int result = 0;
	std::for_each(input.second.begin(), input.second.end(),[&](int elem){
		occurences[elem]++;
	});
	std::sort(input.first.begin(), input.first.end());
	std::for_each(input.first.begin(), input.first.end(), [&](int elem){
		result += (elem * occurences[elem]);
	});
	std::cout << result << std::endl;
}


void day1_ () {
	auto t1 = std::chrono::steady_clock::now();

	//std::string input = "src/day1/testinput.txt";
	std::string input = "src/day1/realinput.txt";
	std::pair<std::vector<int>, std::vector<int>> lists = parseInput(input);
	partOne(lists);
	partTwo(lists);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
}
