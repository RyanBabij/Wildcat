#pragma once

/* #include <Device/Display/DisplayInterfaceManager.hpp>
Container for DisplayInterface objects. */

#include <Container/Vector/Vector.hpp>
#include <Device/Display/DisplayInterface.hpp>

class DisplayInterfaceManager
{
	public:
	Vector <DisplayInterface*> vDisplayInterface;
	/* Call render() on all objects. */
	void renderAll()
	{
		for (int i=0;i<vDisplayInterface.size();++i)
		{
			vDisplayInterface(i)->render();
		}
	}
	
	/* Add a DisplayInterface object to the vector. */
	void add(DisplayInterface* displayInterface)
	{
		vDisplayInterface.push(displayInterface);
	}
	
	void remove(DisplayInterface* displayInterface)
	{
		vDisplayInterface.erase(displayInterface);
	}
	
	int size()
	{
		return vDisplayInterface.size();
	}
	
};
