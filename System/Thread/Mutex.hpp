#pragma once
#ifndef WILDCAT_SYSTEM_THREAD_MUTEX_HPP
#define WILDCAT_SYSTEM_THREAD_MUTEX_HPP

/* Wildcat: Mutex
#include <System/Thread/Mutex.hpp>

   Mutex wrapper to allow optional compile-time threading
   Class will only lock if threading is enabled with WILDCAT_THREADING
*/

class Mutex
{
#ifdef WILDCAT_THREADING
   std::mutex m;
#endif
   public:
   
   inline void lock()
   {
#ifdef WILDCAT_THREADING
      m.lock();
#endif
   }
   inline void unlock()
   {
#ifdef WILDCAT_THREADING
      m.unlock();
#endif
   }
};

#endif