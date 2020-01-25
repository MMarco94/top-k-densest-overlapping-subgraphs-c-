//
// Created by Marco on 22/01/2020.
//

#ifndef C___DOS_H
#define C___DOS_H

#include <utility>
#include "Graph.h"
#include "Peeler.h"
#include "Distances.h"
#include "utils.cpp"

class BestSubGraphFinder {
	public:
		const std::vector<SubGraph> &subGraphs;
		SubGraph bestSubGraph;
		double bestSubGraphScore;

		explicit BestSubGraphFinder(const std::vector<SubGraph> &subGraphs, SubGraph &sg) :
				subGraphs(subGraphs), bestSubGraph(sg), bestSubGraphScore(std::numeric_limits<double>::min()) {}

		void registerSubGraph(const SubGraph &sg, double score) {
			if (this->bestSubGraphScore < score && !contains(this->subGraphs, sg)) {
				this->bestSubGraph = sg;
				this->bestSubGraphScore = score;
			}
		}
};

class DOS {
		std::shared_ptr<Graph> graph;
		double lambda;
	public:
		DOS(std::shared_ptr<Graph> graph, double lambda) : graph(std::move(graph)), lambda(lambda) {}

		[[nodiscard]] std::vector<SubGraph> getOverlappingSubGraphs(int subGraphsCount) const {
			std::vector<SubGraph> ret;
			ret.reserve(subGraphsCount);

			for (int i = 0; i < subGraphsCount; i++) {
				ret.push_back(peel(ret));
			}

			return ret;
		}

	private:

		[[nodiscard]] double marginalGain(const Peeler &peeler, const std::vector<SubGraph> &subGraphs) const {
			double sum = 0;
			for (int i = 0; i < subGraphs.size(); i++) {
				auto &sg = subGraphs[i];
				sum += MetricDistance::getDistance(peeler.candidate, sg, peeler.getIntersectionSize(i));
			}
			return peeler.getCandidateDensity() / 2 + this->lambda * sum;
		}

		[[nodiscard]] SubGraph peel(const std::vector<SubGraph> &subGraphs) const {
			auto p = Peeler(this->graph, subGraphs, this->lambda);
			BestSubGraphFinder finder(subGraphs, p.candidate);
			while (p.candidate.size > 3) {
				p.removeWorstVertex();
				this->marginalGainModified(finder, p, subGraphs);
			}
			//TODO: if finder is empty, return random wedge
			return finder.bestSubGraph;
		}

		void marginalGainModified(BestSubGraphFinder &finder, Peeler &peeler, const std::vector<SubGraph> &subGraphs) const {
			const auto &candidate = peeler.candidate;
			if (contains(subGraphs, candidate)) {
				int gSize = this->graph->size;
				for (int v = 0; v < gSize; v++) {
					const Vertex &vv = Vertex(v);
					if (candidate.contains(vv)) {
						peeler.removeTemporary(vv);
						finder.registerSubGraph(candidate, this->marginalGain(peeler, subGraphs));
						peeler.restoreTemporary();
					} else {
						peeler.addTemporary(vv);
						finder.registerSubGraph(candidate, this->marginalGain(peeler, subGraphs));
						peeler.restoreTemporary();
					}
				}
			} else {
				finder.registerSubGraph(candidate, this->marginalGain(peeler, subGraphs));
			}
		}
};

#endif //C___DOS_H
