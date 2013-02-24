#ifndef OMP_GUARD_HPP_
#define OMP_GUARD_HPP_

// Copied from http://www.thinkingparallel.com/2006/08/21/scoped-locking-vs-critical-in-openmp-a-personal-shootout/
// Maaad awesome.
// ~ Brenton Partridge '12

#include <omp.h>
 
/** This is a class for guard objects using OpenMP
*  It is adapted from the book
*  "Pattern-Oriented Software Architecture". */
class omp_guard {
public:
    /** Acquire the lock and store a pointer to it */
    omp_guard (omp_lock_t &lock);
    /** Set the lock explicitly */
    void acquire ();
    /** Release the lock explicitly (owner thread only!) */
    void release ();
    /** Destruct guard object */
    ~omp_guard ();
 
private:
    omp_lock_t *lock_;  // pointer to our lock
    bool owner_;   // is this object the owner of the lock?
   
    // Disallow copies or assignment
    omp_guard (const omp_guard &);
    void operator= (const omp_guard &);
};
 
#endif /*OMP_GUARD_HPP_*/