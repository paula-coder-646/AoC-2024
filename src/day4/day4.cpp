#include "day4.h"
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <numeric>
#include <chrono>
#include <regex>
#include <algorithm>




static std::vector<std::vector<std::string>> parseInput(std::string input) {
	std::vector<std::vector<std::string>> result;
	std::vector<std::string> view;
	std::ifstream read(input);
	// Horizontal
	for(std::string line; std::getline(read, line);) {
		view.push_back(line);
	}
	result.push_back(view);

	// Vertical
	std::vector<std::string> vertview(view[0].size());
	for(uint32_t i = 0; i < view.size(); i++) {
		for(uint32_t j = 0; j < view[i].size(); j++) {
			vertview[j] += view[i][j];
		}
	}
	result.push_back(vertview);

	//Diagonal-Down
	std::vector<std::string> diagdown;
	std::string diagstr;
	std::pair<int32_t, int32_t> pos;
	for(uint32_t i = 0; i < view.size(); i++) {
		pos = std::make_pair(i, 0);
		while (pos.first < (int32_t) view.size() && pos.second < (int32_t) view[pos.first].size()) {
			diagstr += view[pos.first][pos.second];
			pos.first++;
			pos.second++;
		}
		diagdown.push_back(diagstr);
		diagstr.clear();
	}
	for(uint32_t j = 1; j < view[0].size(); j++) {
			pos = std::make_pair(0, j);
			while (pos.first < (int32_t) view.size() && pos.second < (int32_t) view[pos.first].size()) {
				diagstr += view[pos.first][pos.second];
				pos.first++;
				pos.second++;
			}
			diagdown.push_back(diagstr);
			diagstr.clear();
		}
	result.push_back(diagdown);

	//Diagonal-UP
	std::vector<std::string> diagup;
	for(int i = view.size() - 1; i >= 0; i--) {
		pos = std::make_pair(i, 0);
		while (pos.first >= 0 && pos.second < view[0].size()) {
			diagstr += view[pos.first][pos.second];
			//Look Here

			//if (pos.first > 0) pos.first--;
			pos.first--;
			pos.second++;

		}
		diagup.push_back(diagstr);
		diagstr.clear();
	}
	for(int j = view[0].size()-1; j > 0; j--) {
			pos = std::make_pair(view.size()-1, j);
		while (pos.first >= 0 && pos.second < view[0].size()) {
			diagstr += view[pos.first][pos.second];

			pos.first--;
			pos.second++;
		}
		diagup.push_back(diagstr);
		diagstr.clear();
		}
	result.push_back(diagup);
	return result;
}

std::vector<std::vector<std::tuple<std::string, std::pair<uint32_t, uint32_t>, std::pair<int32_t, int32_t>>>> parseInputTwo(std::string input) {
	std::vector<std::vector<std::tuple<std::string, std::pair<uint32_t, uint32_t>, std::pair<int32_t, int32_t>>>> result;
	std::vector<std::string> view;
	std::ifstream read(input);
	// Horizontal
	for(std::string line; std::getline(read, line);) {
		view.push_back(line);
	}

	//Diagonal-Down
	std::vector<std::tuple<std::string, std::pair<uint32_t, uint32_t>, std::pair<int32_t, int32_t>>> diagdown;
	std::string diagstr;
	std::pair<int32_t, int32_t> pos;
	std::pair<int32_t, int32_t> initpos;
	std::pair<int32_t, int32_t> movement = std::make_pair(1, 1);

	for(uint32_t i = 0; i < view.size(); i++) {
		pos = std::make_pair(i, 0);
		initpos = pos;
		while (pos.first < (int32_t) view.size() && pos.second < (int32_t) view[pos.first].size()) {
			diagstr += view[pos.first][pos.second];

			pos.first++;
			pos.second++;
		}
		diagdown.push_back({diagstr, initpos, movement});
		diagstr.clear();
	}

	for(uint32_t j = 1; j < view[0].size(); j++) {
			pos = std::make_pair(0, j);
			initpos = pos;
			while (pos.first < (int32_t) view.size() && pos.second < (int32_t) view[pos.first].size()) {
				diagstr += view[pos.first][pos.second];

				pos.first++;
				pos.second++;
			}
			diagdown.push_back({diagstr, initpos, movement});
			diagstr.clear();
		}
	result.push_back(diagdown);

	//Diagonal-UP
	std::vector<std::tuple<std::string, std::pair<uint32_t, uint32_t>, std::pair<int32_t, int32_t>>> diagup;
	movement = std::make_pair(-1, 1);
	for(int i = view.size() - 1; i >= 0; i--) {
		pos = std::make_pair(i, 0);
		initpos = pos;
		while (pos.first >= 0 && pos.second < view[0].size()) {
			diagstr += view[pos.first][pos.second];

			pos.first--;
			pos.second++;

		}
		diagup.push_back({diagstr, initpos, movement});
		diagstr.clear();
	}

	for(int j = view[0].size()-1; j > 0; j--) {
		pos = std::make_pair(view.size()-1, j);
		initpos = pos;
		while (pos.first >= 0 && pos.second < view[0].size()) {
			diagstr += view[pos.first][pos.second];

			pos.first--;
			pos.second++;
		}
		diagup.push_back({diagstr, initpos, movement});
		diagstr.clear();
	}
	result.push_back(diagup);
	return result;

}


