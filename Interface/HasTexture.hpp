#pragma once
#ifndef WILDCAT_HAS_TEXTURE_HPP
#define WILDCAT_HAS_TEXTURE_HPP

/*
	#include <Interface/HasTexture.hpp>

	Classes which inherit from this interface are expected to be able to provide a texture for rendering.
	A function is used instead of a texture object, because I think it is easier to simply assess the
	texture to return on demand, rather than continuously update pointers and whatnot.
*/

class Texture;

class HasTexture
{
	public:
	// The object must return the current texture to be displayed when this is called.
	// THE TEXTURE DOESN'T HAVE TO ALWAYS BE THE SAME ONE. THIS ALLOWS FOR ANIMATIONS AND WHATNOT.
	//virtual Texture* currentTexture() { return 0; }
	virtual Texture* currentTexture() =0;

		// THE MINIMUM SIZE THE TEXTURE CAN BE DRAWN AT. UNLESS SET TO 1, IT SHOULD BE DIVISIBLE BY 2.
	virtual int getMinSize() { return 1; }
};

#endif
