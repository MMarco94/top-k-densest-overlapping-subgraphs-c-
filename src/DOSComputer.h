//
// Created by Marco on 28/06/2020.
//

#ifndef C___DOSCOMPUTER_H
#define C___DOSCOMPUTER_H

#include <utility>
#include "Graph.h"
#include "Distances.h"
#include "datastructure/VerticesLinkedList.h"

/**
 * A partition of [Graph]. Contains all the [Vertex] that belong in [partitionKey].
 */
class Partition {
	public:
		std::vector<SubGraph> partitionKey;
		VerticesByDegreeQueue vertices;

		explicit Partition(const int maxDegree) : vertices(maxDegree) {}
};

class DOSComputer {
	private:
		std::vector<Partition> partitions;
		std::vector<Node> nodes;
	public:
		std::shared_ptr<Graph> graph;
		double lambda;
		std::vector<SubGraph> subGraphs;

		DOSComputer(std::shared_ptr<Graph> graph, double lambda) : graph(std::move(graph)), lambda(lambda), nodes(this->graph->size) {
			Partition &partition = partitions.emplace_back(this->graph->maxDegree);
			for (auto &n : this->nodes) {
				n.resetDegree(this->graph);
				partition.vertices.add(n);
			}
		}

		SubGraph peel();
};
class PartitionForPeeling {
	public:
		Peeler* peeler;
		Partition* oldPartition;
		VerticesByDegreeQueue* verticesByDegree ;
double peelWeight;

		PartitionForPeeling(Partition* oldPartition):oldPartition(oldPartition){
			this->verticesByDegree = &this->oldPartition->vertices;
			this->peelWeight=  -4 * lambda * oldPartition.partitionKey.size;
		};
init {
oldPartition.vertices = VerticesByDegreeQueue(graph.maxDegree)
}

fun isEmpty() = verticesByDegree.isEmpty()
fun isNotEmpty() = verticesByDegree.isNotEmpty()
fun minWeight() = peelWeight + verticesByDegree.min().degree
fun min() = verticesByDegree.min()
fun removeMin() = verticesByDegree.removeMin()

fun createPartition(best: SubGraph): DOSComputer.Partition {
check(verticesByDegree.isEmpty())
best.forEachVertex { v ->
val node = nodes[v]
if (node.queue == oldPartition.vertices) {
node.remove()
node.resetDegree(graph)
verticesByDegree.add(node)
}
}
return Partition(oldPartition.partitionKey.plus(best), verticesByDegree)
}
}
class Peeler {
	private:
		DOSComputer *dosComputer;
		SnapshottableSubGraph candidate;
		int candidateEdges;
		std::vector<int> intersections;
		//TODO: partitionsForPeeling
	public:
		explicit Peeler(DOSComputer *dosComputer) :
				dosComputer(dosComputer),
				candidate(dosComputer->graph),
				candidateEdges(dosComputer->graph->edges.size()) {
			this->intersections.reserve(dosComputer->subGraphs.size());
			for (auto &sg : dosComputer->subGraphs) {
				this->intersections.emplace_back(sg.size);
			}

		}

		SubGraph peel() {
			GraphSnapshot best = this->candidate.snapshot();
			double bestGain = this->marginalGain();
			//TODO
		}

	private:
		double marginalGain() {
			if (candidate.size() == 0) {
				return std::numeric_limits<double>::max();
			} else {

				double distance = 0;
				for (int i = 0; i < dosComputer->subGraphs.size(); i++) {
					distance += MetricDistance::getDistance(candidate.size(), dosComputer->subGraphs[i].size, intersections[i]);
				}
				return (double) candidateEdges / candidate.size() / 2 + dosComputer->lambda * distance;
			}
		}
};

SubGraph DOSComputer::peel() {
	return Peeler(this).peel();
}

#endif //C___DOSCOMPUTER_H
