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
	void keyboardEventAll(Keyboard* _keyboard)
	{
		for (int i=0;i<vKeyboardInterface.size();++i)
		{
			vKeyboardInterface(i)->keyboardEvent(_keyboard);
		}
	}
	
	/* Add a DisplayInterface object to the vector. */
	void add(KeyboardInterface* keyboardInterface)
	{
		vKeyboardInterface.push(keyboardInterface);
	}
	
};
