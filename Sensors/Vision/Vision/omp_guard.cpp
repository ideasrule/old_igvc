/** The class contained in this file is a guard object */
 
#include "omp_guard.hpp"
 
/** Construct guard object and acquire our lock */
omp_guard::omp_guard (omp_lock_t &lock) : lock_ (&lock)
    , owner_ (false)
{
    acquire ();
}
 
/** Explicitly set our lock */
void omp_guard::acquire ()
{
    omp_set_lock (lock_);
    owner_ = true;
}
 
/** Explicitly unset our lock.
* Only unset it, though, if we are still the owner.
*/
void omp_guard::release ()
{
    if (owner_) {
        owner_ = false;
        omp_unset_lock (lock_);
    }
}
 
/** Destruct guard object, release the lock */
omp_guard::~omp_guard ()
{
    release ();
}