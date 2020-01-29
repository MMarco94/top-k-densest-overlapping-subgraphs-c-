//
// Created by marco on 23/01/20.
//

#ifndef C___LINEARQUEUE_H
#define C___LINEARQUEUE_H

#include <cstdint>
#include <vector>
#include "../Graph.h"
#include "../utils.cpp"

class LinearQueue {
		const std::vector<double> &backingWeights;
		const std::vector<VectorizableBool> &isNotRemoved;
	public:
		LinearQueue(const int size, const std::vector<double> &backingWeights, const std::vector<VectorizableBool> &isNotRemoved) : backingWeights(backingWeights), isNotRemoved(isNotRemoved) {
		}

		Vertex head() {
			Vertex worst(-1);
			double worstWeight = std::numeric_limits<double>::max();
			int size = this->backingWeights.size();
			for (int i = 0; i < size; i++) {
				const auto &v = Vertex(i);
				const auto w = this->getWeight(v);
				if (w < worstWeight) {
					worstWeight = w;
					worst = v;
				}
			}
			return worst;
		}

		void notifyComparisonChanged(Vertex vertex) {
		}

	private:

		double getWeight(Vertex pos) {
			if (this->isNotRemoved[pos.id]) {
				return this->backingWeights[pos.id];
			} else {
				return std::numeric_limits<double>::max();
			}
		}
};

#endif //C___LINEARQUEUE_H
