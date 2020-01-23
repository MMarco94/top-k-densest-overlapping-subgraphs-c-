//
// Created by Marco on 22/01/2020.
//

#ifndef C___PEELER_H
#define C___PEELER_H

#include <utility>
#include "Graph.h"

class Peeler {
	private:
		std::shared_ptr<Graph> graph;
		const std::vector<SubGraph> &subGraphs;
		double lambda;

		int candidateEdges;
		std::vector<int> degrees;
		std::vector<double> weights;
		std::vector<int> intersectionsSize;
	public:
		SubGraph candidate;

		Peeler(std::shared_ptr<Graph> graph, const std::vector<SubGraph> &subGraphs, double lambda) :
				graph(std::move(graph)),
				subGraphs(subGraphs),
				lambda(lambda),
				candidate(this->graph),
				candidateEdges(this->graph->edges.size()),
				degrees(this->graph->size),
				weights(this->graph->size),
				intersectionsSize(this->subGraphs.size()) {
			for (int i = 0; i < this->graph->size; i++) {
				degrees[i] = this->graph->edgesMap[i].size();

				int cnt = 0;
				for (auto &sg:subGraphs) {
					if (sg.contains(Vertex(i))) {
						cnt++;
					}
				}
				weights[i] = degrees[i] - 4 * lambda * cnt;
			}

			for (int i = 0; i < this->subGraphs.size(); i++) {
				intersectionsSize[i] = subGraphs[i].size;
			}
		}

		[[nodiscard]] double getCandidateDensity() const {
			return candidateEdges / (double) candidate.size;
		}

		[[nodiscard]] int getIntersectionSize(int subGraphIndex) const {
			return this->intersectionsSize[subGraphIndex];
		}

		void removeWorstVertex() {
			Vertex worst(-1);
			double worstWeight = std::numeric_limits<double>::max();
			int size = this->graph->size;
			for (int i = 0; i < size; i++) {
				const auto &v = Vertex(i);
				if (candidate.contains(v)) {
					double w = this->weights[i];
					if (w < worstWeight) {
						worstWeight = w;
						worst = v;
					}
				}
			}

			this->remove(worst);
		}

		void remove(Vertex vertex) {
			this->candidateEdges -= this->degrees[vertex.id];
			candidate.remove(vertex);
			forEachConnectedVertex(vertex, [this](Vertex v, int vc) {
				this->weights[v.id] -= vc;
				this->degrees[v.id] -= vc;
			});
			forEachSubGraphs(
					vertex,
					[this](const SubGraph &sg, int index) {
						this->intersectionsSize[index]--;
					},
					[this](const SubGraph &g, Vertex v) {
						weights[v.id] += 4 * this->lambda / g.size;
					}
			);
		}

		template<typename F>
		void forEachConnectedVertex(Vertex vertex, F f) {
			int vertexCount = 0;
			auto &edges = this->graph->edgesMap[vertex.id];
			for (auto &e:edges) {
				auto other = e.otherVertex(vertex);
				if (this->candidate.contains(other)) {
					f(other, 1);
					vertexCount++;
				}
			}
			if (vertexCount > 0) {
				f(vertex, vertexCount);
			}
		}

		template<typename F1, typename F2>
		void forEachSubGraphs(Vertex vertex, F1 sg, F2 f) {
			unsigned long sgSize = this->subGraphs.size();
			int gSize = this->graph->size;
			for (int i = 0; i < sgSize; i++) {
				const auto &subGraph = this->subGraphs[i];
				if (subGraph.contains(vertex)) {
					sg(subGraph, i);
					for (int v = 0; v < gSize; v++) {
						const Vertex &vv = Vertex(v);
						if (subGraph.contains(vv)) {
							f(subGraph, vv);
						}
					}
				}
			}
		}
};

#endif //C___PEELER_H
