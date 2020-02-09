//
// Created by Marco on 22/01/2020.
//
#ifndef C___GRAPH_H
#define C___GRAPH_H

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
		std::vector<std::vector<Vertex>> outConnections;
		std::vector<std::vector<Vertex>> inConnections;

	public:
		Graph(int size, const std::vector<Edge> &edges) : size(size), edges(edges), outConnections(size), inConnections(size) {
			for (auto &e : this->edges) {
				this->outConnections[e.a.id].emplace_back(e.b);
				this->inConnections[e.b.id].emplace_back(e.a);
			}
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

#endif //C___GRAPH_H
