#pragma once
#ifndef TIME_TIMER_HPP
#define TIME_TIMER_HPP

/* #include <Time/Timer.hpp>
Linux Implementation (Seems to also work with mingw or whatever)

Note that tv_usec resets after each second.


Profiling example:

		Timer t;
		t.init();
		t.start();
			// CODE TO TIME
		t.update();
		std::cout<<"Execution time: "<<t.fullSeconds<<".\n";

*/


#include <sys/time.h>

class Timer
{
    public:

	timeval startTime;
	timeval stopTime;

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
	}
  void start()
  {
    gettimeofday(&startTime,0);
  }
	void update()
	{
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
	}
	
	// void addUSeconds(unsigned long int uSeconds)
	// {
		// uSeconds+=
	
	// }
	
};

#endif
