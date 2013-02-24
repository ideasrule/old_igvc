#include "DStar.h"
#include "FloatingPointMath.h"

namespace DStar {

	Node* Node::currStart = 0;

	Node::Node(int x, int y, double g, double rhs)
		:x(x), y(y), g(g), rhs(rhs)
	{
		h = (*heuristic)(x, y);
		recalcKey();
		bptr = 0;
		inOpen = false;
		inClosed = false;
		inIncons = false;
		used = true;
		oldStart = currStart;
	}

	Node::~Node()
	{
	}


	void Node::recalcKey()
	{
		if (currStart != oldStart) {
			oldStart = currStart;
			h = (*heuristic)(x, y);
		}
		if (greater_than(g, rhs)) {     
			key.k1 = rhs + epsilon * h;
			key.k2 = rhs;
		} else {
			key.k1 = g + h;
			key.k2 = g;
		}
	}
	

	void Node::reset()
	{	
		bptr = 0;
		g = DOUBLE_INF;
		rhs = DOUBLE_INF;
		key.k1 = DOUBLE_INF;
		key.k2 = DOUBLE_INF;
		inOpen = false;
		inClosed = false;
		inIncons = false;
		used = false;
		h = (*heuristic)(x, y);
	}


	bool Node::operator<(const Node& node2) const
	{

		if (less_than(key.k1, node2.key.k1))
			return true;
		else if (greater_than(key.k1, node2.key.k1))
			return false;
		else {
			if (less_than(key.k2, node2.key.k2))
				return true;
			else
				return false;
		}
	}

	bool Node::operator>(const Node& node2) const
	{
		if (greater_than(key.k1, node2.key.k1))
			return true;
		else if (less_than(key.k1, node2.key.k1))
			return false;
		else {
			if (greater_than(key.k2, node2.key.k2))
				return true;
			else
				return false;
		}
	}

	bool Node::operator==(const Node& node2) const
	{
		return (this->x == node2.x && this->y == node2.y);
	}

	bool Node::operator!=(const Node& node2) const
	{
		return !(this->x == node2.x && this->y == node2.y);
	}

	int getIndex(int x, int y, int xMax)
	{
		return y * (xMax + 1) + x;
	}
}
			
