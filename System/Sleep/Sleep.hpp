#pragma once
#ifndef SYSTEM_SLEEP_SLEEP_HPP
#define SYSTEM_SLEEP_SLEEP_HPP

/* Wildcat: Sleep
#include <System/Sleep/Sleep.hpp> */

/** Cross-platform sleep function.

For Windows it calls Sleep(milliseconds).
For Unix it calls usleep(microseconds).

A millisecond is 1/1,000th of a second.
A microsecond it 1/1,000,000th of a second.

Milliseconds should provide a poll-rate of around 1000 polls per second, which should be good enough for both Windows and Unix, and therefore both sleep functions are converted to milliseconds.

In Windows, Sleep(0) will make the thread relinquish its remaining time to any other threads which need to run.

*/

#ifdef WILDCAT_WINDOWS
#include <windows.h>
#endif
#ifdef WILDCAT_LINUX
#include <unistd.h>
#endif

// prevent accidentally using the OS-specific sleep functions
#define Sleep_Windows Sleep
#define Sleep_Linux usleep
#undef Sleep
#undef usleep

/* sleep(1000) = one second. */
void sleep(const unsigned int milliseconds)
{
	#ifdef WILDCAT_WINDOWS
		Sleep_Windows(milliseconds);
	#elif defined WILDCAT_LINUX
		Sleep_Linux( milliseconds * 1000 );
	#else
		#error SLEEP_SLEEP_HPP needs WILDCAT_WINDOWS or WILDCAT_LINUX define.
	#endif
}

void sleepSeconds(const unsigned int seconds)
{ sleep(seconds*1000); }

#endif /* #ifndef SLEEP_SLEEP_HPP */
