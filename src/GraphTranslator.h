//
// Created by Marco on 22/01/2020.
//

#ifndef C___GRAPHTRANSLATOR_H
#define C___GRAPHTRANSLATOR_H

#include <vector>
#include "Graph.h"


template<typename T>
class GraphTranslator {
		std::vector<T> outToIn;
		std::vector<Edge> outEdges;
	public:
		std::shared_ptr<Graph> graph;

		GraphTranslator(const std::vector<T> &vertices, const std::vector<std::pair<T, T>> &edges) :
				outToIn(vertices), outEdges() {
			this->outEdges.reserve(edges.size());
			for (auto &p:edges) {
				this->outEdges.emplace_back(
						Vertex(indexOf(vertices, p.first)),
						Vertex(indexOf(vertices, p.second))
				);
			}
			this->graph = Graph::getInstance(vertices.size(), this->outEdges);
		}

		T getInitialVertex(Vertex vertex) const {
			return this->outToIn[vertex.id];
		}

	private:
		template<typename V>
		static int indexOf(const std::vector<V> &values, const V &value) {
			return std::find(values.begin(), values.end(), value) - values.begin();
		}
};

#endif //C___GRAPHTRANSLATOR_H
