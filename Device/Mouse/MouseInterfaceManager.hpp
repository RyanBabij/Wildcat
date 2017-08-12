#pragma once

/* #include <Device/Mouse/MouseInterfaceManager.hpp>
Container for MouseInterface objects. */

#include <Container/Vector/Vector.hpp>
#include <Device/Mouse/MouseInterface.hpp>

#include <typeinfo>

class MouseInterfaceManager
{
	public:
	Vector <MouseInterface*> vMouseInterface;
	/* Call render() on all objects. */
	void mouseEventAll(Mouse* mouse)
	{
		for (int i=0;i<vMouseInterface.size();++i)
		{
			vMouseInterface(i)->mouseEvent(mouse);
		}
	}
	
	/* Add a DisplayInterface object to the vector. */
	void add(MouseInterface* mouseInterface)
	{
		vMouseInterface.push(mouseInterface);
	}
	
};
