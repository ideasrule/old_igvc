#include "DStarPriorityQueue.h"
#include <vector>
#include <algorithm>

using std::vector;

namespace DStar {

	DStarPriorityQueue::DStarPriorityQueue(int capacity, int xMax)
	{
		if (capacity > 0)
			this->capacity = capacity;
		else
			capacity = 100;
		this->xMax = xMax;
		arrayIndices = new int[capacity];
		std::fill(arrayIndices, arrayIndices+capacity, -1);
	}

	DStarPriorityQueue::DStarPriorityQueue(Pave_Libraries_Navigation::CostMapInt *costmap)
	{
		capacity = costmap->getArraySize();
		xMax = costmap->getXMax();
		arrayIndices = new int[capacity];
		std::fill(arrayIndices, arrayIndices+capacity, -1);
	}



	void DStarPriorityQueue::setCostMap(Pave_Libraries_Navigation::CostMapInt *costmap)
	{
		int size = costmap->getArraySize();
		xMax = costmap->getXMax();
		if(size > capacity)
		{
			delete [] arrayIndices;
			arrayIndices = new int[size];
			std::fill(arrayIndices, arrayIndices+capacity, -1);
			capacity = size;
		}
		Array.clear();
	}


	vector<Node*> DStarPriorityQueue::clear()
	{
		size_t s = Array.size();
		for(size_t i = 0; i < s; i++)
			arrayIndices[i] = -1;
		vector<Node*> temp = Array;
		Array.clear();
		return temp;
	}



	DStarPriorityQueue::~DStarPriorityQueue()
	{
		delete [] arrayIndices;
	}



	void DStarPriorityQueue::add(Node *item)
	{
		Array.push_back(item);
		ArrayPtr = &Array[0];   
		arrayIndices[item->getIndex(xMax)] = (int)(Array.size() - 1);
		swim(int(Array.size() - 1));
		item->inOpen = true;
	}

	

	void DStarPriorityQueue::del(Node *item)
	{
		del(item->getIndex(xMax));
	}

	void DStarPriorityQueue::del(int x, int y)
	{
		del(getIndex(x, y, xMax));
	}

	void DStarPriorityQueue::del(int index)
	{
		int l = arrayIndices[index];
		if (l == -1) return;
		Array[l]->inOpen = false;
		swap(l, (int)Array.size()-1);
		int index2 = Array[l]->getIndex(xMax);
		Array.pop_back();
		swim(l);
		sink(arrayIndices[index2]);
		arrayIndices[index] = -1;
	}



	bool DStarPriorityQueue::hasBeenAdded(int index)
	{
		return arrayIndices[index] >= 0;
	}


	void DStarPriorityQueue::forAll(void (*func)(Node*, void*), void *param) 
	{
		for (size_t i = 0; i < Array.size(); ++i) {
			 (*func)(Array[i], param);
		}
	}



	void DStarPriorityQueue::update(int index)
	{
		sink(arrayIndices[index]);
		swim(arrayIndices[index]);
	}



	Node* DStarPriorityQueue::peekTop()
	{
		return Array.size() > 0 ? Array[0] : 0;
	}



	Node* DStarPriorityQueue::removeTop()
	{
		int size = (int)Array.size();
		if (size == 0)
			return NULL;
		else if (size == 1)
		{
			Node *result = Array[0];
			result->inOpen = false;
			Array.clear();
			arrayIndices[result->getIndex(xMax)] = -1;
			return result;
		}
		else
		{
			Node *result = Array[0];
			result->inOpen = false;
			swap(0, size - 1);
			Array.pop_back();
			sink(0);
			arrayIndices[result->getIndex(xMax)] = -1;
			return result;
		}
	}



	void DStarPriorityQueue::swim(int index)
	{
		int parent = getParent(index);
		if (parent >= 0)
		{
			if ( *ArrayPtr[index] < *ArrayPtr[parent])
			{
				swap(index, parent);
				swim(parent);
			}
		}
	}



	void DStarPriorityQueue::sink(int index)
	{
		int leftChild = getLeftChild(index);
		int rightChild = getRightChild(index);
		if (leftChild >= 0)
		{
			if (rightChild >= 0)
			{
				if (*ArrayPtr[rightChild] < *ArrayPtr[leftChild])
				{
					if ( *ArrayPtr[rightChild] < *ArrayPtr[index])
					{
						swap(index, rightChild);
						sink(rightChild);
					}
				}
				else
				{
					if ( *ArrayPtr[leftChild] < *ArrayPtr[index])
					{
						swap(index, leftChild);
						sink(leftChild);
					}
				}
			}
			else
			{
				if ( *ArrayPtr[leftChild] < *ArrayPtr[index])
				{
					swap(index, leftChild);
					sink(leftChild);
				}
			}
		}
	}


	/***********************************************************
	 Some debugging functions for the priority queue
	 **********************************************************/

	//void DStarPriorityQueue::print()
	//{
	//	int v = 0;
	//	for (int n = 0; n < 5; n++) {
	//		int k = 1;
	//		for (int i = 0; i < n; i++) 
	//			k *= 2;
	//		for (int i = v; i < k + v; i++) {
	//			if (i >= Array.size()) return;
	//			cout << Array[i]->key.k1 << ' ';
	//		}
	//		cout << endl;
	//		v += k;
	//	}
	//}



	//bool DStarPriorityQueue::checkConsistency()
	//{
	//	for (int i = 0; i < Array.size(); i++) {
	//		int parentIndex = getParent(i);
	//		if (parentIndex < 0) continue;
	//		if (*ArrayPtr[parentIndex] > *Array[i]) {
	//			cout << "Inconsistent heap!!!\n";
	//			return false;
	//		}
	//		//cout << "Consistency OK!" << endl;
	//	}

	//	return true;

	//}

}



