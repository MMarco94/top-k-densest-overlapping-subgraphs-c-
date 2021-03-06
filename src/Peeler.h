//
// Created by Marco on 22/01/2020.
//

#ifndef C___PEELER_H
#define C___PEELER_H

#include <utility>
#include "Graph.h"
#include "priorityQueue/BatchMinHeap.h"
#include "priorityQueue/LinearQueue.h"
#include "priorityQueue/MinHeap.h"

template<class T = MinHeap>
class Peeler {
	private:
		std::shared_ptr<Graph> graph;
		const std::vector<SubGraph> &subGraphs;
		double lambda;

		int candidateEdges;
		std::vector<int> degrees;
		std::vector<double> weights;
		std::vector<int> intersectionsSize;
		std::unique_ptr<T> vertexPriorityQueue;

		Vertex temporaryVertex = Vertex(-1);
		bool temporaryIsAdd = false;

		static std::vector<int> prepareDegrees(const std::shared_ptr<Graph> &graph) {
			int gSize = graph->size;
			std::vector<int> ret;
			ret.reserve(gSize);
			for (int i = 0; i < gSize; i++) {
				ret.emplace_back(graph->outConnections[i].size() + graph->inConnections[i].size());
			}
			return ret;
		}

		static std::vector<double> prepareWeights(const std::shared_ptr<Graph> &graph, const std::vector<SubGraph> &subGraphs, double lambda, const std::vector<int> &degrees) {
			int gSize = graph->size;
			std::vector<double> ret;
			ret.reserve(gSize);
			for (int i = 0; i < gSize; i++) {
				int cnt = 0;
				for (auto &sg:subGraphs) {
					if (sg.contains(Vertex(i))) {
						cnt++;
					}
				}
				ret.emplace_back(degrees[i] - 4 * lambda * cnt);
			}
			return ret;
		}

		static std::vector<int> prepareIntersections(const std::vector<SubGraph> &subGraphs) {
			std::vector<int> ret;
			ret.reserve(subGraphs.size());
			for (const auto &subGraph : subGraphs) {
				ret.emplace_back(subGraph.size);
			}
			return ret;
		}

	public:
		SubGraph candidate;

		Peeler(std::shared_ptr<Graph> graph, const std::vector<SubGraph> &subGraphs, double lambda) :
				graph(std::move(graph)),
				subGraphs(subGraphs),
				lambda(lambda),
				candidate(this->graph),
				candidateEdges(this->graph->edges.size()),
				degrees(Peeler::prepareDegrees(this->graph)),
				weights(Peeler::prepareWeights(this->graph, this->subGraphs, lambda, this->degrees)),
				intersectionsSize(Peeler::prepareIntersections(this->subGraphs)) {
			vertexPriorityQueue = std::make_unique<T>(this->graph->size, this->weights, this->candidate.verticesMask);
		}

		[[nodiscard]] double getCandidateDensity() const {
			return candidateEdges / (double) candidate.size;
		}

		[[nodiscard]] int getIntersectionSize(int subGraphIndex) const {
			return this->intersectionsSize[subGraphIndex];
		}

		void removeWorstVertex() {
			const Vertex &worst = this->vertexPriorityQueue->head();
			this->remove(worst, true);
		}

		void removeTemporary(Vertex vertex) {
			this->temporaryVertex = vertex;
			this->temporaryIsAdd = false;
			remove(vertex, false);
		}

		void addTemporary(Vertex vertex) {
			this->temporaryVertex = vertex;
			this->temporaryIsAdd = true;
			add(vertex, false);
		}

		void restoreTemporary() {
			if (this->temporaryIsAdd) {
				this->remove(this->temporaryVertex, false);
			} else {
				this->add(this->temporaryVertex, false);
			}
			this->temporaryVertex = Vertex(-1);
		}

	private:
		void remove(Vertex vertex, bool updateQueue) {
			this->candidateEdges -= this->degrees[vertex.id];
			this->editWeight(vertex, updateQueue, [this, vertex]() {
				this->candidate.remove(vertex);
			});
			forEachConnectedVertex(vertex, updateQueue, [this](Vertex v, int vc) {
				this->weights[v.id] -= vc;
				this->degrees[v.id] -= vc;
			});
			forEachSubGraphs(
					vertex,
					updateQueue,
					[this](const SubGraph &sg, int index) {
						this->intersectionsSize[index]--;
					},
					[this](const SubGraph &g, Vertex v) {
						weights[v.id] += 4 * this->lambda / g.size;
					}
			);
		}

		void add(Vertex vertex, bool updateQueue) {
			this->editWeight(vertex, updateQueue, [this, vertex]() {
				this->candidate.add(vertex);
			});
			forEachConnectedVertex(vertex, updateQueue, [this](Vertex v, int vc) {
				this->weights[v.id] += vc;
				this->degrees[v.id] += vc;
			});
			this->candidateEdges += this->degrees[vertex.id];
			forEachSubGraphs(
					vertex,
					updateQueue,
					[this](const SubGraph &sg, int index) {
						this->intersectionsSize[index]++;
					},
					[this](const SubGraph &g, Vertex v) {
						weights[v.id] -= 4 * this->lambda / g.size;
					}
			);
		}

		template<typename F>
		void forEachConnectedVertex(Vertex vertex, bool updateQueue, F f) {
			int vertexCount = 0;
			for (auto &other : this->graph->inConnections[vertex.id]) {
				if (this->candidate.contains(other)) {
					this->editWeight(other, updateQueue, [f, other]() {
						f(other, 1);
					});
					vertexCount++;
				}
			}

			for (auto &other : this->graph->outConnections[vertex.id]) {
				if (this->candidate.contains(other)) {
					this->editWeight(other, updateQueue, [f, other]() {
						f(other, 1);
					});
					vertexCount++;
				}
			}
			if (vertexCount > 0) {
				this->editWeight(vertex, updateQueue, [f, vertex, vertexCount]() {
					f(vertex, vertexCount);
				});
			}
		}

		template<typename F1, typename F2>
		void forEachSubGraphs(Vertex vertex, bool updateQueue, F1 sg, F2 f) {
			unsigned long sgSize = this->subGraphs.size();
			int gSize = this->graph->size;
			for (int i = 0; i < sgSize; i++) {
				const auto &subGraph = this->subGraphs[i];
				if (subGraph.contains(vertex)) {
					sg(subGraph, i);
					subGraph.forEachVertex([=](const Vertex &vv) {
						//It makes sense to reorganize the queue only if candidate is in vv. Otherwise the weight will remain Double.MAX_VALUE
						editWeight(vv, updateQueue/* && candidate.contains(vv)*/, [f, &subGraph, vv]() {
							f(subGraph, vv);
						});
					});
				}
			}
		}

		template<typename F>
		void editWeight(Vertex vertex, bool updateQueue, F f) {
			f();
			if (updateQueue) {
				this->vertexPriorityQueue->notifyComparisonChanged(vertex);
			}
		}
};

#endif //C___PEELER_H
