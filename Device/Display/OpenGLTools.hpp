#pragma once
#ifndef DEVICE_DISPLAY_OPENGLTOOLS_HPP
#define DEVICE_DISPLAY_OPENGLTOOLS_HPP

#warning "OpenGLTools is deprecated."

/* #include <Device/Display/OpenGLTools.hpp>
	Functions to make drawing stuff in OpenGL easier.	
*/

/*
As of 0221632876, OpenGLTools will now internally keep track of changes to OpenGL settings, as the impact on performance seems to be negligible.
*/
	
#include <Graphics/Colour/Colour.hpp>
#include <Math/Geometry/Geometry.hpp>
#include <Graphics/Texture/Texture.hpp>

/* NOTE: When making lots of changes to OpenGL settings, use the following:
		glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT); // SAVE SETTINGS
		glDisable(GL_TEXTURE_2D); // ETC
		// CODE
		glPopAttrib(); // RESTORE SETTINGS
		
		That also saves and restores the colour settings.
*/

namespace OpenGLTools
{
	bool textureMode = false;
	bool colourMode = false;
	

	
	/* This keeps track of the previous viewport coordinates. */
	GLint savedViewPort[4];
	
	/* Save the current viewport so it can be quickly restored later. */
	void saveViewPort()
	{
		glGetIntegerv( GL_VIEWPORT, savedViewPort );
	}
	void saveViewport() { saveViewPort(); }
	/* Restore the viewport to the previously saved coordinates. I'm not sure if this code is correct, but it seems to work okay. I'll leave the mess here for now. */
	void restoreViewPort()
	{
					//glViewport(savedViewPort[0],savedViewPort[1],savedViewPort[2],savedViewPort[3]);
		
		//glViewport(0, 0,_nX, _nY);
		//gluOrtho2D(savedViewPort[0],savedViewPort[1],savedViewPort[2],savedViewPort[3]);
		
		//std::cout<<"Restoring viewport... Coords are: "<<savedViewPort[0]<<","<<savedViewPort[1]<<","<<savedViewPort[2]<<","<<savedViewPort[3]<<".\n";
		
		/* Update projection matrix */
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		/* Here we move the viewport to the top right corner of the screen. */
		glViewport(savedViewPort[0],savedViewPort[1],savedViewPort[2],savedViewPort[3]);
		//std::cout<<"Saved viewport: "<<savedViewPort[0]<<","<<savedViewPort[1]<<","<<savedViewPort[2]<<","<<savedViewPort[3]<<".\n";
		/* For some reason this should just be left as it normally is. */
		
		
			//gluOrtho2D(0.0, _resX, 0.0, _resY);
			//gluOrtho2D(_x1,_x2,_y1,_y2);
			gluOrtho2D(savedViewPort[0],savedViewPort[2],savedViewPort[1],savedViewPort[3]);
		/* Go back to default matrix mode. */
		glMatrixMode(GL_MODELVIEW);
	}
	void restoreViewport() { restoreViewPort(); }
	
	
	/* This function recieved the most attention. I'm not so sure about the whole saving and restoring part, but it seems to work okay right now.
		My viewport functions take 2 coordinates, which is different to the usual way viewports are handled in OpenGL. This is just due to the way my engine works. */
	void resizeViewPort(const int _x1, const int _y1, const int _x2, const int _y2)
	{
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
	}
	void resizeViewport(const int _x1, const int _y1, const int _x2, const int _y2) { resizeViewPort(_x1,_y1,_x2,_y2); }
	/* The old code which didn't work. I don't know why I leave this shit lying around. It's not like I'm ever going to want to see it again. */
	// void resizeViewPort(const int _x1, const int _y1, const int _x2, const int _y2, const int _resX, const int _resY)
	// {
			// const int _nX = _x2-_x1;
			// const int _nY = _y2-_y1;
			// //glViewport(_x1,_y1,_nX,_nY);
		
		// //glViewport(0, 0,_nX, _nY);
		// //gluOrtho2D(0,_nX,0,_nY);
		// //gluOrtho2D(_x1,_x2,_y1,_y2);
		
		// //float floatArray [0];
		
