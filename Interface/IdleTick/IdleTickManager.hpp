#pragma once
#ifndef IDLE_TICK_IDLE_TICK_MANAGER_HPP
#define IDLE_TICK_IDLE_TICK_MANAGER_HPP

/*
	#include <IdleTick/IdleTickManager.hpp>
	
	Idle tick manager. Handles objects that want to use idle time to perform tasks.

*/

class IdleTickManager
{
	public:
	/* Vector of objects that use the idleTick() function. */
	Vector <IdleTickInterface*> vIdleTick;
	
	/* Do logic ticks for all objects. */
	void tickAll()
	{
		for(int i=0;i<vIdleTick.size();++i)
		{
			vIdleTick(i)->idleTick();
		}
	}
	
	void add(IdleTickInterface* iti)
	{
		vIdleTick.push(iti);
	}
	
	
};

#endif
