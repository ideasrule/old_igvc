#pragma once

/* Naming of variables and methods follow the paper "Anytime Dynamic A*: An Anytime, Replanning Algorithm" 
   by Likhachev, Ferguson et. al */

namespace DStar {

	struct Key {
		double k1;
		double k2;
	};

	class Node {
	public:
		Node(int x, int y, double g, double rhs);
		~Node();

		int x, y;
		double g;
		double rhs;
		double h;
		Key key;

		Node *oldStart;
		static Node *currStart;

		Node* bptr;   //previous node. used to optimize updates. See "Lifelong Planning A*" Koenig, Likhachev, 2005

		bool inOpen;
		bool inClosed;
		bool inIncons;
		bool used;

		static double epsilon;

		void recalcKey();
		int getIndex(int xMax) { return y * (xMax + 1) + x; }
		void reset();

		// greater/less than operators test inequalities of the key
		bool operator<(const Node&) const;
		bool operator>(const Node&) const;
		// equal/not equal operators test equality of coordinates x and y
		bool operator==(const Node &) const;
		bool operator!=(const Node &) const;

		static double (*heuristic)(int, int);

	};

	int getIndex(int x, int y, int xMax);

}