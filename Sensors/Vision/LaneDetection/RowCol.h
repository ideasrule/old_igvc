#pragma once

struct RowCol
{
public:
	int row, col;

	RowCol() : row(0), col(0) {};
	RowCol(int r, int c) : row(r), col(c) {}

	bool operator < (const RowCol& rc) const
	{
		if (row < rc.row || (row == rc.row && col < rc.col))
			return true;
		else
			return false;
	}

};
