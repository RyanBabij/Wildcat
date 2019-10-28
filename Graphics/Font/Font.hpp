#pragma once

#include "Container/ArrayS3/ArrayS3.hpp"

#include <Render/Renderer.hpp>

// #include <Graphics/Texture/TextureLoader.hpp>
// #include <Graphics/Texture/Texture.hpp>


// HERE BE DRAGONS

namespace Wildcat
{
	// FONT CONFLICTS WITH AN X11 CLASS. ARGH...

class Font
{
	public:
	GLuint characterMap;
	GLuint character [256];
  //Texture ch [256];
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
			
		std::cout<<"Font.hpp Font::loadData(), data* was 0\n";
		return false;
	}
	

	/*
		The text will only draw within the specified coordinates.
		Coords are automatically sorted into the right order.
		The centeredX option will center each line.
		
		
		CenteredX and CenteredY were implemented 0221152847, and
		is a mess. This whole lib is a mess though.
		The whole thing needs to be cleaned up.
    
    Update: will return the number of lines it drew. (0 for error)
		
		Update: Add ability to simply render each character as it appears,
		without trying to make it look neat.
			
	*/
	int drawText(const std::string text, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, bool centeredX = false, bool centeredY=false, bool rawTextMode=false,
				unsigned char _colourRed=0, unsigned char _colourGreen=0,unsigned char _colourBlue=0)
	{
		
		if (loadSuccess == false )
		{
			return 0;
		}
		
		/* Automatically sort the box coordinates here, so we don't need to input the coordinates in any particular order. */
		if (y1<y2) { DataTools::swap(&y1,&y2); }
		if (x1>x2) { DataTools::swap(&x1,&x2); }

		const int panelWidth = x2-x1;
		const int panelHeight = y1-y2;

		if ( panelWidth < nX || panelHeight < nY || text.size() == 0 )
		{
			return 0;
		}
    
    int linesDrawn = 1;


		Renderer::setTextureMode();
    //Renderer::setColour(_colourRed,_colourGreen,_colourBlue);
    glColor3ub(_colourRed,_colourGreen,_colourBlue);

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
        glColor3ub(_colourRed,_colourGreen,_colourBlue);
				//glColor3ub(250,0,0);
				//glEnable(GL_COLOR_MATERIAL);

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
				{     glColor3ub(255,255,255); return linesDrawn; }
		}
		//glColor3ub(255,255,255);
		    glColor3ub(255,255,255);
    return linesDrawn;
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
	
	
	
};
}