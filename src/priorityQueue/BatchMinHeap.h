//
// Created by marco on 23/01/20.
//

#ifndef C___BATCHMINHEAP_H
#define C___BATCHMINHEAP_H

#include <cstdint>
#include <vector>
#include "../Graph.h"
#include "../utils.cpp"

class BatchMinHeap {
		const int size;
		const std::vector<double> &backingWeights;
		const std::vector<VectorizableBool> &isNotRemoved;
		std::vector<Vertex> heap;
		std::vector<int> reverseIndexes;

		std::vector<VectorizableBool> changedPositions;
		int largestChangedPosition;
	public:
		BatchMinHeap(const int size, const std::vector<double> &backingWeights, const std::vector<VectorizableBool> &isNotRemoved) :
				size(size), backingWeights(backingWeights), isNotRemoved(isNotRemoved), changedPositions(size / 2, true), largestChangedPosition(size / 2 - 1) {
			this->heap.reserve(size);
			this->reverseIndexes.reserve(size);
			for (int i = 0; i < size; i++) {
				this->heap.emplace_back(i);
				this->reverseIndexes.emplace_back(i);
			}
		}

		Vertex head() {
			this->ensureChangesAreCommitted();
			return this->heap[0];
		}

		void ensureChangesAreCommitted() {
			for (int pos = this->largestChangedPosition; pos >= 0; pos--) {
				if (this->changedPositions[pos]) {
					if (!this->minHeapify(pos) && pos > 0) {
						this->changedPositions[parent(pos)] = true;
					}
					this->changedPositions[pos] = false;
				}
			}
			this->largestChangedPosition = -1;
		}

		void notifyComparisonChanged(Vertex vertex) {
			int pos = this->reverseIndexes[vertex.id];
			if (pos >= size / 2) {
				pos = parent(pos);
			}
			this->largestChangedPosition = std::max(pos, this->largestChangedPosition);
			this->changedPositions[pos] = true;
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

		/**
		 * @return Whether something changed
		 */
		bool minHeapify(int pos) {
			return this->minHeapify(pos, this->getWeight(this->heap[pos]));
		}

		bool minHeapify(int pos, double posWeight) {
			int leftChild = BatchMinHeap::leftChild(pos);
			int rightChild = BatchMinHeap::rightChild(pos);
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
				return true;
			} else {
				return false;
			}
		}

};

#endif //C___BATCHMINHEAP_H
