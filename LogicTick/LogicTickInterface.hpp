#pragma once
#ifndef LOGICTICK_LOGICTICKINTERFACE_HPP
#define LOGICTICK_LOGICTICKINTERFACE_HPP

/* When an object periodically interacts with the world, it typically does this using 'ticks', which are a chance to perform their AI and whatnot. Such an object inherits the logicTick() function and uses that. The tick manager calls the ticks. I would like to have this running in the background as a thread. */
class LogicTickInterface
{
	public:
	virtual void logicTick()
	{
	}
  
  ~LogicTickInterface() {}
};

#endif
