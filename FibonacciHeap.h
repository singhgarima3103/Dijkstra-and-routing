#ifndef __FIBONACCI_HEAP
#define __FIBONACCI_HEAP


#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;
// node of Heap
class FibHeapNode {
	int nodeKey; //value stored at node
	int nodeLabel; //node label
	int degree; // Number of children
	bool mark; // childCut field 
	FibHeapNode* previous;// sibling pointers for circular doubly linked list
	FibHeapNode* next;
	FibHeapNode* child;
	FibHeapNode* parent;
	FibHeapNode() {}
	FibHeapNode(int v, int c) :nodeKey(c), nodeLabel(v), degree(0), mark(false), child(NULL), parent(NULL)
	{
		previous = next = this;
	}

	bool isSingle() const {
		return (this == this->next);
	}

	// Inserts other node after this node
	void insert(FibHeapNode* other) {
		if (!other)
			return;
		this->next->previous = other->previous;//adjust both previous and next pointers
		other->previous->next = this->next;
		this->next = other;
		other->previous = this;
	}

	// Remove this node from sibling list
	void remove() {
		this->previous->next = this->next;
		this->next->previous = this->previous;//adjust both previous and next pointers
		this->next = this->previous = this;
	}

	// Add a child
	void addChild(FibHeapNode* other) {
		if (!child)
			child = other;
		else
			child->insert(other);
		other->parent = this; //set parent to this current node
		other->mark = false; //set child cut to false
		degree++; //increment degree
	}

	// Remove Child
	void removeChild(FibHeapNode* other) {
		if (other->parent != this) {
			cout << "F-heap Error: Trying to remove a child from a non-parent" << endl;
			exit(EXIT_FAILURE);
		}
		if (other->isSingle()) {
			if (child != other) {
				cout << "F-heap Error: Trying to remove a non-child" << endl;
				exit(EXIT_FAILURE);
			}
			child = NULL;
		}
		else {
			if (child == other)
				child = other->next;
			other->remove(); // from list of children
		}
		other->parent = NULL;
		other->mark = false;
		degree--; //decrease degree
	}

	friend ostream& operator<< (ostream& out, const FibHeapNode& n) {
		return (out << n.nodeLabel << ":" << n.nodeKey);
	}

public:
	int key() const { return nodeKey; }
	int data() const { return nodeLabel; }
	friend class FibonacciHeap;

};


//Heap
class FibonacciHeap {
	typedef FibHeapNode* fNode;
	fNode rootWithMinKey;//used like an entry point to the heap
	int count;// Total number of elements in heap
	int maxDegree;// Maximum degree of a root in the circular d-list

protected:

	// Insert a node into the HeapNode
	fNode insertNode(fNode newNode) {
		if (!rootWithMinKey) {
			// Insert the first node to the heap
			rootWithMinKey = newNode;
		}
		else {
			rootWithMinKey->insert(newNode);
			if (newNode->key() < rootWithMinKey->key())
				rootWithMinKey = newNode;
		}
		return newNode;
	}

public:
	FibonacciHeap() :
		rootWithMinKey(NULL), count(0), maxDegree(0) {}

	bool empty() const { return count == 0; }

	// Gives the pointer to minimum element
	fNode minimum() const {
		if (!rootWithMinKey) {
			cout << "F-heap Error: No min element" << endl;
			exit(EXIT_FAILURE);
		}
		return rootWithMinKey;
	}

	// Insert an element into the heap
	fNode insert(int data, int key) {
		count++;
		return insertNode(new FibHeapNode(data, key));
	}

	void removeMin() {
		if (!rootWithMinKey) {
			cout << "F-heap Error: trying to remove from an empty heap" << endl;
			exit(EXIT_FAILURE);
		}
		count--;
		// Make all children of root new roots
		if (rootWithMinKey->child) {
			fNode c = rootWithMinKey->child;
			do {
				c->parent = NULL;
				c = c->next;
			} while (c != rootWithMinKey->child);
			// Removed all children
			rootWithMinKey->child = NULL;
			rootWithMinKey->insert(c);
		}

		// Handle the case where we delete the last key
		if (rootWithMinKey->next == rootWithMinKey) {
			if (count != 0) {
				cout << "F-heap Error: Internal error: should have 0 keys" << endl;
				exit(EXIT_FAILURE);
			}
			rootWithMinKey = NULL;
			return;
		}
		// Merge roots with the same degree - CONSOLIDATE
		// Make room for a new degree
		vector<fNode> degreeRoots(maxDegree+1);
		fill(degreeRoots.begin(), degreeRoots.end(), (fNode)NULL);
		maxDegree = 0;
		fNode currentPointer = rootWithMinKey->next;
		int currentDegree;
		do {
			currentDegree = currentPointer->degree;
			fNode current = currentPointer;
			currentPointer = currentPointer->next;
			while (degreeRoots[currentDegree]) {
				// Merge the two roots with the same degree
				fNode other = degreeRoots[currentDegree];
				if (current->key() > other->key())
					swap(other, current);

				// Now current->key() <= other->key() so, make other a child of current
				// remove from list of roots
				other->remove();
				current->addChild(other);
				degreeRoots[currentDegree] = NULL;
				currentDegree++;
				if (currentDegree >= degreeRoots.size())
					degreeRoots.push_back((fNode)NULL);
			}
			// keep the current root as the first of its degree in the degrees vector
			degreeRoots[currentDegree] = current;
		} while (currentPointer != rootWithMinKey);

		// Remove the current root, and calculate the new rootWithMinKey
		delete rootWithMinKey;
		rootWithMinKey = NULL;
		int newMaxDegree = 0;
		for (int d = 0; d < degreeRoots.size(); d++) {
			if (degreeRoots[d]) {
				degreeRoots[d]->next = degreeRoots[d]->previous = degreeRoots[d];
				insertNode(degreeRoots[d]);
				if (d > newMaxDegree)
					newMaxDegree = d;
			}
		}
		maxDegree = newMaxDegree;
	}

	void decreaseKey(fNode node, int newKey) {
		if (newKey >= node->nodeKey) {
			cout << "Trying to decrease key to a greater key" << endl;
			exit(EXIT_FAILURE);
		}
		// Update the key and possibly the min key
		node->nodeKey = newKey;
		// Check that min pointer always points to right value
		fNode parent = node->parent;
		// Root node - just make sure the minimum is correct
		if (!parent) {
			if (newKey < rootWithMinKey->key())
				rootWithMinKey = node;
			return;
		}
		else if (parent->key() <= newKey)
		{
			return;
		}
		// Cascading cut
		while (true){
			parent->removeChild(node);
			insertNode(node);
			// Parent is a root - nothing more to do
			if (!parent->parent) {
				break;
			}
			else if (!parent->mark)
			{
				// Parent is not a root and is not marked - just mark it
				parent->mark = true;
				break;
			}
			else
			{
				node = parent;
				parent = parent->parent;
				continue;
			}
		}
	}
};

#endif __FIBONACCI_HEAP
