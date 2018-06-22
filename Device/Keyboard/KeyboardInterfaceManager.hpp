#pragma once

/* #include <Device/Keyboard/KeyboardInterfaceManager.hpp>
Container for KeyboardInterface objects. */

#include <Container/Vector/Vector.hpp>
#include <Device/Keyboard/KeyboardInterface.hpp>

class KeyboardInterfaceManager
{
	public:
	Vector <KeyboardInterface*> vKeyboardInterface;
	/* Call render() on all objects. */
	/* For now we are simply returning true on stolen input (prevent global hotkeys from using it)
		might expand it in the future. */
	bool keyboardEventAll(Keyboard* _keyboard)
	{
		for (int i=0;i<vKeyboardInterface.size();++i)
		{
			if ( vKeyboardInterface(i)->stealKeyboard())
			{
				vKeyboardInterface(i)->keyboardEvent(_keyboard);
				return true;
			}
		}
		//std::cout<<"non-stolen\n";
		for (int i=0;i<vKeyboardInterface.size();++i)
		{
			vKeyboardInterface(i)->keyboardEvent(_keyboard);
		}
		return false;
	}
	
	/* Add a DisplayInterface object to the vector. */
	void add(KeyboardInterface* keyboardInterface)
	{
		vKeyboardInterface.push(keyboardInterface);
	}
	
	void remove(KeyboardInterface* keyboardInterface)
	{
		vKeyboardInterface.erase(keyboardInterface);
	}
	
		/* Check if any objects want to steal keyboard. */
	bool stealKeyboard()
	{
		for (int i=0;i<vKeyboardInterface.size();++i)
		{
			if ( vKeyboardInterface(i)->stealKeyboard())
			{
				//vKeyboardInterface(i)->keyboardEvent(_keyboard);
				return true;
			}
		}
		return false;
	}
	
	void clear()
	{
		vKeyboardInterface.clear();
	}
	
};
