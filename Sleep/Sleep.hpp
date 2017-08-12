#pragma once
#ifndef SLEEP_SLEEP_HPP
#define SLEEP_SLEEP_HPP

/*
#include <Sleep/Sleep.hpp>
Cross-platform sleep function.

For Windows it calls Sleep(milliseconds).
For Unix it calls usleep(microseconds).

A millisecond is 1/1,000th of a second.
A microsecond it 1/1,000,000th of a second.

Milliseconds should provide a poll-rate of around 1000 polls per second, which should be good enough for both Windows and Unix, and therefore both sleep functions are converted to milliseconds.

*/


#ifdef WILDCAT_WINDOWS
#include <windows.h>
#endif
#ifdef WILDCAT_LINUX
#include <unistd.h>
#endif

/* sleep(1000) = one second. */
void sleep(const int milliseconds)
{
	#ifdef WILDCAT_WINDOWS
		Sleep(milliseconds);
	#elif defined WILDCAT_LINUX
		usleep( milliseconds * 1000 );
	#else
	#error SLEEP_SLEEP_HPP needs WILDCAT_WINDOWS or WILDCAT_LINUX define.
	#endif
}

void sleep ( const double milliseconds )
{
	#ifdef WILDCAT_WINDOWS
		Sleep(milliseconds);
	#elif defined WILDCAT_LINUX
		usleep( milliseconds * 1000 );
	#else
	#error SLEEP_SLEEP_HPP needs WILDCAT_WINDOWS or WILDCAT_LINUX define.
	#endif
}

void sleepSeconds(const int seconds)
{ sleep(seconds*1000); }


#endif /* #ifndef SLEEP_SLEEP_HPP */