		// /* Update projection matrix */
		// glMatrixMode(GL_PROJECTION);
		// glLoadIdentity();
		// /* Here we move the viewport to the top right corner of the screen. */
		// //glViewport(_x1,_y1,_x2,_y2);
		// glViewport(_x1,_y1,_nX,_nY);
		// /* For some reason this should just be left as it normally is. */
		
		
			// //gluOrtho2D(0.0, _resX, 0.0, _resY);
			// gluOrtho2D(_x1,_x2,_y1,_y2);
		// /* Go back to default matrix mode. */
		// glMatrixMode(GL_MODELVIEW);
	// }

	
	inline void resetColour()
	{ glColor3ub(255,255,255); }
	inline void resetColor() { resetColour(); }
	
	inline void setTextureMode()
	{
		if (textureMode==false)
		{
			textureMode=true;
			colourMode=false;
			glEnable(GL_TEXTURE_2D);
			resetColour();
		}
	}
	
	inline void setColourMode()
	{
		/* Save texture and colour settings. */
		//glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT);
		/* We can't draw coloured shapes without turning off textures. */
		if(colourMode==false)
		{
			colourMode=true;
			textureMode=false;
			glDisable(GL_TEXTURE_2D);
			resetColour();
		}
	}
	inline void setColorMode() { setColourMode(); }



	/* Functions for quadrilaterals. */
	/* Place a textured quadrilateral. */
	inline void placeTexture8(const int _x1, const int _y1, const int _x2, const int _y2, const int _x3, const int _y3, const int _x4, const int _y4, const GLuint _texture)
	{
		setTextureMode();
	
		glBindTexture( GL_TEXTURE_2D, _texture);
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
	}
	
	inline void placeTexture8(const int _x1, const int _y1, const int _x2, const int _y2, const int _x3, const int _y3, const int _x4, const int _y4, const Texture* _texture)
	{
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
	}

	
	
	
	
	
	

