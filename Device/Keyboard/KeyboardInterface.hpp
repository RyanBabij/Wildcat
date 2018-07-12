#pragma once

/* #include <Device/Keyboard/KeyboardInterface.hpp>
Any object that recieves keyboard input should inherit this class.
*/

class Keyboard;

class KeyboardInterface
{
	public:
	
	virtual bool keyboardEvent(Keyboard* _keyboard)
	{
		return false;
	}
	
		/* New feature. Sometimes a GUI control will want to steal all input (except a small number of special commands).
		Upon any keyboard event, the manager will look for any control which returns true here, and if so, will provide
		the event only to that control.
		*/
	virtual bool stealKeyboard()
	{
		return false;
	}

};
