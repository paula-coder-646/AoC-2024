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
#include "day8.h"
#include <map>

static std::pair<std::vector<std::string>, std::map<char, std::vector<Eigen::Vector2i>>> parseInput(std::string input)
{
	std::vector<std::string> antenna_field;
	 std::map<char, std::vector<Eigen::Vector2i>> antenna_map;

	std::ifstream read(input);
	for(std::string line; std::getline(read, line);)
	{
		antenna_field.push_back(line);
	}
	for(uint32_t y = 0; y < antenna_field.size(); y++) {
		for (uint32_t x = 0; x < antenna_field[y].size(); x++) {
			if(antenna_field[y][x] != '.') {
				antenna_map[antenna_field[y][x]].push_back(Eigen::Vector2i{x,y});
			}
		}
	}
	return std::make_pair(antenna_field, antenna_map);
}


void markAntinodes(std::vector<std::string> &field, std::map<char, std::vector<Eigen::Vector2i>> &antenna_positions, bool partTwo) {
	std::for_each(antenna_positions.begin(), antenna_positions.end(), [&](std::pair<char, std::vector<Eigen::Vector2i>> a) {
		// For all combinations of antennas
		for(auto fit = a.second.begin(); fit < a.second.end(); fit++) {
			for(auto sit = a.second.begin(); sit < a.second.end(); sit++) {
				if(fit == sit) continue;
				std::vector<Eigen::Vector2i> antinodes;
				Eigen::Vector2i antinode;
				Eigen::Vector2i distance = *sit - *fit;

				if(partTwo) {
					for(uint32_t i = 1; i < field.size(); i++) {
						antinode = *fit + i* distance;
						antinodes.push_back(antinode);
						antinode = *sit - i* distance;
						antinodes.push_back(antinode);
					}
				}
				else {
					antinode = *fit + 2* distance;
					antinodes.push_back(antinode);
					antinode = *sit - 2* distance;
					antinodes.push_back(antinode);
				}

				for(auto a : antinodes) {
					if (a.y() < (int)field.size() && a.y() >= 0 && a.x() < (int)field[a.y()].size() && a.x() >= 0) {
						field[a.y()][a.x()] = '#';
					}
				}
			}
		}
	});
	return;
}

void partOne(std::pair<std::vector<std::string>, std::map<char, std::vector<Eigen::Vector2i>>> &input) {
	markAntinodes(input.first, input.second, false);
	uint64_t result = std::transform_reduce(input.first.begin(), input.first.end(), (uint64_t) 0, std::plus{}, [](const std::string a) -> uint64_t {
		return std::count(a.begin(), a.end(), '#');
	});
	std::cout << "Part 1 result: "<< result << std::endl;
}

void partTwo(std::pair<std::vector<std::string>, std::map<char, std::vector<Eigen::Vector2i>>> &input) {
	markAntinodes(input.first, input.second, true);
	uint64_t result = std::transform_reduce(input.first.begin(), input.first.end(), (uint64_t) 0, std::plus{}, [](const std::string a) -> uint64_t {
		return std::count(a.begin(), a.end(), '#');
	});
	std::cout << "Part 2 result: "<< result << std::endl;
}


void day8_() {
	std::string input = "src/day8/realinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto antinodes = parseInput(input);

	//partOne(antinodes);
	partTwo(antinodes);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
	}


