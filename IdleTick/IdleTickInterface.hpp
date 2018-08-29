#pragma once
#ifndef IDLE_TICK_IDLE_TICK_INTERFACE_HPP
#define IDLE_TICK_IDLE_TICK_INTERFACE_HPP

/*
	#include <IdleTick/IdleTickInterface.hpp>

	When the program has idle time between rendering etc, it may go through and give these objects a chance to perform calculations.
	The objects should be nice and perform their calculations within a reasonable time window using Timers.
*/

class IdleTickInterface
{
	public:
	virtual void idleTick()
	{
	}
  
  ~IdleTickInterface() {}
};

#endif
