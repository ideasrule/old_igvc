#pragma once

namespace Pave_Libraries_Navigation {

// This class uniformly maps a continuous array of floating point values
//   to discrete array indices (cells).
// Clients access the array with a pair of float values, and a
//   subarray of the cells that lie between them (inclusive).
template<typename T>
class FloatIndexedArray
{
public:
    typedef T T;
    typedef T * T_Ptr;  // for notational convenience, taking references
    typedef const T * T_Ptr_const;  // for notational convenience, taking references

    // the number of discrete cells, and the min and max floating point values
    FloatIndexedArray(int numberOfCells, double min, double max) :
       N(numberOfCells), minIndex_(min), maxIndex_(max)
   {
       data = new T[N];
   }

    // The subset of cells between lo and hi
    // Parameter ptr is set to point to the first element, and the number of elements
    //   is returned.
    // NOT a copy, so clients can edit the array
    int cells(double lo, double hi, T_Ptr& ptr)
    {
        int loInt = floatToIndex(lo);  // inclusive
        int hiInt = floatToIndex(hi);  // inclusive
        
        ptr = data + loInt;
        return hiInt - loInt + 1;
    }

    // The subset of cells between lo and hi
    // Parameter ptr is set to point to the first element, and the number of elements
    //   is returned.
    // NOT a copy, so clients can edit the array
    int cells(double lo, double hi, T_Ptr_const& ptr) const
    {
        int loInt = floatToIndex(lo);  // inclusive
        int hiInt = floatToIndex(hi);  // inclusive
        
        ptr = data + loInt;
        return hiInt - loInt + 1;
    }

    // Returns the cell corresponding to the range containing value.
    // NOT a copy, so clients can edit the array
    T& cell(double value)
    {
        return data[floatToIndex(value)];
    }

    // Returns the cell corresponding to the range containing value.
    // NOT a copy, so clients can edit the array
    const T& cell(double value) const
    {
        return data[floatToIndex(value)];
    }

	// Returns the minimum and maximum allowed indices
	double minIndex() const { return minIndex_; }
	double maxIndex() const { return maxIndex_; }

    ~FloatIndexedArray() { delete[] data; }

private:
    int floatToIndex(double d) const {
		// correct for numerical reasons
		if(d < minIndex()) d = minIndex();
		if(d > maxIndex()) d = maxIndex();

		double effectiveN = (double) N - 1e-10; // returned index must be < N
        return (int) ((d - minIndex_) * effectiveN / (maxIndex_ - minIndex_));
    }

    int N;
    double minIndex_, maxIndex_;
    T *data;
};

} // namespace Pave_Libraries_Navigation