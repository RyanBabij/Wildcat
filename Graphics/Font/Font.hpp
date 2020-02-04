#pragma once
#ifndef GRAPHICS_FONT_FONT_HPP
#define GRAPHICS_FONT_FONT_HPP

/* Wildcat: Font
	#include <Graphics/Font/Font.hpp>
	
	The Font class loads bitmap fonts from a PNG with the appropriate layout,
	and can then render strings with the bitmap font. Features are currently
	very basic but it works well enough for me.
	
	Protip for bitmap fonts: The bitmap should be white so that colour can
	be easily applied. The PNG must be the expected size or it won't load.
	
	FEATURES
	
	Currently only 128 ASCII is supported. Extended table support will probably
	be done later. Anything more complicated (eg Unicode) will probably just use
	an external library.
	
	ANSI ESCAPE CODES
	
	I'm currently adding support for colours using the ANSI escape codes.
*/

#include <Container/ArrayS3/ArrayS3.hpp> // Storing PNG pixel data
#include <Graphics/Render/Renderer.hpp>
//#include <Graphics/Colour/Palette.hpp> // For basic font colours.

#include <Container/String/ANSI.hpp> // For processing ANSI escape codes.

// Font conflicts with an X11 class, so we need to namespace this.
namespace Wildcat
{

class Font
{
	public:
	GLuint characterMap;
	GLuint character [256];
   Texture *aTexFont[256];
   ArrayS3 <unsigned char> aTexFont2[256]; // Using ArrayS3 for better compatibility with other function.

	int nX, nY; /* Dimensions of a character. (Currently all characters must have the same dimensions.) */
	
	int xSpacing, ySpacing; /* How much spacing you would like between glyphs. */
	
	bool loadSuccess;
	
   Font()
   {
      nX=0; nY=0;
      loadSuccess=false;
      xSpacing = 0;
      ySpacing = 0;
   }

   bool loadData(Png* png, const int _nX, const int _nY)
   {
      if (png==0)
      {
         std::cout<<"ERROR: Font PNG did not load.\n";
         return false;
      }
      if ( png->nX == 0 || png->nY == 0 )
      {
         std::cout<<"Error: PNG dimensions are 0x0.\n";
         return false;
      }

      if ( _nX * 16 != png->nX || _nX *16 != png->nY )
      {
         std::cout<<"ERROR: Font PNG has unexpected dimensions for given font size: "<<_nX<<", "<<_nY<<".\n";
         return false;
      }

      std::cout<<"Loading font\n";
      std::cout<<"NX, NY: "<<_nX*16<< ", "<<_nY*16<<".\n";

      nX=_nX;
      nY=_nY;

      glGenTextures(1, &characterMap);
      glBindTexture(GL_TEXTURE_2D, characterMap);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

      glGenTextures(256,character);

      int i=0;
      int currentX=0;
      int currentY=0;
      
      

			
      while(i<256)
      {
         glBindTexture(GL_TEXTURE_2D, character[i]);
         glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

         ArrayS3 <unsigned char> sub;
         sub.init(_nX,_nY,4,0);
         for(int _y=0;_y<_nY;++_y)
         {
            for(int _x=0;_x<_nX;++_x)
            {
               sub(_x,_y,0)=png->getPixel3D(currentX*_nX+_x,currentY*_nY+_y,0);
               sub(_x,_y,1)=png->getPixel3D(currentX*_nX+_x,currentY*_nY+_y,1);
               sub(_x,_y,2)=png->getPixel3D(currentX*_nX+_x,currentY*_nY+_y,2);
               sub(_x,_y,3)=png->getPixel3D(currentX*_nX+_x,currentY*_nY+_y,3);
            }
         }

         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _nX, _nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, sub.data);
         
         // also build a texture object so we can do per-pixel stuff.
         aTexFont[i] = new Texture;
         aTexFont[i]->create(_nX,_nY,1,true);
         //aTexFont[i]->data=sub.data;
         
         std::copy(sub.data, sub.data+_nX*_nY*4, aTexFont[i]->data);
         //aTexFont[i]->loadArray(sub);
         
         // Switching to ArrayS3 for pixel operations.
         aTexFont2[i].init(_nX,_nY,4,0);
         std::copy(sub.data, sub.data+_nX*_nY*4, aTexFont2[i].data);
         
         ++currentX;
         if(currentX==16)
         {
            currentX=0;
            ++currentY;
         }

         ++i;
      }
      std::cout<<"end loading font\n";
      loadSuccess=true;
      return true;
	}
	

