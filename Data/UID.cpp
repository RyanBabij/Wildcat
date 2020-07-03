#pragma once
#ifndef WILDCAT_DATA_UID_CPP
#define WILDCAT_DATA_UID_CPP

/* Wildcat: UID
#include <Data/UID.cpp> */

/**

Class which returns a unique incrementing number for any unique ID purposes. Currently it doesn't do anything fancy like
manage datatype limits and whatnot.

Todo: Threadsafe.

*/

template <class T> 
class UID
{
	public:
	T uid;
	
	UID()
	{
		// setting to 1 differentiate nulls.
		uid=1;
	}
	
	T get()
	{
		// return uid and increment.
		return uid++;
	}
};

#endif