void partOne(std::vector<std::vector<std::string>> input ) {

	int32_t result = 0;
	std::string christmasspirit = "XMAS";
	std::for_each(input.begin(), input.end(), [&](std::vector<std::string> view) {
		std::for_each(view.begin(), view.end(), [&](std::string a ) {
			for(auto pos = std::search(a.begin(), a.end(), christmasspirit.begin(), christmasspirit.end());
					pos != a.end();
					pos = std::search(std::next(pos), a.end(), christmasspirit.begin(), christmasspirit.end())) {
			result++;
			}
			for(auto pos = std::search(a.rbegin(), a.rend(), christmasspirit.begin(), christmasspirit.end());
					pos != a.rend();
					pos = std::search(std::next(pos), a.rend(), christmasspirit.begin(), christmasspirit.end())) {
			result++;
			}
		});
	});
	std::cout << result << std::endl;
}

void partTwo(std::vector<std::vector<std::tuple<std::string, std::pair<uint32_t, uint32_t>, std::pair<int32_t, int32_t>>>> input) {
	int32_t result = 0;
	std::string christmasspirit = "MAS";
	std::vector<std::pair<int32_t, int32_t>> checkpositions;
	std::pair<int32_t, int32_t> candidatepos;

	// Find diagonal "MAS" needed for crosses
	std::for_each(input[0].begin(), input[0].end(), [&](std::tuple<std::string, std::pair<int32_t, int32_t>, std::pair<int32_t, int32_t>> a ) {

		for(auto pos = std::search(std::get<0>(a).begin(), std::get<0>(a).end(), christmasspirit.begin(), christmasspirit.end());
				pos != std::get<0>(a).end();
				pos = std::search(std::next(pos), std::get<0>(a).end(), christmasspirit.begin(), christmasspirit.end())) {

			// search gives the M in MAS but since Corsses happen at the A, we want to check for it :D
			int32_t dist = std::distance(std::get<0>(a).begin(), pos) + 1;
			// Convert Position in Diagonal to Position in Array to later check against crosses
			candidatepos = std::make_pair(std::get<1>(a).first + dist * std::get<2>(a).first, std::get<1>(a).second + dist * std::get<2>(a).second);
			checkpositions.push_back(candidatepos);
		}

		// Also check for reverse lines
		for(auto pos = std::search(std::get<0>(a).begin(), std::get<0>(a).end(), christmasspirit.rbegin(), christmasspirit.rend());
				pos != std::get<0>(a).end();
				pos = std::search(std::next(pos), std::get<0>(a).end(), christmasspirit.rbegin(), christmasspirit.rend())) {

			// search gives the M in MAS but since Corsses happen at the A, we want to check for it :D
			int32_t dist = std::distance(std::get<0>(a).begin(), pos) + 1;
			// Convert Position in Diagonal to Position in Array to later check against crosses
			candidatepos = std::make_pair(std::get<1>(a).first + dist * std::get<2>(a).first, std::get<1>(a).second + dist * std::get<2>(a).second);
			checkpositions.push_back(candidatepos);
		}

	});
	// Now verify if we have crosses
	std::for_each(input[1].begin(), input[1].end(), [&](std::tuple<std::string, std::pair<int32_t, int32_t>, std::pair<int32_t, int32_t>> a ) {

			for(auto pos = std::search(std::get<0>(a).begin(), std::get<0>(a).end(), christmasspirit.begin(), christmasspirit.end());
					pos != std::get<0>(a).end();
					pos = std::search(std::next(pos), std::get<0>(a).end(), christmasspirit.begin(), christmasspirit.end())) {

				// search gives the M in MAS but since Corsses happen at the A, we want to check for it :D
				int32_t dist = std::distance(std::get<0>(a).begin(), pos) + 1;
				// Convert Position in Diagonal to Position in Array to later check against crosses
				int ypos = std::get<1>(a).first + dist * std::get<2>(a).first;
				int xpos = std::get<1>(a).second + dist * std::get<2>(a).second;
				candidatepos = std::make_pair(ypos, xpos);
				checkpositions.push_back(candidatepos);
			}
		});
	// Also for reverse crosses
	std::for_each(input[1].begin(), input[1].end(), [&](std::tuple<std::string, std::pair<int32_t, int32_t>, std::pair<int32_t, int32_t>> a ) {

			for(auto pos = std::search(std::get<0>(a).begin(), std::get<0>(a).end(), christmasspirit.rbegin(), christmasspirit.rend());
					pos != std::get<0>(a).end();
					pos = std::search(std::next(pos), std::get<0>(a).end(), christmasspirit.rbegin(), christmasspirit.rend())) {

				// search gives the M in MAS but since Corsses happen at the A, we want to check for it :D
				int32_t dist = std::distance(std::get<0>(a).begin(), pos) + 1;
				// Convert Position in Diagonal to Position in Array to later check against crosses
				int ypos = std::get<1>(a).first + dist * std::get<2>(a).first;
				int xpos = std::get<1>(a).second + dist * std::get<2>(a).second;
				candidatepos = std::make_pair(ypos, xpos);
				checkpositions.push_back(candidatepos);
			}

		});
	std::map<std::pair<int32_t, int32_t>, int32_t> crosspositions;
	std::for_each(checkpositions.begin(), checkpositions.end(), [&crosspositions](auto pos) {
		crosspositions[pos]++;
	});
	result = std::transform_reduce(crosspositions.begin(), crosspositions.end(), 0, std::plus(), [](auto a) {
		return a.second == 2;
	});
	std::cout << result << std::endl;
}


void day4_() {
	std::string input = "src/day4/realinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	std::vector<std::vector<std::string>> matrices = parseInput(input);
	auto matricestwo = parseInputTwo(input);

	partOne(matrices);
	partTwo(matricestwo);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
}
