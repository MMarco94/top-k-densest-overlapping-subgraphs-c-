//
// Created by Marco on 28/06/2020.
//

#ifndef C___VERTICESLINKEDLIST_H
#define C___VERTICESLINKEDLIST_H

#include "../Graph.h"
#include "VerticesByDegreeQueue.h"

class VerticesLinkedList;

class Node {
	public:
		Vertex vertex;
		Node *prev = nullptr;
		Node *next = nullptr;
		int degree = 0;
		VerticesByDegreeQueue *queue = nullptr;
		VerticesLinkedList *list = nullptr;

		explicit Node(const Vertex &vertex) : vertex(vertex) {}

		void remove();

		void add(VerticesByDegreeQueue *newQueue, VerticesLinkedList *newList);

		void move(VerticesLinkedList *newList);

		void changeDegree(int newDegree);

		void resetDegree(const std::shared_ptr<Graph> &graph);
};

class VerticesLinkedList {
	public:
		Node *head = nullptr;

		[[nodiscard]] bool isEmpty() const {
			return this->head == nullptr;
		}

		[[nodiscard]] Node *first() const {
			return this->head;
		}

		[[nodiscard]] Node *min() const {
			Node *ret = this->head;
			int retDegree = ret->degree;
			Node *iter = ret->next;
			while (iter != nullptr) {
				int iterDegree = iter->degree;
				if (iterDegree < retDegree) {
					ret = iter;
					retDegree = iterDegree;
				}
				iter = iter->next;
			}
			return ret;
		}

};


void Node::remove() {
	VerticesLinkedList *l = list;
	//Saving this variable since the compiler cannot know that
	//changing the order of this instruction is side effect free
	bool isHead = l->head == this;
	Node *p = prev;
	Node *n = next;

	if (p != nullptr) {
		p->next = n;
	}
	if (n != nullptr) {
		n->prev = p;
	}

	this->prev = nullptr;
	this->next = nullptr;
	this->list = nullptr;
	this->queue = nullptr;

	if (isHead) {
		l->head = n;
	}
}

void Node::add(VerticesByDegreeQueue *newQueue, VerticesLinkedList *newList) {
	Node *prevHead = newList->head;
	this->next = prevHead;
	if (prevHead != nullptr) {
		prevHead->prev = this;
	}
	newList->head = this;
	this->list = newList;
	this->queue = newQueue;
}

/**
 * Alternative to calling [remove] and [add].
 * This is slightly (but measurably) faster than doing so.
 */
void Node::move(VerticesLinkedList *newList) {
	VerticesLinkedList *oldL = this->list;
	Node *oldN = next;

	if (oldL->head == this) {
		oldL->head = oldN;
		if (oldN != nullptr) {
			oldN->prev = nullptr;
		}
	} else {
		Node *oldP = prev;
		if (oldP != nullptr) {
			oldP->next = oldN;
		}
		if (oldN != nullptr) {
			oldN->prev = oldP;
		}
		this->prev = nullptr;
	}

	Node *newHead = newList->head;
	this->next = newHead;
	if (newHead != nullptr) {
		newHead->prev = this;
	}
	newList->head = this;
	this->list = newList;
}


void Node::changeDegree(int newDegree) {
	int oldDegree = degree;
	if (newDegree != oldDegree) {
		degree = newDegree;
		if (this->queue != nullptr) {
			this->queue->changeDegree(this, newDegree);
		}
	}
}

void Node::resetDegree(const std::shared_ptr<Graph> &graph) {
	changeDegree(graph->connectionsMap[this->vertex.id].size());
}


#endif //C___VERTICESLINKEDLIST_H