	/*
		The text will only draw within the specified coordinates.
		Coords are automatically sorted into the right order.
		The centeredX option will center each line.

    Will return the number of lines it drew. (0 for error)
		
		Update: Add ability to simply render each character as it appears,
		without trying to make it look neat.
		
		The current font table uses only 128 characters. This is the same as the old standard ASCII table.
		Extended table support is planned for future.
		
		Update: Added support for ANSI escape codes. Colours will automatically be rendered if ANSI
		colour codes are provided. Currently only red is supported.
		
	*/
   int drawText(std::string text, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, bool centeredX = false, bool centeredY=false, bool rawTextMode=false,
				unsigned char _colourRed=0, unsigned char _colourGreen=0,unsigned char _colourBlue=0,unsigned char _alpha=255)
	{
		
		if (loadSuccess == false )
		{
			return 0;
		}
		
		if (text.size() == 0)
		{
			return 0;
		}
      
      
		
		/* Automatically sort the box coordinates here, so we don't need to input the coordinates in any particular order. */
		if (y1<y2) { DataTools::swap(&y1,&y2); }
		if (x1>x2) { DataTools::swap(&x1,&x2); }

		const int panelWidth = x2-x1;
		const int panelHeight = y1-y2;
		
		// STRIP AND PROCESS ANSI ESCAPE CODES.
		ANSI ansi;
		ansi.setDefaultForeground(_colourRed,_colourGreen,_colourBlue,_alpha);
		ansi.setDefaultBackground(0,0,0,0);
		ansi.read(text);
		
		text = ansi.ansiString;

		if (ansi.size()==0)
		{
			return 0;
		}
		
		if ( panelWidth < nX || panelHeight < nY || text.size() == 0 )
		{
			return 0;
		}
    
    int linesDrawn = 1;


		Renderer::setTextureMode();		
		Renderer::setColour(ansi.vForegroundColour(0));
		

		const int charsPerLine = (x2-x1) / nX;
		
		unsigned int currentX=x1;
		unsigned int currentY=y1;
		
		/* Center the initial line, by altering the starting coordinate for the first character. */
		if(centeredX==true)
		{
			//unsigned int lineWidth = 0;
			unsigned int currentX2 = x1;
			/* Figure out how many characters fit on this line. */
			for( unsigned int i=0;i<text.size() && currentX2+nX <= x2;++i)
			{
				//lineWidth+=nX;
				currentX2+=nX;
			}
			
			int remainderPixels = x2 - currentX2;
			
			
			currentX = x1+(remainderPixels/2);
		}
		
		if(centeredY==true)
		{
			int nLines = (text.size()/charsPerLine)+1;
			int vSpace = nLines*(nY+ySpacing)-ySpacing;
			
			int sY = y1-y2;
			currentY = y1 - ((sY-vSpace)/2);

		}
		
		for(unsigned int i=0;i<text.size();++i)
		{

				// No spaces when going to newline.
			if ( currentX==x1 )
			{
				if (text[i]==' ' && rawTextMode == false)
				{
					++i;
				}
			}
		
			// SET THE COLOUR
      // Protip: Changing colour doesn't work on black.
      Renderer::setColour(ansi.vForegroundColour(i));

				if ( text[i]!='\n' )
				{
					glBindTexture(GL_TEXTURE_2D, character[(int)text[i]]);
					glBegin(GL_QUADS);
					glTexCoord2f(0,1);
					glVertex2f(currentX,currentY-nY);
					glTexCoord2f(1,1);
					glVertex2f(currentX+nX,currentY-nY);
					glTexCoord2f(1,0);
					glVertex2f(currentX+nX,currentY);
					glTexCoord2f(0,0);
					glVertex2f(currentX,currentY);
					glEnd();
				}
		
				currentX+=nX;

				/* If we have reached x2, then start a new line. */
				if (currentX+nX > x2 || text[i]=='\n')
				{
					currentX=x1;
					currentY-=(nY+ySpacing);
					++linesDrawn;
					
					
					/* Center the new line. */
					if(centeredX==true)
					{
						unsigned int currentX2 = x1;
						/* Figure out how many characters fit on this line. */
						for( unsigned int i2=i;i2<text.size() && currentX2+nX <= x2;++i2)
						{
							currentX2+=nX;
						}
						int remainderPixels = x2 - currentX2;
						
						currentX = x1+(remainderPixels/2);
					}
				}
				
				// I think this breaks early if writing  a long word which will break
				if ( rawTextMode == false && text.size() > 16 && text[i]==' ' )
				{
					int charsUntilSpace = 0;
					for (unsigned int i2=1; i2<20&&i2+i<text.size();++i2)
					{
						if ( text[i2+i] == ' ' )
						{
							break;
						}
						++charsUntilSpace;
					}
					const int spaceRequired = nX*charsUntilSpace+nX;
					if ( spaceRequired < panelWidth && charsUntilSpace > 0 )
					{
						if ( currentX + spaceRequired > x2 )
						{
							currentX=x1;
							currentY-=(nY+2);
              ++linesDrawn;
						}
					}
				}

				/* If we are out of space, return. */
				if (currentY-nY < y2)
				{     Renderer::resetColour(); return linesDrawn; }
		}
		
		// Reset to default colour.
		Renderer::resetColour();
    return linesDrawn;
  }
	
