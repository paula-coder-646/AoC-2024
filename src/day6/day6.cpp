#include "day6.h"

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
#include <unordered_set>

#include <boost/algorithm/string.hpp>

#include <boost/functional/hash.hpp>

template<typename _scalar, int row, int col>
struct std::hash<Eigen::Matrix<_scalar, row, col>> {
	std::size_t operator()(const Eigen::Matrix<_scalar, row, col> &matrix) const noexcept {
		std::size_t res = 0;
		for(int i = 0; i < row; i++)
		{
			for(int j = 0; j < col; j++)
			{
				std::size_t h1 = std::hash<_scalar>{}(matrix(i,j));
				boost::hash_combine(res, h1);
			}
		}
		return res;
	}
};

static std::pair<Eigen::Vector2i, std::vector<std::string>> parseInput(std::string input)
{
	std::pair<Eigen::Vector2i, std::vector<std::string>> result;
	std::vector<std::string> playfield;
	Eigen::Vector2i guardposition;

	std::ifstream read(input);
	for(std::string line; std::getline(read, line);)
	{
		playfield.push_back(line);
	}
	// Find Guard Start Pos
	for(uint32_t row = 0; row < playfield.size(); row++) {
		for(uint32_t col = 0; col < playfield[row].size(); col++) {
			if (playfield[row][col] == '^') {
				guardposition = {col, row};
			}
		}
	}
	return std::make_pair(guardposition, playfield);
}

std::pair<std::unordered_set<Eigen::Vector2i>, bool> simulate (std::pair<Eigen::Vector2i, std::vector<std::string>> &input) {
	Eigen::Vector2i guard_direction{0, -1};
	Eigen::Vector2i guard_position = input.first;
	std::unordered_set<Eigen::Vector2i> visited_positions;
	int32_t rowcount = input.second.size() -1;
	int32_t colcount = input.second[0].size() - 1;
	const Eigen::Matrix2i right_rotation = Eigen::Rotation2D(M_PI/2).toRotationMatrix().cast<int>();
	uint32_t cycles = 10000;
	bool loop = false;
	while (true) {
		guard_position += guard_direction; //Next guard position

		// Check if Guard left playing field()
		if (guard_position.x() > colcount || guard_position.x() < 0 || guard_position.y() > rowcount || guard_position.y() < 0) {
			break;
		}
		// Hit obstacle ?
		else if (input.second[guard_position.y()][guard_position.x()] == '#') {
			guard_position -= guard_direction;
			guard_direction = right_rotation * guard_direction;
		}
		// Move forward
		else {
			visited_positions.insert(guard_position);
			input.second[guard_position.y()][guard_position.x()] = 'X';
			}
		cycles--;

		if(!cycles) {
			loop = true;
			break;
		}

	}
	return std::make_pair(visited_positions, loop);
}


void partOne(std::pair<Eigen::Vector2i, std::vector<std::string>> input) {
	int result = simulate(input).first.size() + 1;
	std::cout << "Part 1 result: "<< result << std::endl;
}

void partTwo(std::pair<Eigen::Vector2i, std::vector<std::string>> input) {
	auto originput = input;
	std::pair<std::unordered_set<Eigen::Vector2i>, bool> origresult = simulate(input);
	std::pair<std::unordered_set<Eigen::Vector2i>, bool> simresult;
	int result = std::transform_reduce(origresult.first.begin(), origresult.first.end(), 0, std::plus{}, [&input, &originput, &simresult](Eigen::Vector2i a) -> uint32_t {
		if(a == input.first) return 0;
		else {
			//auto siminput = originput;
			originput.second[a.y()][a.x()] = '#';
			simresult = simulate(originput);
			// If loop condition is met, increase count
			originput.second[a.y()][a.x()] = '.';
			return simresult.second;
		}
	});
	std::cout << "Part 2 result: "<< result << std::endl;
}


void day6_() {
	std::string input = "src/day6/livinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto guard = parseInput(input);

	partOne(guard);
	partTwo(guard);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
	}


