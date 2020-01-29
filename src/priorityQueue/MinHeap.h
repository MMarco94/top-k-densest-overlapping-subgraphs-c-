//
// Created by marco on 23/01/20.
//

#ifndef C___MINHEAP_H
#define C___MINHEAP_H

#include <cstdint>
#include <vector>
#include "../Graph.h"
#include "../utils.cpp"

class MinHeap {
		const int size;
		const std::vector<double> &backingWeights;
		const std::vector<VectorizableBool> &isNotRemoved;
		std::vector<Vertex> heap;
		std::vector<int> reverseIndexes;
	public:
		MinHeap(const int size, const std::vector<double> &backingWeights, const std::vector<VectorizableBool> &isNotRemoved) : size(size), backingWeights(backingWeights), isNotRemoved(isNotRemoved) {
			this->heap.reserve(size);
			this->reverseIndexes.reserve(size);
			for (int i = 0; i < size; i++) {
				this->heap.emplace_back(i);
				this->reverseIndexes.emplace_back(i);
			}
			for (int pos = size / 2; pos >= 0; pos--) {
				this->minHeapify(pos);
			}
		}

		Vertex head() {
			return this->heap[0];
		}

		void notifyComparisonChanged(Vertex vertex) {
			int pos = this->reverseIndexes[vertex.id];
			this->minHeapify(pos);
			this->bubbleUp(pos);
		}

	private:

		double getWeight(Vertex pos) {
			if (this->isNotRemoved[pos.id]) {
				return this->backingWeights[pos.id];
			} else {
				return std::numeric_limits<double>::max();
			}
		}

		static int parent(int pos) {
			return (pos + 1) / 2 - 1;
		}

		static int leftChild(int pos) {
			return 2 * pos + 1;
		}

		static int rightChild(int pos) {
			return 2 * pos + 2;
		}

		void swap(int a, int b) {
			Vertex ha = this->heap[a];
			Vertex hb = this->heap[b];
			int tmp = this->reverseIndexes[ha.id];
			this->reverseIndexes[ha.id] = this->reverseIndexes[hb.id];
			this->reverseIndexes[hb.id] = tmp;
			this->heap[a] = hb;
			this->heap[b] = ha;
		}

		void bubbleUp(int pos) {
			int current = pos;
			double currentWeight = this->getWeight(this->heap[pos]);
			while (current > 0 && getWeight(this->heap[MinHeap::parent(pos)]) > currentWeight) {
				swap(current, MinHeap::parent(current));
				current = MinHeap::parent(current);
			}
		}

		void minHeapify(int pos) {
			this->minHeapify(pos, this->getWeight(this->heap[pos]));
		}

		void minHeapify(int pos, double posWeight) {
			int leftChild = MinHeap::leftChild(pos);
			int rightChild = MinHeap::rightChild(pos);
			int hasLeftChild = leftChild < size;
			int hasRightChild = rightChild < size;

			double leftWeight = hasLeftChild ? this->getWeight(this->heap[leftChild]) : -1.0;
			double rightWeight = hasRightChild ? this->getWeight(this->heap[rightChild]) : -1.0;
			if ((hasLeftChild && posWeight > leftWeight) || (hasRightChild && posWeight > rightWeight)) {
				if (hasRightChild && leftWeight > rightWeight) {
					this->swap(pos, rightChild);
					this->minHeapify(rightChild, posWeight);
				} else if (hasLeftChild) {
					this->swap(pos, leftChild);
					this->minHeapify(leftChild, posWeight);
				} else throw std::logic_error("Illegal state");
			}
		}

};

#endif //C___MINHEAP_H
