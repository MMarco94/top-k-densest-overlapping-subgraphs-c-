//
// Created by Marco on 22/01/2020.
//

#ifndef C___DISTANCES_H
#define C___DISTANCES_H

#include "Graph.h"

class MetricDistance {
	public:
		static double getDistance(int g1Size, int g2Size, const int intersectionSize) {
			bool areEqual = g1Size == g2Size && g1Size == intersectionSize;
			if (!areEqual) {
				return 2 - intersectionSize * intersectionSize / (double) (g1Size * g2Size);
			} else {
				return 0.0;
			}
		}
};

#endif //C___DISTANCES_H
