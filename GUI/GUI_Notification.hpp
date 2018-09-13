#pragma once
#ifndef WILDCAT_GUI_GUI_NOTIFICATION_HPP
#define WILDCAT_GUI_GUI_NOTIFICATION_HPP

/* Wildcat: GUI_Notification
	#include <GUI/GUI_Notification.hpp>
	
	A notification is a popup with some text information, optionally a picture, and one or more buttons.
*/

#include <GUI/GUI_Interface.hpp>

class GUI_Notification: public GUI_Interface
{
	private:

	public:

	bool active;


	GUI_Notification() : GUI_Interface() /* super */
	{
    active = false;
	}

	void render()
	{
    if ( active == false )
    { return; }
	}

	bool mouseEvent ( Mouse* mouse)
	{
    if (active == false )
    { return false; }

		return false;
	}


};

#endif
