#include <chrono>

#include <vector>
#include <utility>
#include <fstream>
#include <string>
#include "boost/algorithm/string.hpp"
#include "day13.h"
#include "regex"
#include <cmath>
#include <iostream>
#include <numeric>
#include <eigen3/Eigen/Dense>
#include <optional>

using Matrix2l = Eigen::Matrix<long, 2, 2>;
using Vector2l = Eigen::Matrix<long, 2, 1>;

typedef std::vector<std::pair<Matrix2l, Vector2l>> sles;

bool p2 = true;

static sles parseInput(const std::string& inputFile) {
    std::vector<std::pair<Matrix2l, Vector2l>> result;
    std::ifstream file(inputFile);
    std::regex pattern(R"(X[+=](\d+),\s*Y[+=](\d+))");

    Matrix2l A;
    Vector2l b;
    int row = 0;

    for (std::string line; std::getline(file, line);) {
        if (line.empty()) {
            if (row == 3) {
                result.emplace_back(A, b);
            }
            row = 0;
            continue;
        }
        std::smatch match;
        if (std::regex_search(line, match, pattern)) {
            int x = std::stoul(match[1]);
            int y = std::stoul(match[2]);

            if (row < 2) {
                A(row, 0) = x;
                A(row, 1) = y;
            } else {
                b(0) = p2? x + 1e13 : x;
                b(1) = p2? y + 1e13 : y;
            }
            ++row;
        }
    }
    // Handle last block (if file doesn't end in a blank line)
    if (row == 3) {
        result.emplace_back(A, b);
    }
    return result;
}

std::optional<std::pair<uint64_t, uint64_t>> cramer(const Matrix2l& A, const Vector2l& sol) {

	long a = A(0, 0), b = A(1, 0);
	long c = A(0, 1), d = A(1, 1);
	long e = sol(0), f = sol(1);

	// We are only interested in Integer solutions for x where 0 < x_1, x_2
	long det_A = a*d - b*c;
	if (det_A == 0) return std::nullopt;
	long det_A1 = e*d - f*b;
	long det_A2 = a*f - c*e;
	if (det_A1 % det_A != 0 || det_A2 % det_A != 0) return std::nullopt;

	long x = det_A1 / det_A;
	long y = det_A2 / det_A;

	if (x <= 0 || y <= 0) return std::nullopt;

	return std::pair<uint64_t, uint64_t>{x, y};
}

static void partOne (sles& input) {
	uint64_t result = std::transform_reduce(input.begin(), input.end(), uint64_t(0), std::plus{}, [](auto Ab) -> uint64_t {
		auto& [A, b] = Ab;
		if (auto sol = cramer(A, b)) {
			// Set p2 = true for Part 2
			auto [x, y] = *sol; // Note to self: std::optionals need dereferencing
			return 3*x +y; // button A is 3 times as expensive as button B
		}
		return 0;
	});
	std::cout << "The minimum number of tokens required are: " << result << "\n";
	return;
}


void day13_() {
	std::string input = "src/day13/realinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto Equations = parseInput(input);
	partOne(Equations);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
}

