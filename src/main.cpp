#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include "GraphTranslator.h"
#include "DOS.h"

std::vector<int> parseLabels(const std::string &fileName) {
	std::ifstream file;
	file.open("src/test/resources" + fileName + ".labels");
	std::string s;
	std::vector<int> ret;
	while (std::getline(file, s)) {
		ret.push_back(std::stod(s));
	}
	return ret;
}

std::vector<std::pair<int, int>> parseEdges(const std::string &fileName) {
	std::ifstream file;
	file.open("src/test/resources" + fileName + ".edgelist");
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

	const auto &subGraphs = DOS(translator.graph, 0.25).getOverlappingSubGraphs(10);

	for (auto &sg:subGraphs) {
		std::cout << sg.size << std::endl;
	}

	return 0;
}
