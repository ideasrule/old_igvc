/*****************************************************************************
 *  Path2DExceptions.h
 *
 *  Contains several classes of exceptions used in the Path2D class
 *  
 *  These exceptions are thrown in the Path2D class, as mentioned in the
 *  comments in the Point2D.h file. It is possible for other classes to use
 *  these exceptions, but please let me (the author) know becuase in that case
 *  this file should be given another name and moved to the common namespace.
 *  Actually that seems like a good idea... we'll leave that for another time.
 *
 *  Dependancies: exception
 *
 *  Author: Derrick Yu (ddyu@)
 *****************************************************************************/

#include <exception>

namespace Pave_Libraries_Geometry {
    // If your index is somehow below zero, this happens.
    class IndexNegativeException : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "One of the input indices is negative";
        }
    };

    // If your index exceeds the end of the array, this happens.
    class IndexExceedsLengthException : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "One of the input indices exceeds array length";
        }
    };

    // Pretty self explanitory here...
    class InvalidInputException : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Invalid function input";
        }
    };

    // Indicates that the path has too few points to accurately perform
    // a calulation
    class InvalidSize : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Path is too small for calculation";
        }
    };

    // Thrown when a call to getClosestIndex functions evaluate to -1, meaning
    // that the point does not exist
    class PointNonexistantException : public std::exception
    {
        virtual const char* what() const throw()
        {
            return "Invalid calculation or point does not exist."
                   "Check path size";
        }
    };
}  // namespace Pave_Libraries_Geometry 