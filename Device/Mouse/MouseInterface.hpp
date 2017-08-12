#pragma once

/* #include <Device/Mouse/MouseInterface.hpp>
Any object that recieves mouse input should inherit this class.
*/

#include <Device/Mouse/Mouse.hpp>

class MouseInterface
{
	public:

	virtual bool mouseEvent(Mouse* mouse)
	{
		return false;
	}
	
};
