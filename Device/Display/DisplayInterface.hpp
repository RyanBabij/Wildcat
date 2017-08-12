#pragma once

/* #include <Device/Display/DisplayInterface.hpp>
Display is the medium for displaying visual output, typically a monitor. This class provides a render function so a class may send visual output.
*/


class DisplayInterface
{
	public:

	virtual void render()=0;

};
