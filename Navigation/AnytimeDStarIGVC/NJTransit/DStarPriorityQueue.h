#pragma once

#include "DStar.h"
#include "CostMapInt.h"
#include <vector>

namespace DStar {

	class DStarPriorityQueue
	{
	public:
		DStarPriorityQueue(int capacity = 100, int xMax = 0);
		DStarPriorityQueue(Pave_Libraries_Navigation::CostMapInt *costmap);

		void setCostMap(Pave_Libraries_Navigation::CostMapInt *costmap);


		~DStarPriorityQueue();


		std::vector<Node*> clear();
		void add(Node *item);

		void del(Node *item);
		void del(int x, int y);
		
		bool hasBeenAdded(Node *item) { hasBeenAdded(item->getIndex(xMax)); }
		bool hasBeenAdded(int x, int y) { hasBeenAdded(getIndex(x, y, xMax)); }
		
		void update(Node *item) { update(item->getIndex(xMax)); }
		void update(int x, int y) { update(getIndex(x, y, xMax)); }
		
		Node* peekTop();    //return index
		
		Node* removeTop();

		bool isEmpty()
		{
			return Array.size() == 0;
		}

		void forAll( void (*func)(Node *, void *), void *param = 0 );

		//Some debugging utilities
		//void print();
		//bool checkConsistency();

	private:
		std::vector<Node*> Array;
		Node **ArrayPtr;
		int capacity;
		int *arrayIndices;

		int xMax;

		void del(int index);
		bool hasBeenAdded(int index);
		void update(int index);



		int getParent(int index)
		{
			if(index == 0)
				return -1;
			return (index - 1) / 2;
		}

		int getLeftChild(int index)
		{
			int result = 2 * index + 1;
			if (result > (int)Array.size() - 1)
				return -1;
			return result;
		}

		int getRightChild(int index)
		{
			int result = 2 * index + 2;
			if (result > (int)Array.size() - 1)
				return -1;
			return result;
		}

		void swap(int index1, int index2)
		{
			Node *temp = ArrayPtr[index1];
			ArrayPtr[index1] = ArrayPtr[index2];
			ArrayPtr[index2] = temp;
			arrayIndices[ArrayPtr[index1]->getIndex(xMax)] = index1;
			arrayIndices[ArrayPtr[index2]->getIndex(xMax)] = index2;
		}

		bool isLeftChild(int index)	{ return (index % 2) != 0; }

		bool isRightChild(int index) { return (index % 2) == 0; }

		bool isRoot(int index) { return (index == 0); }

		void swim(int index);

		void sink(int index);
	};

}