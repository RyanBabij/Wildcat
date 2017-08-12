#pragma once
#ifndef WILDCAT_INTERFACE_HASCLASSID_HPP
#define WILDCAT_INTERFACE_HASCLASSID_HPP

/*
	#include <Interface/HasClassID.hpp>

	Classes which inherit this class return a unique identifying string. The string is typically the name of their include guard.

	Useful for debugging a collection of polymorphic objects.

	For example, mouse input is being passed to a vector of classes which accept mouse input. One of them is modifying the mouse input, and I need to figure out which one it is.
*/

class HasClassID
{
	public:

	virtual std::string getClassID()
	{
		return "WILDCAT_INTERFACE_HASCLASSID_HPP";
	}
	
};

#endif
