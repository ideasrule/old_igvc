#pragma once

#include "boost/shared_ptr.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_array.hpp"
#include "boost/scoped_array.hpp"
using boost::shared_ptr;
using boost::scoped_ptr;
using boost::shared_array;
using boost::scoped_array;

template<typename T>
class shared_ptr_nodelete_noopDeleter
{
public:
	void operator()(T *) {}
};

template<typename T>
shared_ptr<T> shared_ptr_nodelete(T *ptr)
{
	return shared_ptr<T>(ptr, shared_ptr_nodelete_noopDeleter<T>());
};
