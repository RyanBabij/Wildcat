#pragma once
#ifndef LOGICTICK_LOGICTICKMANAGER_HPP
#define LOGICTICK_LOGICTICKMANAGER_HPP

/* Tick manager. Handles game logic 'ticks'. */
class LogicTickManager
{
	public:
	/* Vector of objects that use the logicTick() function. */
	Vector <LogicTickInterface*> vLogicTick;
	
	/* Do logic ticks for all objects. */
	void tickAll()
	{
		for(int i=0;i<vLogicTick.size();++i)
		{
			vLogicTick(i)->logicTick();
		}
	}
	
	void add(LogicTickInterface* lti)
	{
		vLogicTick.push(lti);
	}
	
	
};

#endif
