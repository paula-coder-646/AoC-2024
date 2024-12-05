#include "day3.h"
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <numeric>
#include <chrono>
#include <regex>
#include <algorithm>

static std::string parseInput(std::string input) {
	std::ifstream read(input);
	std::string res;
	for (std::string line; std::getline(read, line);) {
		res += line;
	}
	return res;
}

void partOne (std::string input) {
	std::regex regpattern(R"(mul\((\d+),(\d+)\))");
	std::vector<std::pair<uint32_t, uint32_t>> matches;
	int result = 0;

	 std::transform(std::sregex_token_iterator(input.begin(), input.end(), regpattern, 1),
	              std::sregex_token_iterator(),
				  std::sregex_token_iterator(input.begin(), input.end(), regpattern, 2),
	              std::back_inserter(matches), [](std::string elem1, std::string elem2) -> std::pair<uint32_t, uint32_t> {
		 return std::make_pair(std::stoi(elem1), std::stoi(elem2));
	 });
	 result = std::transform_reduce(matches.begin(), matches.end(), 0, std::plus{}, [](std::pair<uint32_t, uint32_t> a) -> uint32_t {
		return a.first * a.second;
	 });
	 std::cout << result << std::endl;
}

void partTwo (std::string input) {
	std::regex instrucpattern(R"((mul\(\d+,\d+\)|do\(\)|don't\(\)))");
	std::regex multpattern(R"(mul\((\d+),(\d+)\))");
	int result = 0;

	std::vector<std::tuple<std::string, uint32_t, uint32_t>> instructions;

	std::transform(std::sregex_token_iterator(input.begin(), input.end(), instrucpattern, 1),
		              std::sregex_token_iterator(),
		              std::back_inserter(instructions), [multpattern](auto elem) -> std::tuple<std::string, uint32_t, uint32_t> {

		std::tuple<std::string, uint32_t, uint32_t> instruction;
		std::get<0>(instruction) = elem;

		std::smatch mult_match;

		if(std::regex_match(std::get<0>(instruction), mult_match, multpattern)) {
			std::get<1>(instruction) = std::stoi(mult_match[1]);
			std::get<2>(instruction) = std::stoi(mult_match[2]);
		}
		return instruction;
	});

	bool op_state = true;


	std::for_each(instructions.begin(), instructions.end(), [&](std::tuple<std::string, uint32_t, uint32_t> a){
		if (std::get<0>(a) == "don't()") {
			op_state = false;
		} else if (std::get<0>(a) == "do()") {
			op_state = true;
		} else if (op_state) {
			result += std::get<1>(a) * std::get<2>(a);
		}
	});

	std::cout << result << std::endl;

}
void day3_() {
	std::string input = "src/day3/realinput.txt";


	auto t1 = std::chrono::steady_clock::now();


	std::string operations = parseInput(input);

	partOne(operations);
	partTwo(operations);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
}
