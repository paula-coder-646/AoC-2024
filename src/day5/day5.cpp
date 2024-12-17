#include "day5.h"

#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <utility>
#include <fstream>
#include <algorithm>
#include <numeric>

#include <boost/algorithm/string.hpp>

static std::pair<std::unordered_map<uint32_t, std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>> parseInput(std::string in_file)
{
	std::pair<std::unordered_map<uint32_t, std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>> result;

	std::ifstream read(in_file);
	for(std::string line; std::getline(read, line);)
	{
		if(std::find(line.cbegin(), line.cend(), '|') != line.cend())
		{
			// map item
			std::vector<std::string> first_and_second_number;
			boost::split(first_and_second_number, line, boost::is_any_of("|"), boost::token_compress_off);

			// For every page store all the pages that need to come before it as "page -> dependencies"
			result.first[std::stoul(first_and_second_number[1])].push_back(std::stoul(first_and_second_number[0]));
		}
		else if(std::find(line.cbegin(), line.cend(), ',') != line.cend())
		{
			std::vector<uint32_t> sequence;
			//
			std::transform(boost::make_split_iterator(line, boost::token_finder(boost::is_any_of(","), boost::token_compress_off)),
					boost::split_iterator<std::string::iterator>(), std::back_inserter(sequence), [](auto a) -> uint32_t {
				return std::stoul(boost::copy_range<std::string>(a));
			});

			result.second.push_back(sequence);

		}
	}
	return result;
}

bool checkSequence(const std::unordered_map<uint32_t, std::vector<uint32_t>> &rules, const std::vector<uint32_t> &sequence) {
	bool goodseq = true;

	std::vector<uint32_t> sorted_seq = sequence;
	std::sort(sorted_seq.begin(), sorted_seq.end());

	for (auto s = sequence.cbegin(); s < sequence.cend(); s++) {
		if(rules.count(*s) < 1) {
			continue; // If we have no rule for the item, skip it
		}

		std::vector<uint32_t> sorted_rules = rules.at(*s);
		std::sort(sorted_rules.begin(), sorted_rules.end());

		// Filter the rules for those actually present in sequence
		std::vector<uint> needed_rules;
		std::set_intersection(sorted_rules.cbegin(), sorted_rules.cend(),
				sorted_seq.cbegin(), sorted_seq.cend(), std::back_inserter(needed_rules));

		std::vector<uint32_t> current_seq;
		std::copy(sequence.begin(), s, std::back_inserter(current_seq));
		std::sort(current_seq.begin(), current_seq.end());
		//check if (unordered) sequence up to this point is part of rules

		if(!std::includes(current_seq.begin(), current_seq.end(), needed_rules.begin(), needed_rules.end())) {
			goodseq = false;
			break;
		}
	}
	return goodseq;
}

bool checkSequenceTwo(const std::unordered_map<uint32_t, std::vector<uint32_t>> &rules, std::vector<uint32_t> &sequence) {
	// Only repair rules that are broken already
	if (!checkSequence(rules, sequence)) {
			std::vector<uint32_t> sorted_seq = sequence;
			std::sort(sorted_seq.begin(), sorted_seq.end());

			auto repairelem = sequence.begin();
			for (auto current = sequence.begin(); current < sequence.end();) { // No increment as we take of that ourselves
				if(rules.count(*current) < 1) {
					current++;
					continue; // If we have no rule for the item, skip it
				}

				// Only rules for current element are relevant
				std::vector<uint32_t> sorted_rules = rules.at(*current);
				std::sort(sorted_rules.begin(), sorted_rules.end());

				// Filter dependencies for elements actually present in sequence
				std::vector<uint> needed_rules;
				std::set_intersection(sorted_rules.cbegin(), sorted_rules.cend(),
						sorted_seq.cbegin(), sorted_seq.cend(), std::back_inserter(needed_rules));

				std::vector<uint32_t> current_seq;
				std::copy(sequence.begin(), current, std::back_inserter(current_seq));
				std::sort(current_seq.begin(), current_seq.end());
				//check if (unordered) sequence up to this point is part of rules

				if(!std::includes(current_seq.begin(), current_seq.end(), needed_rules.begin(), needed_rules.end())) {
					current = sequence.insert(std::next(current, 2), *current) - 2;
					current = sequence.erase(current);
					repairelem = ++current; // Keep Track of moved elem
				}
				else {
					if (repairelem != sequence.end()) { // In this case, an element was moved -> Recursive cleanup needed
						current = sequence.begin();
						repairelem = sequence.end();
					}
					else {
						current++; // Seems like last iteration everything was clean -> Move on to next
					}
				}
			}
		return true;
	}
	else {
		return false;
	}
}



uint32_t middleNum(const std::vector<uint32_t>& vec) {
	if (vec.size() % 2 != 1 ) throw std::out_of_range("no middle in even array");
	return vec[vec.size()/2];
}

void partOne(std::pair<std::unordered_map<uint32_t, std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>> input) {
	int result = std::transform_reduce(input.second.cbegin(), input.second.cend(), 0, std::plus{},
			[&](const std::vector<uint32_t>& sequence) -> uint32_t {
		if(checkSequence(input.first, sequence)) {
			return middleNum(sequence);
		} else {
			return 0;
		}
	});
	std::cout << "Part 1 result: "<< result << std::endl;
}

void partTwo(std::pair<std::unordered_map<uint32_t, std::vector<uint32_t>>, std::vector<std::vector<uint32_t>>> input) {
	int result = std::transform_reduce(input.second.begin(), input.second.end(), 0, std::plus{},
			[&](std::vector<uint32_t>& sequence) -> uint32_t {
		if(checkSequenceTwo(input.first, sequence)) {
			return middleNum(sequence);
		} else {
			return 0;
		}
	});
	std::cout << "Part 2 result: "<< result << std::endl;
}


void day5_() {
	std::string input = "src/day5/realinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto booksandrules = parseInput(input);

	partOne(booksandrules);
	partTwo(booksandrules);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
	}


