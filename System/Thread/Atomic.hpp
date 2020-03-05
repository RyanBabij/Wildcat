#pragma once
#ifndef WILDCAT_SYSTEM_THREAD_ATOMIC_HPP
#define WILDCAT_SYSTEM_THREAD_ATOMIC_HPP

/* Wildcat: Atomic
#include <System/Thread/Atomic.hpp>

   Atomic wrapper to allow optional compile-time threading
   Class will compile as a normal datatype if threading is disabled.
*/

#ifdef WILDCAT_THREADING
   #include <atomic>
#endif


template <class T>
class Atomic
{
#ifdef WILDCAT_THREADING
   std::atomic <T> value;
#else
   T value;
#endif

   public:

};

#endif