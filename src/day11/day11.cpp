#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <utility>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <string>
#include <map>
#include <cmath>
#include "day11.h"
#include <thread>
#include <mutex>
#include <format>
#include "boost/algorithm/string.hpp"

namespace std {
	template<typename T1, typename T2>
	struct hash<std::pair<T1, T2>> {
		std::size_t operator()(const std::pair<T1, T2> p) const {
			auto h1 = std::hash<T1>{}(p.first);
			auto h2 = std::hash<T2>{}(p.second);
			return h1 ^ (h2 < 1);
		}
	};
}

struct Pebbles {
	std::unordered_map<uint64_t, uint64_t> cache;
	std::unordered_map<std::pair<uint64_t, int>, uint64_t> newcache;
	std::vector<uint64_t> current_epoch;
};

uint32_t num_Digits(uint64_t n) {
	return n < 10 ? 1 :
		       n < 100 ? 2 :
		       n < 1000 ? 3 :
		       n < 10000 ? 4 :
		       n < 100000 ? 5 :
		       n < 1000000 ? 6 :
		       n < 10000000 ? 7 :
		       n < 100000000 ? 8 :
		       n < 1000000000 ? 9 :
		       n < 10000000000ULL ? 10 :
		       n < 100000000000ULL ? 11 :
		       n < 1000000000000ULL ? 12 :
		       n < 10000000000000ULL ? 13 :
		       n < 100000000000000ULL ? 14 :
		       n < 1000000000000000ULL ? 15 :
		       n < 10000000000000000ULL ? 16 :
		       n < 100000000000000000ULL ? 17 :
		       n < 1000000000000000000ULL ? 18 : 19;
}

static uint64_t POW10[] = {
	1, 10, 100, 1000, 10000, 100000, 1000000,
	10000000, 100000000, 1000000000, 10000000000ULL,
	100000000000ULL, 1000000000000ULL, 10000000000000ULL,
	100000000000000ULL, 1000000000000000ULL,
	10000000000000000ULL, 100000000000000000ULL,
	1000000000000000000ULL, 10000000000000000000ULL
};

static Pebbles parseInput(const std::string& input) {
	Pebbles result;
	std::ifstream read(input);
	std::vector<uint64_t> numbers;
	std::string line;
	std::getline(read, line);
	std::vector<boost::iterator_range<std::string::iterator>> copyrange;

	boost::split(copyrange, line, boost::is_any_of(" "), boost::token_compress_on);

	numbers.reserve(copyrange.size());

	std::transform(copyrange.begin(), copyrange.end(), std::back_inserter(numbers), [&](const boost::iterator_range<std::string::iterator>& token) {
		return static_cast<uint64_t>(std::stoul(std::string(token.begin(), token.end())));
	});
	result.current_epoch = std::move(numbers);
	return result;
}

void computeEpoch(Pebbles& input) {
	// Go over array, and count added numbers to avoid reallocation
	uint32_t newnumbers = 0;
	std::vector<uint64_t> new_epoch;
	new_epoch.reserve(input.current_epoch.size() * 2);

	// Idea: search new number chunks and group them into 8 threads
	for (uint64_t pebble : input.current_epoch) {
		if (pebble == 0) {
			new_epoch.push_back(1);
			continue;
		}
		uint32_t digits = num_Digits(pebble);
		if ((digits & 1) == 0) {
			uint64_t divisor = POW10[digits/2];
			new_epoch.push_back(pebble / divisor);
			new_epoch.push_back(pebble % divisor);
		}
		else {
			auto it = input.cache.find(pebble);
			if(it != input.cache.end()) {
				new_epoch.push_back(it->second);
			}
			else {
				uint64_t newnum = pebble * 2024;
				new_epoch.push_back(newnum);
				input.cache[pebble] = newnum;

			}
		}
	}
	input.current_epoch = std::move(new_epoch);
}