	//Pass a character array. Array can be 2D, but rows won't be respected as newlines unless the coordinates are set manually to align.
	// Be aware that 2d arrays are row major, meaning x and y are switched.
	inline int drawText(unsigned int arraySize, const char* text, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, bool centeredX = false, bool centeredY=false, bool rawTextMode=false,
				unsigned char _colourRed=0, unsigned char _colourGreen=0,unsigned char _colourBlue=0)
	{
		if (loadSuccess == false || arraySize == 0)
		{
			return 0;
		}
		
		// just convert it into a string for now
		// Removed a bunch of array code, hopefully didn't break anything.
		std::string _str="";
		for (unsigned int i=0;i<arraySize;++i)
		{
			_str+=text[i];
		}
		return drawText(_str,x1,y1,x2,y2,centeredX,centeredY,rawTextMode,_colourRed,_colourGreen,_colourBlue);
	}
	

	
	void drawText(const std::string text, int x1, int y1)
	{
		if (loadSuccess == false )
		{
			return;
		}
		
    glColor3ub(0,0,0);
		for(unsigned int i=0;i<text.size();++i)
		{
				Renderer::setTextureMode();
				glBindTexture(GL_TEXTURE_2D, character[(int)text[i]]);
				glBegin(GL_QUADS);
				glTexCoord2f(0,1);
				glVertex2f(x1,y1-nY);
				glTexCoord2f(1,1);
				glVertex2f(x1+nX,y1-nY);
				glTexCoord2f(1,0);
				glVertex2f(x1+nX,y1);
				glTexCoord2f(0,0);
				glVertex2f(x1,y1);
				glEnd();
				
				x1+=nX;		
		}
    glColor3ub(255,255,255);
	}
	
	// Draw a single character from this top-left point. Useful for drawing text in grids (eg Terminals)
	void putChar(unsigned char _char, const int _x, const int _y, Colour _colour)
	{
		if ( _char == '\n' || _char == '\r' ) { return; }
		
      Renderer::setColour(_colour);

		glBindTexture(GL_TEXTURE_2D, character[(int)_char]);
		glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex2f(_x,_y-nY);
		glTexCoord2f(1,1);
		glVertex2f(_x+nX,_y-nY);
		glTexCoord2f(1,0);
		glVertex2f(_x+nX,_y);
		glTexCoord2f(0,0);
		glVertex2f(_x,_y);
		glEnd();
		
		// Reset to default colour.
		Renderer::resetColour();
	}
	
};
}

#endif
