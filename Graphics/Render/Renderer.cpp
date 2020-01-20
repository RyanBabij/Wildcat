#pragma once
#ifndef WILDCAT_RENDER_RENDERER_CPP
#define WILDCAT_RENDER_RENDERER_CPP

/* #include <Render/Renderer.hpp>
	Implementation of Renderer.hpp
*/

#include "Renderer.hpp"

/* STL libs */
/* Internal libs */
#include <Graphics/Texture/Texture.hpp> /* For texture rendering. */
#include <Math/Geometry/Geometry.hpp> /* For rotations and coordinates. */
#include <Graphics/Colour/Colour.hpp> /* For rendering colours. */

#include <Debug/DebugMacro.hpp>

/* Initialise static variables. */
#ifdef WILDCAT_USE_OPENGL
	bool Renderer::textureMode=false;
	bool Renderer::colourMode=false;
	GLint Renderer::savedViewPort[4]={0,0,0,0};
#elif defined WILDCAT_USE_DIRECT3D
	/* Put Direct3D code here... */
#endif


/* Save the current viewport so it can be quickly restored later. */
void Renderer::saveViewPort()
{
	#ifdef WILDCAT_USE_OPENGL
		glGetIntegerv( GL_VIEWPORT, savedViewPort );
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

/* Restore the viewport to the previously saved coordinates. I'm not sure if this code is correct, but it seems to work okay. I'll leave the mess here for now. */
void Renderer::restoreViewPort()
{
	#ifdef WILDCAT_USE_OPENGL
		/* Update projection matrix */
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(savedViewPort[0],savedViewPort[1],savedViewPort[2],savedViewPort[3]);
		gluOrtho2D(savedViewPort[0],savedViewPort[2],savedViewPort[1],savedViewPort[3]);
		/* Go back to default matrix mode. */
		glMatrixMode(GL_MODELVIEW);
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

/* This function recieved the most attention. I'm not so sure about the whole saving and restoring part, but it seems to work okay right now.
	My viewport functions take 2 coordinates, which is different to the usual way viewports are handled in OpenGL. This is just due to the way my engine works. */
void Renderer::resizeViewPort(const int _x1, const int _y1, const int _x2, const int _y2)
{
	#ifdef WILDCAT_USE_OPENGL
		/* Note that the glViewport wants the bottom-left point and then the width and height, whereas gluOrtho2D wants x1,x2,y1,y2. */
		const int _nX = _x2-_x1;
		const int _nY = _y2-_y1;
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		/* Modify viewport, so that only the selected area of the screen can be drawn on. */
		glViewport(_x1,_y1,_nX,_nY);
		/* Modify the projection so that there's still a 1:1 pixel to coordinate scale. */
		gluOrtho2D(0,_nX,0,_nY);
		/* Go back to default matrix mode. */
		glMatrixMode(GL_MODELVIEW);
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

void Renderer::resetColour()
{
	#ifdef WILDCAT_USE_OPENGL
		glColor4ub(255,255,255,255);
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

void Renderer::setColour(Colour colour)
{
	#ifdef WILDCAT_USE_OPENGL
		glColor4ub(colour.red,colour.green,colour.blue,colour.alpha);
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif

}

void Renderer::setTextureMode()
{
	#ifdef WILDCAT_USE_OPENGL
		if (textureMode==false)
		{
			textureMode=true;
			colourMode=false;
			glEnable(GL_TEXTURE_2D);
			resetColour();
		}
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

void Renderer::setColourMode()
{
	#ifdef WILDCAT_USE_OPENGL
		/* We can't draw coloured shapes without turning off textures. */
		if(colourMode==false)
		{
			colourMode=true;
			textureMode=false;
			glDisable(GL_TEXTURE_2D);
			resetColour();
		}
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

/* There was also a function like this which took 'const GLuint _texture' instead of 'const Texture* _texture'. I think I'll try to stick with using the Texture object from now on. */
void Renderer::placeTexture8(const int _x1, const int _y1, const int _x2, const int _y2, const int _x3, const int _y3, const int _x4, const int _y4, const Texture* _texture)
{
	#ifdef WILDCAT_USE_OPENGL
	setTextureMode();
	
	glBindTexture( GL_TEXTURE_2D, _texture->textureID);
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex2f(_x1,_y1);
		glTexCoord2f(1,1);
		glVertex2f(_x2,_y2);
		glTexCoord2f(1,0);
		glVertex2f(_x3,_y3);
		glTexCoord2f(0,0);
		glVertex2f(_x4,_y4);
	glEnd();
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

#include <math.h>

/* Render a square texture rotated about its center by a certain number of degrees. */
void Renderer::placeTexture4RotatedDegrees(const int _x1, const int _y1, const int _x2, const int _y2, const Texture* _texture, const int rotationDegrees)
{
	#ifdef WILDCAT_USE_OPENGL
		setTextureMode();
	
		/* Turn the 2 points into a square object. */
		Square <double> square (_x1,_y1,_x2,_y2);
		/* Rotate the square object. */
		square.rotateAboutCenterDegrees(rotationDegrees);

		glBindTexture( GL_TEXTURE_2D, _texture->textureID);
		glBegin(GL_QUADS);
		
			glTexCoord2f(0,1);
			glVertex2f(round(square.p1.x),round(square.p1.y));
			glTexCoord2f(1,1);
			glVertex2f(round(square.p2.x),round(square.p2.y));
			glTexCoord2f(1,0);
			glVertex2f(round(square.p3.x),round(square.p3.y));
			glTexCoord2f(0,0);
			glVertex2f(round(square.p4.x),round(square.p4.y));
		glEnd();
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

/*
placeTexture4: Render an upright square texture.
bool preserveAspectRatio true will ensure the texture fills the given coordinates, but will not alter the aspect ratio. This means that some of the texture could overflow out of the intended area, or get cut off. The texture will be centered inside the given coordinates.
This function uses the Texture class, as querying OpenGL texture dimensions can apparently hang the GPU, depending on the driver. Therefore creating a wrapper for the OpenGL textures seems to be a common practice. A good driver would be expected to cache the texture specs, but this cannot be guaranteed.
Anyway, this new method is recommended for new texture loading, but I'll keep the old function around to avoid a thousand compile errors for now. Where practical, GLuints should now be converted to Texture objects.
*/
void Renderer::placeTexture4(const int _x1, const int _y1, const int _x2, const int _y2, const Texture* _texture, const bool preserveAspectRatio /* Default false. */)
{
	if (_texture == 0)
	{ return; }
	#ifdef WILDCAT_USE_OPENGL
		setTextureMode();
	
		if(preserveAspectRatio==false)
		{
			glBindTexture( GL_TEXTURE_2D, _texture->textureID);
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);
				glVertex2f(_x1,_y1);
				glTexCoord2f(1,1);
				glVertex2f(_x2,_y1);
				glTexCoord2f(1,0);
				glVertex2f(_x2,_y2);
				glTexCoord2f(0,0);
				glVertex2f(_x1,_y2);
			glEnd();
		}
		else
		{
			/* Stretch the image to fit, but preserve aspect ratio. */
			
			const double textureAspectRatio = (double)_texture->nX/_texture->nY;
			const int selectionNX = _x2-_x1;
			const int selectionNY = _y2-_y1;
			const double selectionAspectRatio = (double) selectionNX/selectionNY;

			/* higher number == wider. */
			if(selectionAspectRatio==textureAspectRatio)
			{
				glBindTexture( GL_TEXTURE_2D, _texture->textureID);
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex2f(_x1,_y1);
					glTexCoord2f(1,1);
					glVertex2f(_x2,_y1);
					glTexCoord2f(1,0);
					glVertex2f(_x2,_y2);
					glTexCoord2f(0,0);
					glVertex2f(_x1,_y2);
				glEnd();
			}
			/* Coords are too wide. */
			else if(selectionAspectRatio>textureAspectRatio)
			{
				/* Find required width. */
				const int requiredWidth = selectionNX;
				/* Find height that matches aspect ratio. */
				const int requiredHeight = selectionNX*(1/textureAspectRatio);
				/* Needs to be centered vertically. */
				const int centerY = ((_y2-_y1) / 2) + _y1;
				
				glBindTexture( GL_TEXTURE_2D, _texture->textureID);
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex2f(_x1,centerY - (requiredHeight/2));
					glTexCoord2f(1,1);
					glVertex2f(_x2,centerY - (requiredHeight/2));
					glTexCoord2f(1,0);
					glVertex2f(_x2,centerY + (requiredHeight/2));
					glTexCoord2f(0,0);
					glVertex2f(_x1,centerY + (requiredHeight/2));
				glEnd();
				
				
			}
			else
			{
				/* Find required width. */
				const int requiredWidth = selectionNY*textureAspectRatio;
				const int centerX = ((_x2-_x1) / 2) + _x1;
				
				glBindTexture( GL_TEXTURE_2D, _texture->textureID);
				glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex2f(centerX - (requiredWidth/2),_y1);
					glTexCoord2f(1,1);
					glVertex2f(centerX + (requiredWidth/2),_y1);
					glTexCoord2f(1,0);
					glVertex2f(centerX + (requiredWidth/2),_y2);
					glTexCoord2f(0,0);
					glVertex2f(centerX - (requiredWidth/2),_y2);
				glEnd();
			}
		}
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}
void Renderer::placeTexture4(const int _x1, const int _y1, const int _x2, const int _y2, HasTexture* _texture, const bool preserveAspectRatio /* Default false. */)
{
	placeTexture4 ( _x1, _y1, _x2, _y2, _texture->currentTexture(), preserveAspectRatio);
}

void Renderer::placeTextureCentered (const int _centerX, const int _centerY, HasTexture* _texture)
{
	const int _x1 = _centerX - _texture->currentTexture()->nX/2;
	//const int _x2 = _centerX + _texture->currentTexture()->nX/2;

	const int _y1 = _centerY - _texture->currentTexture()->nY/2;
	//const int _y2 = _centerY + _texture->currentTexture()->nY/2;

	placeTexture4 ( _x1, _y1, _x1+ _texture->currentTexture()->nX, _y1+_texture->currentTexture()->nY, _texture->currentTexture(), false);
}

void Renderer::placeColour4 (const unsigned char r, const unsigned char g, const unsigned char b, const int _x1, const int _y1, const int _x2, const int _y2)
{
	#ifdef WILDCAT_USE_OPENGL
		setColourMode();
		/* Set the colour of the shape. */
		glColor3ub(r,g,b);
		/* Draw the shape. */
		glBegin(GL_TRIANGLE_STRIP);
			glVertex2f(_x1,_y2);
			glVertex2f(_x1,_y1);
			glVertex2f(_x2,_y2);
			glVertex2f(_x2,_y1);
		glEnd();
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

void Renderer::placeColour4a (const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a, const int _x1, const int _y1, const int _x2, const int _y2)
{
	#ifdef WILDCAT_USE_OPENGL
		setColourMode();
		/* Set the colour of the shape. */
		glColor4ub(r,g,b,a);
		/* Draw the shape. */
		glBegin(GL_TRIANGLE_STRIP);
			glVertex2f(_x1,_y2);
			glVertex2f(_x1,_y1);
			glVertex2f(_x2,_y2);
			glVertex2f(_x2,_y1);
		glEnd();
		
			// Reset alpha.
		glColor4ub(r,g,b,255);
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

void Renderer::placeColour4 (Colour* colour, const int _x1, const int _y1, const int _x2, const int _y2)
{
	#ifdef WILDCAT_USE_OPENGL
		if(colour!=0)
		{
			setColourMode();
			Renderer::placeColour4 (colour->red,colour->green,colour->blue,_x1,_y1,_x2,_y2);
		}
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

//Stolen from https://stackoverflow.com/questions/13462209/opengl-draw-rectangle-outline
void Renderer::placeBorder4(const unsigned char r, const unsigned char g, const unsigned char b, const int _x1, const int _y1, const int _x2, const int _y2)
{
	#ifdef WILDCAT_USE_OPENGL
		setColourMode();
		/* Set the colour of the shape. */
		glColor3ub(r,g,b);
		/* Draw the shape. */
		 glBegin(GL_LINE_LOOP);
      glVertex2f(_x1,_y1);
      glVertex2f(_x1,_y2);
      glVertex2f(_x2,_y2);
      glVertex2f(_x2,_y1);
      //glVertex2f(_x1,_y1);
      

		 glEnd();
    
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

void Renderer::placeLine(const unsigned char r, const unsigned char g, const unsigned char b, const int _x1, const int _y1, const int _x2, const int _y2)
{
	#ifdef WILDCAT_USE_OPENGL
		setColourMode();
		/* Set the colour of the shape. */
		glColor3ub(r,g,b);
		/* Draw the shape. */
		 glBegin(GL_LINES);
      glVertex2f(_x1,_y1);
      glVertex2f(_x2,_y2);
		 glEnd();
    
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}

void Renderer::placeLineAlpha(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a, const int _x1, const int _y1, const int _x2, const int _y2)
{
	#ifdef WILDCAT_USE_OPENGL
		setColourMode();
		/* Set the colour of the shape. */
		glColor4ub(r,g,b,a);
		/* Draw the shape. */
		 glBegin(GL_LINES);
      glVertex2f(_x1,_y1);
      glVertex2f(_x2,_y2);
		 glEnd();
    
	#elif defined WILDCAT_USE_DIRECT3D
		/* Put Direct3D code here... */
	#endif
}
#endif