	/* Functions for squares. */


	
	/* Render a square texture rotated about its center by a certain number of degrees. */
	inline void placeTexture4RotatedDegrees(const int _x1, const int _y1, const int _x2, const int _y2, const GLuint _texture, const int rotationDegrees)
	{
		setTextureMode();
	
		/* Turn the 2 points into a square object. */
		Square <int> square (_x1,_y1,_x2,_y2);
		/* Rotate the square object. */
		square.rotateAboutCenterDegrees(rotationDegrees);

		glBindTexture( GL_TEXTURE_2D, _texture);
		glBegin(GL_QUADS);
		
			glTexCoord2f(0,1);
			glVertex2f(square.p1.x,square.p1.y);
			glTexCoord2f(1,1);
			glVertex2f(square.p2.x,square.p2.y);
			glTexCoord2f(1,0);
			glVertex2f(square.p3.x,square.p3.y);
			glTexCoord2f(0,0);
			glVertex2f(square.p4.x,square.p4.y);
		glEnd();
	}
	
	
	/* Render an upright square texture. */
	/* bool preserveAspectRatio true will ensure the texture fills the given coordinates, but will not alter the aspect ratio. This means that some of the texture could overflow out of the intended area, or get cut off. The texture will be centered inside the given coordinates.*/ 
	/* This function uses the Texture class, as querying OpenGL texture dimensions can apparently hang the GPU, depending on the driver. Therefore creating a wrapper for the OpenGL textures seems to be a common practice. A good driver would be expected to cache the texture specs, but this cannot be guaranteed.
	Anyway, this new method is recommended for new texture loading, but I'll keep the old function around to avoid a thousand compile errors for now. Where practical, GLuints should now be converted to Texture objects. */
	inline void placeTexture4(const int _x1, const int _y1, const int _x2, const int _y2, const Texture* _texture, const bool preserveAspectRatio=false)
	{
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
			
			//std::cout<<"Texture aspect ratio: "<<textureAspectRatio<<".\n";
		
			const int selectionNX = _x2-_x1;
			const int selectionNY = _y2-_y1;
		
			const double selectionAspectRatio = (double) selectionNX/selectionNY;
			
			//std::cout<<"Selection aspect ratio: "<<selectionAspectRatio<<".\n";
		
			/* higher number == wider. */
			
			if(selectionAspectRatio==textureAspectRatio)
			{
				//std::cout<<"No adjustment required.\n";
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
	}
	
	/* DEPRECATED */

	//#ifdef 0
	
	#ifdef OLD_STUFF
	
	/* Render an upright square texture. */
	/* bool preserveAspectRatio true will ensure the texture fills the given coordinates, but will not alter the aspect ratio. This means that some of the texture could overflow out of the intended area, or get cut off. */ 
	inline void placeTexture4(const int _x1, const int _y1, const int _x2, const int _y2, const GLuint _texture, const bool preserveAspectRatio=false)
	{
		setTextureMode();
	
		if(preserveAspectRatio==false)
		{
			glBindTexture( GL_TEXTURE_2D, _texture);
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
			
		
		
		
		}
	}
	#endif
	#ifdef OLD_STUFF
	/* Renders a square texture rotated 90 degrees clockwise. */
	inline void placeTexture4RotatedLeft(const int _x1, const int _y1, const int _x2, const int _y2, const GLuint _texture)
	{
		setTextureMode();
		glBindTexture( GL_TEXTURE_2D, _texture);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2f(_x1,_y1);
			glTexCoord2f(0,1);
			glVertex2f(_x2,_y1);
			glTexCoord2f(1,1);
			glVertex2f(_x2,_y2);
			glTexCoord2f(1,0);
			glVertex2f(_x1,_y2);
		glEnd();
	}
	#endif

	inline void placeColour4 (const unsigned char r, const unsigned char g, const unsigned char b, const int _x1, const int _y1, const int _x2, const int _y2)
	{
		setColourMode();
		/* Save texture and colour settings. */
		//glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT);
		/* We can't draw coloured shapes without turning off textures. */
		//glDisable(GL_TEXTURE_2D);
		/* Set the colour of the shape. */
		glColor3ub(r,g,b);
		/* Draw the shape. */
		glBegin(GL_TRIANGLE_STRIP);
			glVertex2f(_x1,_y2);
			glVertex2f(_x1,_y1);
			glVertex2f(_x2,_y2);
			//glVertex2f(_x2,_y1);
			glVertex2f(_x2,_y1);

		glEnd();
		/* Restore texture and colour settings. */
		//glPopAttrib();
	}
	//#endif
	
#ifdef OLD_STUFF

	/* Place a square section of colour. */
	inline void placeColour4o (const unsigned  char r, const unsigned char g, const unsigned char b, const int _x1, const int _y1, const int _x2, const int _y2)
	{
		setColourMode();
		/* Save texture and colour settings. */
		//glPushAttrib(GL_TEXTURE_BIT | GL_CURRENT_BIT);
		/* We can't draw coloured shapes without turning off textures. */
		//glDisable(GL_TEXTURE_2D);
		/* Set the colour of the shape. */
		glColor3ub(r,g,b);
		/* Draw the shape. */
		glBegin(GL_QUADS);
			glVertex2f(_x1,_y1);
			glVertex2f(_x2,_y1);
			glVertex2f(_x2,_y2);
			glVertex2f(_x1,_y2);
		glEnd();
		/* Restore texture and colour settings. */
		//glPopAttrib();
	}
#endif
	
	inline void placeColour4 (Colour* colour, const int _x1, const int _y1, const int _x2, const int _y2)
	{
		if(colour!=0)
		{
			setColourMode();
			OpenGLTools::placeColour4 (colour->red,colour->green,colour->blue,_x1,_y1,_x2,_y2);
		}
	}


	
	//void undoColourMode()
	//{
	//	glPopAttrib();
	//}
	
	// void placeColour4Fast (const unsigned  char r, const unsigned char g, const unsigned char b, const int _x1, const int _y1, const int _x2, const int _y2)
	// {
		// /* Set the colour of the shape. */
		// glColor3ub(r,g,b);
		// /* Draw the shape. */
		// glBegin(GL_QUADS);
			// glVertex2f(_x1,_y1);
			// glVertex2f(_x2,_y1);
			// glVertex2f(_x2,_y2);
			// glVertex2f(_x1,_y2);
		// glEnd();
	// }
	

	
	// void placeColour4Smart (const unsigned  char r, const unsigned char g, const unsigned char b, const int _x1, const int _y1, const int _x2, const int _y2)
	// {
		// if (colourMode == false)
		// {
			// //setColourMode();
			// colourMode = true;
			
			// glDisable(GL_TEXTURE_2D);
			
			
		// }

	
		// /* Set the colour of the shape. */
		// glColor3ub(r,g,b);
		// /* Draw the shape. */
		// glBegin(GL_QUADS);
			// glVertex2f(_x1,_y1);
			// glVertex2f(_x2,_y1);
			// glVertex2f(_x2,_y2);
			// glVertex2f(_x1,_y2);
		// glEnd();

	// }
	
}

#endif
