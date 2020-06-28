//
// Created by Marco on 28/06/2020.
//

#ifndef C___VERTICESBYDEGREEQUEUE_H
#define C___VERTICESBYDEGREEQUEUE_H

#include <queue>
#include "VerticesLinkedList.h"

class DegreeBucket {
	public:
		bool inPQ = false;
		VerticesLinkedList vertices;
		const int degree;

		explicit DegreeBucket(const int degree) : degree(degree) {}

};

struct DegreeBucketComparator {
	bool operator()(const DegreeBucket *lhs, const DegreeBucket *rhs) const {
		return lhs->degree < rhs->degree;
	}
};

class VerticesByDegreeQueue {

	private:
		std::vector<DegreeBucket> buckets;
		std::priority_queue<DegreeBucket *, std::vector<DegreeBucket *>, DegreeBucketComparator> queue;

	public:
		explicit VerticesByDegreeQueue(const int maxDegree) : buckets(maxDegree) {
		}

		void add(Node *node) {
			node->add(this, getListForDegree(node->degree));
		}

		VerticesLinkedList *getListForDegree(int degree) {
			DegreeBucket *bucket = &(this->buckets[degree]);
			if (!bucket->inPQ) {
				bucket->inPQ = true;
				this->queue.push(bucket);
			}
			this->ensureInPQ(bucket);
			return &bucket->vertices;
		}

		bool isEmpty() {
			return firstNonEmptyBucket() == nullptr;
		}

		bool isNotEmpty() {
			return !this->isEmpty();
		}

		void changeDegree(Node *node, int newDegree) {
			VerticesLinkedList *newList = getListForDegree(newDegree);
			if (node->list != newList) {
				node->move(newList);
			}
		}

	private:

		DegreeBucket *firstNonEmptyBucket() {
			while (true) {
				if (queue.empty()) {
					return nullptr;
				} else {
					DegreeBucket *firstBucket = queue.top();
					if (firstBucket->vertices.isEmpty()) {
						queue.pop();
						firstBucket->inPQ = false;
					} else {
						return firstBucket;
					}
				}
			}
		}

		void ensureInPQ(DegreeBucket *bucket) {
			if (!bucket->inPQ) {
				bucket->inPQ = true;
				this->queue.push(bucket);
			}
		}
};

#endif //C___VERTICESBYDEGREEQUEUE_H
