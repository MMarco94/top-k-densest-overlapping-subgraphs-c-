//
// Created by Marco on 22/01/2020.
//
#ifndef C___GRAPH_H
#define C___GRAPH_H

#include <utility>
#include <vector>
#include <memory>
#include "utils.cpp"

class Vertex {
	public:
		int id;

		explicit Vertex(int id) : id(id) {}

		bool operator==(const Vertex &rhs) const {
			return id == rhs.id;
		}

		bool operator!=(const Vertex &rhs) const {
			return id != rhs.id;
		}
};

class Edge {
	public:
		Vertex a;
		Vertex b;

		Edge(const Vertex &a, const Vertex &b) : a(a), b(b) {}
};

class Graph {
	public:
		const int size;
		const std::vector<Edge> &edges;
		const std::vector<std::vector<Vertex>> connectionsMap;
		const int maxDegree;

		static std::vector<std::vector<Vertex>> _connectionsMap(int size, const std::vector<Edge> &edges) {
			std::vector<std::vector<Vertex>> ret(size);
			for (auto &e : edges) {
				ret[e.a.id].emplace_back(e.b);
				ret[e.b.id].emplace_back(e.a);
			}
			return ret;
		}

		static int _maxDegree(const std::vector<std::vector<Vertex>> &connectionsMap) {
			int maxDegree = 0;
			for (auto &n : connectionsMap) {
				if (n.size() > maxDegree) {
					maxDegree = n.size();
				}
			}
			return maxDegree;
		}

	public:
		Graph(int size, const std::vector<Edge> &edges) :
				size(size),
				edges(edges),
				connectionsMap(_connectionsMap(size, edges)),
				maxDegree(_maxDegree(connectionsMap)) {
		}

		static std::shared_ptr<Graph> getInstance(int size, const std::vector<Edge> &edges) {
			return std::make_shared<Graph>(size, edges);
		}
};

class SubGraph {
	public:
		std::shared_ptr<Graph> parent;
		int size;
		std::vector<VectorizableBool> verticesMask;
	private:
		std::vector<Vertex> materializedVertices;
	public:

		explicit SubGraph(std::shared_ptr<Graph> parent) :
				parent(std::move(parent)),
				size(this->parent->size),
				verticesMask(size, 1) {
		}

		SubGraph(std::shared_ptr<Graph> parent, std::vector<VectorizableBool> verticesMask, int size) : parent(std::move(parent)), verticesMask(std::move(verticesMask)), size(size) {}

		/*	//copy
			SubGraph(const SubGraph &sg) {
				this->parent = sg.parent;
				this->size = sg.size;
				this->verticesMask = sg.verticesMask;
			}*/

		template<typename F>
		void forEachVertex(F f) const {
			if (size > 0) {
				if (this->materializedVertices.empty()) {
					int vs = this->verticesMask.size();
					for (int i = 0; i < vs; i++) {
						if (this->verticesMask[i]) {
							f(Vertex(i));
						}
					}
				} else {
					for (auto &v : this->materializedVertices) {
						f(v);
					}
				}
			}
		}

		void materializeVertices() {
			forEachVertex([this](Vertex v) {
				this->materializedVertices.emplace_back(v);
			});
		}

		[[nodiscard]] bool contains(const Vertex vertex) const {
			return this->verticesMask[vertex.id];
		}

		void remove(const Vertex vertex) {
			this->verticesMask[vertex.id] = false;
			this->size--;
		}

		void add(const Vertex vertex) {
			this->verticesMask[vertex.id] = true;
			this->size++;
		}

		bool operator==(const SubGraph &rhs) const {
			return parent == rhs.parent &&
				   size == rhs.size &&
				   verticesMask == rhs.verticesMask;
		}

		bool operator!=(const SubGraph &rhs) const {
			return !(rhs == *this);
		}
};

class GraphSnapshot {
	private:
		int removalCount;
		std::shared_ptr<Graph> graph;
		std::shared_ptr<std::vector<Vertex>> removals;
	public:
		GraphSnapshot(int removalCount, std::shared_ptr<Graph> graph, std::shared_ptr<std::vector<Vertex>> removals) :
				removalCount(removalCount),
				graph(std::move(graph)),
				removals(std::move(removals)) {}

		SubGraph toSubGraph() {
			SubGraph ret = SubGraph(this->graph);
			for (int i = 0; i < removalCount; i++) {
				ret.remove(removals->at(i));
			}
			return ret;
		}
};


class SnapshottableSubGraph {
	private:
		SubGraph subGraph;
		std::shared_ptr<std::vector<Vertex>> removals;

	public:

		explicit SnapshottableSubGraph(std::shared_ptr<Graph> parent) : subGraph(std::move(parent)) {}

		[[nodiscard]] int size() const {
			return subGraph.size;
		}

		[[nodiscard]] bool contains(Vertex vertex) const {
			return subGraph.contains(vertex);
		}

		void remove(Vertex vertex) {
			this->removals->emplace_back(vertex);
			subGraph.remove(vertex);
		}

		[[nodiscard]] GraphSnapshot snapshot() const {
			return GraphSnapshot(this->removals->size(), this->subGraph.parent, this->removals);
		}
};

#endif //C___GRAPH_H
