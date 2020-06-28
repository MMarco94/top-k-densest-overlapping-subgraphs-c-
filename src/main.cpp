#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <algorithm>
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

std::vector<std::pair<int, int>> parseEdgesAsPairs(const std::string &fileName) {
	std::ifstream file;
	file.open("res/" + fileName);
	std::string s;
	std::vector<std::pair<int, int>> ret;
	int e1;
	int e2;
	while (std::getline(file, s)) {
		if (s[0] != '#') {
			if (sscanf(s.c_str(), "%d\t%d", &e1, &e2) == 2) {
				ret.emplace_back(e1, e2);
			}
		}
	}
	return ret;
}

std::vector<Edge> parseEdgesDirectly(const std::string &fileName) {
	std::ifstream file;
	file.open("res/" + fileName);
	std::string s;
	std::vector<Edge> ret;
	int e1;
	int e2;
	while (std::getline(file, s)) {
		if (s[0] != '#') {
			if (sscanf(s.c_str(), "%d\t%d", &e1, &e2) == 2) {
				ret.emplace_back(Vertex(e1), Vertex(e2));
			}
		}
	}
	return ret;
}

int mainWithTranslator() {
	const std::string &dataset = "dbbox_Latin-America";

	const auto &labels = parseLabels(dataset);
	const auto &edges = parseEdgesAsPairs(dataset + ".edgelist");
	const GraphTranslator<int> translator(labels, edges);

	std::cout << labels.size() << " vertices with " << edges.size() << " edges" << std::endl;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	const auto &subGraphs = DOS(translator.graph, 0.25).getOverlappingSubGraphs(10);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Took = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "µs" << std::endl;

	for (auto &sg : subGraphs) {
		std::cout << sg.size << ": ";
		std::vector<int> translatedLabels;
		translatedLabels.reserve(sg.size);
		sg.forEachVertex([&translatedLabels, &translator](Vertex v) {
			translatedLabels.emplace_back(translator.getInitialVertex(v));
		});
		std::sort(translatedLabels.begin(), translatedLabels.end());
		for (auto &l : translatedLabels) {
			std::cout << l << ", ";
		}
		std::cout << std::endl;
	}

	return 0;
}

int main() {
	const std::string &dataset = "web-Google.txt";

	const auto &edges = parseEdgesDirectly(dataset);
	Vertex max(0);
	for (auto &e:edges) {
		if (e.a.id > max.id) { max = e.a; }
		if (e.b.id > max.id) { max = e.b; }
	}

	std::cout << (max.id + 1) << " vertices with " << edges.size() << " edges" << std::endl;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	const auto &graph = std::make_shared<Graph>(max.id + 1, edges);
	const auto &subGraphs = DOS(graph, 0.25).getOverlappingSubGraphs(20);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Took = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "µs" << std::endl;

	for (auto &sg : subGraphs) {
		std::cout << sg.size << ": ";
		std::vector<int> translatedLabels;
		translatedLabels.reserve(sg.size);
		sg.forEachVertex([&translatedLabels](Vertex v) {
			translatedLabels.emplace_back(v.id);
		});
		std::sort(translatedLabels.begin(), translatedLabels.end());
		for (auto &l : translatedLabels) {
			std::cout << l << ", ";
		}
		std::cout << std::endl;
	}

	return 0;
}
