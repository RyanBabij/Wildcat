#pragma once

/* #include <Device/Keyboard/KeyboardInterface.hpp>
Any object that recieves keyboard input should inherit this class.
*/

class KeyboardInterface
{
	public:
	
	virtual bool keyboardEvent(Keyboard* _keyboard)
	{
		return false;
	}

};
