//
// Created by Marco on 22/01/2020.
//
#ifndef C___GRAPH_H
#define C___GRAPH_H

#include <stdexcept>
#include <utility>
#include <vector>
#include <bits/shared_ptr.h>
#include <memory>

class Vertex {
	public:
		int id;

		Vertex(int id) : id(id) {}

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

		[[nodiscard]] Vertex otherVertex(Vertex v) const {
			if (a == v) {
				return b;
			} else if (b == v) {
				return a;
			} else {
				throw std::logic_error("Vertex not in this edge");
			}
		}
};

class Graph {
	public:
		const int size;
		const std::vector<Edge> &edges;
		std::vector<std::vector<Edge>> edgesMap;

	private :
		Graph(int size, const std::vector<Edge> &edges) : size(size), edges(edges), edgesMap(size) {
			for (auto &e : this->edges) {
				this->edgesMap[e.a.id].emplace_back(e);
				this->edgesMap[e.b.id].emplace_back(e);
			}
		}

	public:
		static std::shared_ptr<Graph> getInstance(int size, const std::vector<Edge> &edges) {
			return std::make_shared<Graph>(size, edges);
		}
};

class SubGraph {
	public:
		std::shared_ptr<Graph> parent;
		std::vector<bool> verticesMask;
		int size;

		SubGraph(std::shared_ptr<Graph> parent) :
				size(parent->size),
				parent(std::move(parent)),
				verticesMask(this->size, true) {}

		SubGraph(std::shared_ptr<Graph> parent, std::vector<bool> verticesMask, int size) : parent(std::move(parent)), verticesMask(std::move(verticesMask)), size(size) {}

		[[nodiscard]] SubGraph clone() const {//TODO: check copy
			return SubGraph(this->parent, this->verticesMask, this->size);
		}

		[[nodiscard]] bool contains(Vertex vertex) const {
			return this->verticesMask[vertex.id];
		}

		void remove(Vertex vertex) {
			if (!this->verticesMask[vertex.id]) { throw std::logic_error("Vertex not in Subgraph"); }
			this->verticesMask[vertex.id] = false;
			this->size--;
		}

		void add(Vertex vertex) {
			if (this->verticesMask[vertex.id]) { throw std::logic_error("Vertex already in Subgraph"); }
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