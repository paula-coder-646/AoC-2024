#include "day7.h"

#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <boost/algorithm/string.hpp>
#include <cmath>

static std::vector<std::pair<uint64_t, std::vector<uint64_t>>> parseInput(std::string input)
{
	std::vector<std::pair<uint64_t, std::vector<uint64_t>>> result;

	std::ifstream read(input);
	for(std::string line; std::getline(read, line);)
	{
		std::vector<std::string> tempresult;
		std::vector<std::string> tempsequence;
		std::vector<uint64_t> sequence;
		uint64_t num;

		boost::split(tempresult, line, boost::is_any_of(":"), boost::token_compress_on);
		boost::trim(tempresult[0]);
		num = std::stoull(tempresult[0]);
		boost::trim(tempresult[1]);
		boost::split(tempsequence, tempresult[1], boost::is_any_of(" "), boost::token_compress_on);
		std::transform(tempsequence.begin(), tempsequence.end(), std::back_inserter(sequence), [](std::string a) -> uint64_t {
			return std::stoull(a);
		});
		result.push_back(std::make_pair(num, sequence));
	}
	return result;
}

bool checkOperators (uint64_t checkresult, uint64_t tempresult, const std::vector<uint64_t> &input, uint64_t oplevel, bool partTwo) {
	if (oplevel >= input.size()){
		if (tempresult == checkresult) {
			return true;
		}
	}
	else {
		if (checkOperators (checkresult, tempresult + input.at(oplevel), input, oplevel+1, partTwo)) {
			return true;
		}
		if (checkOperators (checkresult, tempresult * input.at(oplevel), input, oplevel+1, partTwo)) {
			return true;
		}
		if (partTwo) {
			uint64_t num_digits = static_cast<uint64_t>(std::log10(input.at(oplevel))+1);
			tempresult = std::pow(10, num_digits)*tempresult + input.at(oplevel);
			if (checkOperators (checkresult, tempresult, input, oplevel+1, partTwo)) {
				return true;
			}
		}
	}
	return false;
}


void partOne(std::vector<std::pair<uint64_t, std::vector<uint64_t>>> &input) {
	uint64_t result = std::transform_reduce(input.begin(), input.end(), (uint64_t) 0, std::plus{}, [](const std::pair<uint64_t, std::vector<uint64_t>> &a) -> uint64_t {
		if (checkOperators(a.first, a.second[0], a.second, 1, false)) {
			return a.first;
		}
		return 0;
	});
	std::cout << "Part 1 result: "<< result << std::endl;
}

void partTwo(std::vector<std::pair<uint64_t, std::vector<uint64_t>>> &input) {
	uint64_t result = std::transform_reduce(input.begin(), input.end(), (uint64_t) 0, std::plus{}, [](const std::pair<uint64_t, std::vector<uint64_t>> &a) -> uint64_t {
		if (checkOperators(a.first, a.second[0], a.second, 1, true)) {
			return a.first;
		}
		return 0;
	});
	std::cout << "Part 1 result: "<< result << std::endl;
}


void day7_() {
	std::string input = "src/day7/realinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto sequences = parseInput(input);

	partOne(sequences);
	partTwo(sequences);


	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
	}


