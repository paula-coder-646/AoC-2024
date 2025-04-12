#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <cmath>
#include "day9.h"
#include <map>
#include <string>
#include <tuple>

// COnvert Line to List of Blocks (Blockid, Fileid, string)

static std::vector<int32_t> parseInput(std::string input)
{
	std::vector<int32_t> result;

	std::ifstream read(input);
	std::string line;
	std::getline(read, line);
	uint32_t blockcounter = 0;
	for (uint32_t i = 0; i < line.size();i++) {

		uint32_t num = line[i] - '0';

		if(!(i%2)) { // File
			for(uint32_t block = 0; block < (uint32_t) num; block++)
			{
				result.push_back(blockcounter);
			}
			blockcounter++;
		}
		if((i%2)) { // Free Space
			for(uint32_t block = 0; block < (uint32_t) num; block++)
			{
				result.push_back(-1);
			}
		}
	}
	return result;
}


std::vector<int32_t> shiftbits (const std::vector<int32_t> &input) {

	std::vector<int32_t> result = input;

	for (auto block = result.begin(); block < result.end(); block++) {
		if (*block == -1) { // Free Space
			while(*std::prev(result.end()) == -1) { // Remove trailing free space
				result.pop_back();
			}
			//
			if (block < result.end()) {
				int32_t lastelem = *std::prev(result.end()); // Swap last element to current free position
				result.pop_back();
				*block = lastelem;
			}
		}
	}

	while(*std::prev(result.end()) == -1) { // Remove trailing free space
		result.pop_back();
	}

	return result;
}

std::vector<int32_t> shiftbitsadvanced (const std::vector<int32_t> &input) {
	std::vector<int32_t> result = input;

	while(*std::prev(result.end()) == -1) { // Remove trailing free space
		result.pop_back();
	}

	for(auto block = result.end() - 1; block > result.begin();) {
		bool foundspace = false;
		auto currentblock = block;
		uint32_t currentblocksize = 0;
		uint32_t scheck = 0;
		while(*currentblock == *block) {
			currentblocksize++;
			currentblock--;
		}

		// search a space for the block
		for(auto searchit = result.begin(); searchit < block;) {
			if(!foundspace) {
				if (*searchit == -1) {
					scheck = 0;
					auto spaceit = searchit;
					while(*spaceit == -1) {
						scheck++;
						spaceit++;
					}
				}
				if (scheck >= currentblocksize) { // Copy File
					foundspace ^= true;
					int32_t tempblock = *block;
					for(uint32_t i = 0; i < currentblocksize; i++) {
						*(searchit+i) = tempblock; // Move Block
						*(block-i) = -1; // Free Old Location
					}
				}
				else searchit++;
			}
			else break;
		}
		block = currentblock; // Current Block pointed to last file bit
	}

	return result;
}

uint64_t checksum(std::vector<int32_t> &input) {
	uint64_t result = 0;
	for(uint64_t i = 0; i < input.size(); i++) {
		if(input[i] != -1)  {
			result += i * input[i];
		}
	}
	return result;
}

void partOne(std::vector<int32_t> &input) {
	auto defrag = shiftbits(input);
	uint64_t result = checksum(defrag);
	std::cout << "Part 1 result: "<< result << std::endl;
}

void partTwo(std::vector<int32_t> &input) {
	auto defrag = shiftbitsadvanced(input);
	uint64_t result = checksum(defrag);
	std::cout << "Part 2 result: "<< result << std::endl;
}


void day9_() {
	std::string input = "src/day9/realinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto disk = parseInput(input);

	partOne(disk);
	partTwo(disk);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
	}

