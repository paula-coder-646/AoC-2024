//#include "day10.h"

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
#include <string>
#include <lemon/list_graph.h>
#include <lemon/dijkstra.h>

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>
#include <boost/functional/hash.hpp>

// Hash to use an unordered Map for Eigen Matrices
template<typename Scalar, int Rows, int Cols>
struct std::hash<Eigen::Matrix<Scalar, Rows, Cols>> {

	std::size_t operator()(const Eigen::Matrix<Scalar, Rows, Cols> &v) const noexcept {
		std::size_t result = 0;

		for(std::size_t x = 0; x < Rows; x++) {
			for(std::size_t y = 0; y < Cols; y++) {
				std::size_t temp = std::hash<Scalar>{}(v(x,y));
				boost::hash_combine(temp, result);
			}
		}
		return result;
	}
};

struct Graph {

	// Using a Pointer frees us from calling the constructor of the base classes
	std::unique_ptr<lemon::ListDigraph> graph;
	std::unique_ptr<lemon::ListDigraph::ArcMap<int>> weights;
	std::unique_ptr<boost::bimap<boost::bimaps::set_of<lemon::ListDigraph::Node>, boost::bimaps::unordered_set_of<Eigen::Vector2i>>> nodes;
	std::unique_ptr<boost::bimap<boost::bimaps::unordered_set_of<Eigen::Vector2i>, boost::bimaps::unordered_multiset_of<char>>> height_map;
	std::pair<uint32_t, uint32_t> map_dimension;

	// typedef because damn are those types a pain to write :D
	typedef boost::bimap<boost::bimaps::set_of<lemon::ListDigraph::Node>, boost::bimaps::unordered_set_of<Eigen::Vector2i>>::value_type nodemap_type;
	typedef boost::bimap<boost::bimaps::unordered_set_of<Eigen::Vector2i>, boost::bimaps::unordered_multiset_of<char>>::value_type heightmap_type;
};

static Graph parseInput(std::string input)
{
	Graph result;

	result.graph = std::make_unique<lemon::ListDigraph>();
	result.weights = std::make_unique<lemon::ListDigraph::ArcMap<int>>(*result.graph);
	result.nodes = std::make_unique<boost::bimap<boost::bimaps::set_of<lemon::ListDigraph::Node>, boost::bimaps::unordered_set_of<Eigen::Vector2i>>>();
	result.height_map = std::make_unique<boost::bimap<boost::bimaps::unordered_set_of<Eigen::Vector2i>, boost::bimaps::unordered_multiset_of<char>>>();

	std::ifstream read(input);
	uint32_t y_cnt = 0;
	uint32_t x_cnt = 0;
	for(std::string line; std::getline(read, line);) {
		for(uint32_t x = 0; x < line.size(); x++) {
			x_cnt = line.size();
			if(line.at(x) != '.') {	// Debug Examples
				Eigen::Vector2i position(x, y_cnt);
				auto node = result.graph->addNode();

				// insert needs to
				result.nodes->insert(Graph::nodemap_type(node, position));
				result.height_map->insert(Graph::heightmap_type(position, std::stoul(std::string(1, line.at(x)))));
			}
		}

		for(uint32_t y = 0; y < y_cnt; y++) {
			for(uint32_t x = 0; x < x_cnt; x++) {

				Eigen::Vector2i position(x, y);
				if(result.height_map->left.count(position) == 0) continue; // Debug Examples dont have a height

				auto node = result.nodes->right.at(position);
				uint32_t height = result.height_map->left.at(position);
				std::vector<Eigen::Vector2i> dir_multiplier{Eigen::Vector2i{-1, 0}, Eigen::Vector2i{1, 0}, Eigen::Vector2i{0, 1}, Eigen::Vector2i{0, -1}};
				for(const auto &vector : dir_multiplier) {
					Eigen::Vector2i neighbor_position = position + vector;

					if(neighbor_position.x() > (int32_t)0 && neighbor_position.x() < (int32_t)x_cnt && neighbor_position.y() > (int32_t)0 && neighbor_position.y() < (int32_t)y_cnt) {
						if(result.height_map->left.count(neighbor_position) == 0) continue; // Debug examples

						uint32_t neighbor_height = result.height_map->left.at(neighbor_position);
						lemon::ListDigraph::Node neighbor_node = result.nodes->right.at(neighbor_position);

						if (neighbor_height - height == 1 ) {
							auto newarc = result.graph->addArc(node, neighbor_node);
							(*result.weights)[newarc] = 1;
						}

					}
				}

			}
		}
		y_cnt++;
	}
	result.map_dimension = std::make_pair(x_cnt, y_cnt);
	return result;
}

void partOne(Graph &input) {
	uint64_t result = 0;
	auto startpositions = input.height_map->right.equal_range(0);
	auto peaks = input.height_map->right.equal_range(9);
	for (auto trailhead = startpositions.first; trailhead != startpositions.second; trailhead++) {

		lemon::ListDigraph::Node currentnode = input.nodes->right.at(trailhead->get_left());
		lemon::Dijkstra<lemon::ListDigraph> djikstra(*input.graph, *input.weights);
		djikstra.run(currentnode);

		for(auto peak = peaks.first; peak != peaks.second; peak++) {
			auto peaknode = input.nodes->right.at(peak->get_left());
			if(djikstra.reached(peaknode)) result++;
		}
	}

	std::cout << "Part 1 result: "<< result << std::endl;
}


void day10_old_() {
	std::string input = "src/day10/testinput.txt";
	auto t1 = std::chrono::steady_clock::now();
	auto terrain = parseInput(input);

	partOne(terrain);

	auto t2 = std::chrono::steady_clock::now();
	std::cout << "Time taken to solve both puzzles: " << std::chrono::duration<double>(t2-t1).count() << " seconds." << std::endl;
	}