void computeEpoch_faster(Pebbles& input) {
	int threads = 8;
	std::mutex cache_mutex;
	std::vector<std::vector<uint64_t>> outputs(threads);
	std::vector<std::thread> pool;

	auto& in = input.current_epoch;
	int chunksize = (in.size() + threads - 1) / threads;

	for (int t = 0; t < threads; t++) {
		pool.emplace_back([&, t]() {
			int begin = t * chunksize;
			int end = std::min((t+1) * chunksize, static_cast<int>(in.size()));
			auto& output = outputs[t];

			for (int i = begin; i < end; i++)
			{
				uint64_t n = in.at(i);
				if (n == 0) {
					output.push_back(1);
					continue;
				}
				uint32_t digits = num_Digits(n);
				if ((digits & 1) == 0) {

					uint64_t divisor = POW10[digits/2];
					// e.g: split 1234 -> divide by 10² to get 12 * 100 + 34
					output.push_back(n / divisor);
					output.push_back( n % divisor);
				} else {
					uint64_t cache_entry;
					bool found = false;
					{
						std::scoped_lock<std::mutex> lock(cache_mutex);
						auto iterator = input.cache.find(n);
						if (iterator != input.cache.end()) {
							cache_entry = iterator->second;
							found = true;
						}
					}
					if (found) {
						output.push_back(cache_entry);
					} else {
						uint64_t num = 2024 * n;
						input.cache[n] = num;
						output.push_back(num);
					}
				}
			}
		});
	}

	for ( std::thread& t : pool) t.join();
	std::vector<uint64_t> result;
	uint64_t total = 0;
	for (std::vector<uint64_t>& o : outputs) total += o.size();
	result.reserve(total);
	for (auto& o : outputs) {
		result.insert(result.end(), o.begin(), o.end());
	}
	input.current_epoch = std::move(result);
}

uint64_t computeEpoch_fastest(Pebbles& cache, uint64_t pebble, int epoch) {
	if (epoch == 0) {
		return 1;
	} else {
		if (auto cached = cache.newcache.find({pebble, epoch}); cached != cache.newcache.end()) {
			return cached->second;
		}

		uint64_t result = 0;
		if (pebble == 0) {
			result = computeEpoch_fastest(cache, static_cast<uint64_t>(1), epoch - 1);
		} else {
			int digits = num_Digits(pebble);
			if ((digits & 1) == 0) {
				int divisor = POW10[digits/2];
				result = computeEpoch_fastest(cache, pebble/divisor, epoch - 1) + computeEpoch_fastest(cache, pebble%divisor, epoch - 1);
				cache.newcache[{pebble, epoch}] = result;
			} else {
				result = computeEpoch_fastest(cache, pebble * 2024, epoch - 1);
				cache.newcache[{pebble, epoch}] = result;
			}
		}
		return result;
	}
}


static void partOne(Pebbles& input){
	int counter = 45;
	std::vector<double> epoch_times;

	for (int i = 0; i < 45; ++i) {
		auto start = std::chrono::steady_clock::now();
		computeEpoch_faster(input);
		auto end = std::chrono::steady_clock::now();
		epoch_times.push_back(std::chrono::duration<double>(end - start).count());
	}
	double total = std::accumulate(epoch_times.begin(), epoch_times.end(), 0.0);
	std::cout << "Avg epoch time: " << (total / epoch_times.size()) << " s\n";
	std::cout << "Stones after " << counter << " iterations: " << input.current_epoch.size() << "\n";

}

static void partTwo(Pebbles& input) {
	int counter = 75;
	uint64_t result = std::transform_reduce(input.current_epoch.begin(), input.current_epoch.end(), 0l, std::plus{}, [&](uint64_t pebble){
		return computeEpoch_fastest(input, pebble, counter);
	});
	std::cout << "Stones after " << counter << " iterations: " << result << "\n";

	return;
}

void day11_() {
	std::string input = "src/day11/realinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto pebbles = parseInput(input);
	//partOne(pebbles);
	partTwo(pebbles);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
}

