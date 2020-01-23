//
// Created by Marco on 22/01/2020.
//

#ifndef C___DISTANCES_H
#define C___DISTANCES_H

#include "Graph.h"

class MetricDistance {
	public:
		static double getDistance(const SubGraph &g1, const SubGraph &g2, const int intersectionSize) {
			bool areEqual = g1.size == g2.size && g1.size == intersectionSize;
			if (!areEqual) {
				return 2 - intersectionSize * intersectionSize / (double) (g1.size * g2.size);
			} else {
				return 0.0;
			}
		}
};

#endif //C___DISTANCES_H
