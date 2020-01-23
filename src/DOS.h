//
// Created by Marco on 22/01/2020.
//

#ifndef C___DOS_H
#define C___DOS_H

#include <bits/shared_ptr.h>

#include <utility>
#include "Graph.h"
#include "Peeler.h"
#include "Distances.h"

class BestSubGraphFinder {
	public:
		SubGraph bestSubGraph;
		double bestSubGraphScore;

		explicit BestSubGraphFinder(SubGraph &sg) : bestSubGraph(sg), bestSubGraphScore(std::numeric_limits<double>::min()) {}

		void registerSubGraph(const SubGraph &sg, double score) {
			if (this->bestSubGraphScore < score) {
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
			BestSubGraphFinder finder(p.candidate);
			while (p.candidate.size > 3) {
				p.removeWorstVertex();
				this->marginalGainModified(finder, p, subGraphs);
			}
			return finder.bestSubGraph;
		}

		void marginalGainModified(BestSubGraphFinder &finder, Peeler &peeler, const std::vector<SubGraph> &subGraphs) const {
			//TODO: check if in subGraphs
			finder.registerSubGraph(peeler.candidate, this->marginalGain(peeler, subGraphs));
		}
};

#endif //C___DOS_H
