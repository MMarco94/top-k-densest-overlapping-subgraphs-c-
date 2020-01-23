#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <chrono>
#include "GraphTranslator.h"
#include "DOS.h"

std::vector<int> parseLabels(const std::string &fileName) {
	std::ifstream file;
	file.open("res/" + fileName + ".labels");
	std::string s;
	std::vector<int> ret;
	while (std::getline(file, s)) {
		ret.push_back(std::stod(s));
	}
	return ret;
}

std::vector<std::pair<int, int>> parseEdges(const std::string &fileName) {
	std::ifstream file;
	file.open("res/" + fileName + ".edgelist");
	std::string s;
	std::vector<std::pair<int, int>> ret;
	int e1;
	int e2;
	while (std::getline(file, s)) {
		if (sscanf(s.c_str(), "%d\t%d", &e1, &e2) == 2) {
			ret.emplace_back(e1, e2);
		}
	}
	return ret;
}

int main() {
	auto dataset = "dbbox_Latin-America";

	const auto &labels = parseLabels(dataset);
	const auto &edges = parseEdges(dataset);
	const GraphTranslator<int> translator(labels, edges);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	const auto &subGraphs = DOS(translator.graph, 0.25).getOverlappingSubGraphs(10);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Took = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "µs" << std::endl;

	for (auto &sg : subGraphs) {
		std::cout << sg.size << ": ";
		for (int i = 0; i < translator.graph->size; i++) {
			const Vertex &v = Vertex(i);
			if (sg.contains(v)) {
				std::cout << translator.getInitialVertex(v) << ", ";
			}
		}
		std::cout << std::endl;
	}

	return 0;
}
