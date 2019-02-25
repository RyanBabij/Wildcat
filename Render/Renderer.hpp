#pragma once
#ifndef RENDER_RENDERER_HPP
#define RENDER_RENDERER_HPP

/* #include <Render/Renderer.hpp> Created: 0221683187. Updated: 0221683187. Component of GaroSoft WildCat game engine.
	Static class to link generic rendering calls to either OpenGL or Direct3D API at compile time or runtime. Currently only OpenGL is supported.
	Replaces <Device/Display/OpenGLTools.hpp>.
*/

class Texture;
class Colour;

#include <Interface/HasTexture.hpp>

class Renderer
{
	private:
		#ifdef WILDCAT_USE_OPENGL
			static bool textureMode;
			static bool colourMode;
			static GLint savedViewPort[4]; /* This keeps track of the previous viewport coordinates. */
		#endif

	public:
	
	/* Save the current viewport so it can be quickly restored later. */
	static void saveViewPort();
	static void saveViewport() { saveViewPort(); }

	/* Restore the viewport to the previously saved coordinates. I'm not sure if this code is correct, but it seems to work okay. I'll leave the mess here for now. */
	static void restoreViewPort();
	static void restoreViewport() { restoreViewPort(); }

	/* This function recieved the most attention. I'm not so sure about the whole saving and restoring part, but it seems to work okay right now.
		My viewport functions take 2 coordinates, which is different to the usual way viewports are handled in OpenGL. This is just due to the way my engine works. */
	static void resizeViewPort(const int _x1, const int _y1, const int _x2, const int _y2);
	static void resizeViewport(const int _x1, const int _y1, const int _x2, const int _y2) { resizeViewPort(_x1,_y1,_x2,_y2); }
	
	
	static void resetColour();
	static void resetColor() { resetColour(); }
	
	static void setTextureMode();
	
	static void setColourMode();
	static void setColorMode() { setColourMode(); }


	/* There was also a function like this which took 'const GLuint _texture' instead of 'const Texture* _texture'. I think I'll try to stick with using the Texture object from now on. */
	static void placeTexture8(const int _x1, const int _y1, const int _x2, const int _y2, const int _x3, const int _y3, const int _x4, const int _y4, const Texture* _texture);
	
	
	/* Render a square texture rotated about its center by a certain number of degrees. */
	static void placeTexture4RotatedDegrees(const int _x1, const int _y1, const int _x2, const int _y2, const Texture* _texture, const int rotationDegrees);
	
/*
	placeTexture4: Render an upright square texture.
	bool preserveAspectRatio true will ensure the texture fills the given coordinates, but will not alter the aspect ratio. This means that some of the texture could overflow out of the intended area, or get cut off. The texture will be centered inside the given coordinates.
	This function uses the Texture class, as querying OpenGL texture dimensions can apparently hang the GPU, depending on the driver. Therefore creating a wrapper for the OpenGL textures seems to be a common practice. A good driver would be expected to cache the texture specs, but this cannot be guaranteed.
	Anyway, this new method is recommended for new texture loading, but I'll keep the old function around to avoid a thousand compile errors for now. Where practical, GLuints should now be converted to Texture objects.
*/
	//static void placeTexture4(const int _x1, const int _y1, const int _x2, const int _y2, const Texture* _texture, const bool preserveAspectRatio=false);
	
	//0231723688: UPDATED TO WORK WITH HASTEXTURE (TEXTURE IMPLEMENTS HASTEXTURE).
	static void placeTexture4(const int _x1, const int _y1, const int _x2, const int _y2, const Texture* _texture, const bool preserveAspectRatio=false);
		static void placeTexture4(const int _x1, const int _y1, const int _x2, const int _y2, HasTexture* _texture, const bool preserveAspectRatio=false);

	static void placeTextureCentered(const int _centerX, const int _centerY, HasTexture* _texture);

	
	static void placeColour4 (const unsigned char r, const unsigned char g, const unsigned char b, const int _x1, const int _y1, const int _x2, const int _y2);
	static void placeColor4 (const unsigned char r, const unsigned char g, const unsigned char b, const int _x1, const int _y1, const int _x2, const int _y2)
	{ placeColour4(r,g,b,_x1,_y1,_x2,_y2); }
	
	static void placeColour4a (const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a, const int _x1, const int _y1, const int _x2, const int _y2);
	
	static void placeColour4 (Colour* colour, const int _x1, const int _y1, const int _x2, const int _y2);
  
    // Draws a rectangle using lines.
  static void placeBorder4(const unsigned char r, const unsigned char g, const unsigned char b, const int _x1, const int _y1, const int _x2, const int _y2);
/* End of class. */
};

#endif
