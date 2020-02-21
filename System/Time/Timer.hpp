#pragma once
#ifndef WILDCAT_SYSTEM_TIME_TIMER_HPP
#define WILDCAT_SYSTEM_TIME_TIMER_HPP

/* Wildcat: Timer
   #include <Time/Timer.hpp>
   
   Linux Implementation (Seems to also work with mingw or whatever)

   Note that tv_usec resets after each second.

   Profiling example:

   Timer t;
   t.init();
   t.start();
   // CODE TO TIME
   t.update();
   std::cout<<"Execution time: "<<t.fullSeconds<<".\n";
   
   GetTickCount() returns milliseconds since startup

*/

#if defined WILDCAT_LINUX || defined __MINGW32__
#include <sys/time.h> // I believe this is normally Linux only, but also works with MingW.
#else
#endif

class Timer
{
   public:
   
#if defined WILDCAT_LINUX || defined __MINGW32__
   timeval startTime;
   timeval stopTime;
#else
   unsigned long int startTick; // milliseconds since startup
#endif

   long unsigned int uSeconds; /* This goes only as far as 1 second before resetting. See documentation. */
   unsigned int seconds;
   double fullSeconds;

   long int totalUSeconds;

   Timer()
   {
      init();
   }
   void init()
   {
      uSeconds=0;
      seconds=0;
      fullSeconds=0;
      totalUSeconds=0;
#if defined WILDCAT_LINUX || defined __MINGW32__
#else
      startTick=0;
#endif
   }
   void start()
   {
#if defined WILDCAT_LINUX || defined __MINGW32__
      gettimeofday(&startTime,0);
#else
   startTick = GetTickCount();
#endif
   }
   void update()
   {
#if defined WILDCAT_LINUX || defined __MINGW32__
      gettimeofday(&stopTime,0);
      seconds=stopTime.tv_sec-startTime.tv_sec;

      totalUSeconds = stopTime.tv_usec-startTime.tv_usec + (seconds*1000000);
      if(stopTime.tv_usec<startTime.tv_usec)
      {
         stopTime.tv_usec+=1000000;
         seconds--;
      }
      uSeconds=stopTime.tv_usec-startTime.tv_usec;
      
      fullSeconds=seconds+((double)uSeconds/1000000);
#else
      long unsigned int endTick = GetTickCount();
      long unsigned int totalTicks = endTick-startTick;
      
      uSeconds = (totalTicks%1000)*1000;
      totalUSeconds = totalTicks*1000;
      seconds=totalTicks/1000;
      fullSeconds=seconds+((double)uSeconds/1000000);
#endif
   }

};

#endif
