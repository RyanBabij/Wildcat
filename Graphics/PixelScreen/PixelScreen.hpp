#pragma once
#ifndef WILDCAT_GRAPHICS_PIXELSCREEN_HPP
#define WILDCAT_GRAPHICS_PIXELSCREEN_HPP

/* Wildcat: PixelScreen
#include <Graphics/PixelScreen/PixelScreen.hpp>

A PixelScreen is an array of pixels which can be converted to a texture and then rendered to a set of screen
coordinates. It can also have textures like fonts merged onto it. The main purpose is to create a faux-retro screen
monitor, however some other uses for it could come up, you could even make pixelshit games with it.

PixelScreen is mostly to deal with rendering. Advanced functionality like scrolling should be handled by an external
class.

Currently it creates and binds a new texture at runtime, which is not optimal but works well enough. The performance of
binding a 320x200 texture at runtime is acceptable, achieving about 70FPS on my dev builds. Binding larger textures
however becomes significantly more costly.

PixelScreen also has some effects it can do like basic glare effects
and whatnot.

PixelScreen should have filters which can be customised and overlaid
in any order. PixelScreen_Filter class with an execute function.

Filters can be blended, merged, or copied directly.

PixelScreen supports a text layer but only the grid for rendering it. It does not support advanced things like
scrolling, these are implemented in Terminal. Obviously things could be done differently by adding a
PixelScreen_TextMode class but that is outside of the project scope.

Todo:

* State updates should be decoupled from render calls.
* Pixel Buffer Object support

*/

#include <Container/ArrayS3/ArrayS3.hpp>
#include <Math/Random/RandomLehmer.hpp>
#include <Interface/HasTexture.hpp>

// Sprites are not currently blitted, but rather directly rendered at the full resolution. Therefore you should
// manually ensure that the pixels line up if making pixelshit unless you are trolling.
class Sprite: public HasTexture
{
public:
	int x1,y1,x2,y2;
	// PixelScreen coords (we might want to convert these to center coords and size
	Texture * texture;

	Sprite()
	{
		x1=0;
		y1=0;
		x2=0;
		y2=0;
		texture=0;
	}

	void setCoord(const int _x1, const int _y1, const int _x2, const int _y2)
	{
		x1=_x1;
		y1=_y1;
		x2=_x2;
		y2=_y2;
	}

	bool isCollisionX(const int _x)
	{
		return( _x >= x1 && _x <= x2);
	}

	virtual Texture* currentTexture()
	{
		return texture;
	}
};

// Custom algorithms for filters and effects designed to be overlaid on the main screen
class PixelScreen_Filter
{


};

#include <Graphics/GUI/GUI_Interface.hpp>

// The actual pixelscreen canvas thing. You can draw individual pixels just like the good old VGA days.

// Todo:
//		* Custom layered filtering (scanlines, bloom)
//		* Temporal effects (fading, glitches, etc)

class PixelScreen: public GUI_Interface, public IdleTickInterface
{
private:
	RandomLehmer rngLehmer;

	// Currently pixels are only directly blitted, in future we will probably have the input and output (after effects)
	// state.
	ArrayS3 <unsigned char> aScreenDataBuffer; // Desired state of screen
	//ArrayS3 <unsigned char> aScreenDataReal; // Actual state of screen after effects
	
	// Text mode array.
	ArrayS2 <unsigned char> aCharMode; // Grid for drawing fonts onto screen.
	
	
	// The current colour layer system. Probably could be better.
	ArrayS2 <unsigned char> textRed;
	ArrayS2 <unsigned char> textGreen;
	ArrayS2 <unsigned char> textBlue;

	unsigned char rngPool [1000]; // for generating static

	Timer updateTimer;

	unsigned short int nX, nY; // number of pixels (not size of panel)

	// vector of sprites to "blit"
	Vector <Sprite*> vSprite;


public:
	unsigned short int nCharX, nCharY;

	std::string strOverlay; // simple text overlay for hud or help menus, immune from screen clears.
	// in future I'll probably replace it with a separate char array.

	Texture texScreen; // dynamically generated texture
	Texture texOverlay; // dynamically generated texture. Text/hud overlay

	double scalingFactor; // how many times the standard resolution to scale up. Currently seems to affect only some
	// aspects of the render. We should probably support decimal values although it would not look perfect.

	int fadeSpeed; // max rgb value change per frame
	double updatesPerSecond; // amount of times to update screen state per second
	
	
	bool scanLines; // enable scanlines filter. Currently pretty hardcoded but looks fine anyway.
	
	bool charLayer; // important: Charlayer is text mode. Text mode can be switched off.
	
	// I might want to just link it with the global idleTick speed.
	// Or have a postRender() function which preps for the next render.

	unsigned char amountStatic; // maximum static val. 0 = disabled.

	short int mouseX,mouseY; // these are the pixel coords, not screen coords.

	PixelScreen(const unsigned short int _nX, const unsigned short int _nY) // define the size in pixels
	{
		init (_nX,_nY);
	}

	void init(const unsigned short int _nX, const unsigned short int _nY)
	{
		nX=_nX;
		nY=_nY;

		nCharX=0; nCharY=0;

		active=false;
		panelX1=0; panelY1=0; panelX2=0; panelY2=0;
		font=0;
		aScreenDataBuffer.init(nX,nY,4,0); // RGBA
		//aScreenDataReal.init(nX,nY,4,0); // RGBA
		
		aCharMode.init(0,0,' '); // aCharMode is initialised when the font is set.
		textRed.init(nX,nY,255);
		textGreen.init(nX,nY,255);
		textBlue.init(nX,nY,255);

		texScreen.create(nX,nY,1,true); // we might instead use this as render
		texScreen.fill(0);

		texOverlay.create(nX,nY,1,true); // we might instead use this as render
		texOverlay.fill(0);

		updateTimer.init();
		updateTimer.start();

		updatesPerSecond=60;
		scanLines=true;
		charLayer=true; // should be enabled by default to prevent confusion.
		amountStatic=0;
		scalingFactor=1;

		mouseX=-1;
		mouseY=-1;
		
		for (int i=0;i<1000;++i)
		{
			//rngPool[i]=Random::randomInt(255);
			rngPool[i]=rngLehmer.rand8();
		}
	}

	void init() override
	{

	}
	
	void setFont (Wildcat::Font* _font) override
	{
		if ( _font == 0 )
		{
			font=0;
			return;
		}
		font = _font;

		// adjust text mode array size based on font size.
		nCharX = nX / _font->nX;
		nCharY = nY / _font->nY;
		aCharMode.init(nCharX,nCharY,' ');
	}

	void clear()
	{
		aScreenDataBuffer.fill(0); // RGBA
		//aScreenDataReal.fill(0); // RGBA
		aCharMode.fill(' ');
		textRed.fill(255);
		textGreen.fill(255);
		textBlue.fill(255);
		aCharMode.fill(' ');
	}

	void fill (unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
	{
		aScreenDataBuffer.fillLayer(0,_r);
		aScreenDataBuffer.fillLayer(1,_g);
		aScreenDataBuffer.fillLayer(2,_b);
		aScreenDataBuffer.fillLayer(3,_a);

		texScreen.fillChannel(0,_r);
		texScreen.fillChannel(1,_g);
		texScreen.fillChannel(2,_b);
		texScreen.fillChannel(3,_a);
	}

	// write a character to the given row and column with the specified colour.
	void putChar (const unsigned short int _x, const unsigned short int _y, const unsigned char _char, ColourRGBA <unsigned char> foregroundColour)
	{
		aCharMode(_x,_y)=_char;
		textRed(_x,_y)=foregroundColour.red;
		textGreen(_x,_y)=foregroundColour.green;
		textBlue(_x,_y)=foregroundColour.blue;
	}
	// write an entire string to the screen from the given coordinate. Wrapping should be done automatically.
	void putString (unsigned short int _x, const unsigned short int _y, std::string _str, bool wordBreak=false)
	{
		for (unsigned int i=0;i<_str.size();++i)
		{
			if ( aCharMode.isSafe(_x,_y))
			{
				aCharMode(_x,_y)=_str[i];
				textRed(_x,_y)=255;
				textGreen(_x,_y)=255;
				textBlue(_x,_y)=255;
				++_x;
			}
		}
	}

	/* MouseInterface:: */
	bool mouseEvent (Mouse* _mouse) override
	{
		//calculate the mouse's pixel coords

		if ( _mouse == 0 ) { return false; }

		if (_mouse->inBounds(panelX1,panelY1,panelX2,panelY2))
		{
			int diffX = _mouse->x - panelX1;
			int diffY = _mouse->y - panelY1;

			mouseX = diffX/scalingFactor;
			mouseY = diffY/scalingFactor;
			return true;
		}

		mouseX=-1;
		mouseY=-1;
		return false;
	}


	// This needs to be converted to logic ticks as screen updates are inconsistent using this.
	void idleTick() override
	{ // I'm not sure if we should have an independent timer for this,
		// or just use logic ticks instead.
		// update state if conditions met

		if (font==0) { return; }
		updateTimer.update();

		int uUPS = 1000000/updatesPerSecond;

		//we should be using a filter system to determine the order to draw things.
		//but for now it's hardcoded

		if ( updateTimer.totalUSeconds > uUPS)
		{
			texScreen.fill(0);
			texOverlay.fill(0);
			//fillStatic(amountStatic);
			fillStatic(80);
			//fill(255,0,0,255);

			//merge down the pixel buffer
			const int nPixel = nX*nY*4;
			for (int i=0;i<nPixel;++i)
			{
				// texScreen.data[i]=aScreenDataBuffer.data[i];
			}

			// update screen state
			//texScreen.fillChannel(Random::randomInt(3),Random::randomInt(255));

			// draw font
			for (int _y=0;_y<nCharY;++_y)
			{
				for (int _x=0;_x<nCharX;++_x)
				{
					if ( aCharMode(_x,_y) != ' ' )
					{
						// For now I'll just overlay the font with the normal render call.
						// I'd like this option in future for easy text overlays

						// texScreen.copyDown(font->aTexFont[(unsigned char)aCharMode(_x,_y)],font->nX*_x,font->nY*_y);
						//texScreen.morphDown(font->aTexFont[(unsigned char)aCharMode(_x,_y)],font->nX*_x,font->nY*_y,100);
					}
					//texRuntime.morphDown(font8x8.aTexFont[(unsigned char)aGlyph[_y][_x]],8*_x,8*_y,22);

					//texScreen.morphDown(font->aTexFont['A'],8*_x,8*_y,25);
				}
			}
			
		if ( charLayer )
		{
			// draw font as an overlay
			for (int _y=0;_y<nCharY;++_y)
			{
				for (int _x=0;_x<nCharX;++_x)
				{
					if ( aCharMode(_x,_y) != ' ' )
					{
						// For now I'll just overlay the font with the normal render call.
						// I'd like this option in future for easy text overlays
						
						//texOverlay.copyDown(font->aTexFont[(unsigned char)aCharMode(_x,_y)],(font->nX)*_x,(font->nY)*_y);
						texOverlay.copyDown(font->aTexFont[(unsigned char)aCharMode(_x,_y)],(font->nX)*_x,(font->nY)*_y,textRed(_x,_y),textGreen(_x,_y),textBlue(_x,_y));
						// I believe this slowly fades a char onto the screen.
						// doesn't seem to work atm but I don't really use it anyway
						//texScreen.morphDown(font->aTexFont[(unsigned char)aCharMode(_x,_y)],font->nX*_x,font->nY*_y,100);
						
						// This just blits a font directly onto the screen for debug purposes.
						//font8x8.putChar(aGlyph[_y][_x],panelX1+(10*_x),panelY2-(10*_y),foregroundColour[_y][_x]);
					}
				}
			}
		}
		


			// scanlines use OpenGL lines, so it's handled by Terminal
			//draw scanlines
			// this doesn't work as the scanlines are too wide. they must be rendered.
			for (int y=0;y<nY;y+=2)
			{
				for(int x=0;x<nX;++x)
				{
					// texOverlay.setPixel(x,y,0,texOverlay.uPixel(x,y,2)/2);
					// texOverlay.setPixel(x,y,1,texOverlay.uPixel(x,y,2)/2);
					// texOverlay.setPixel(x,y,2,texOverlay.uPixel(x,y,2)/2);
				}
			}
			updateTimer.init();
			updateTimer.start();
		}
	}

	void setPixel(const short int _x, const short int _y, const short int _z, const unsigned char _value)
	{
		aScreenDataBuffer(_x,_y,_z)=_value;
	}
	void setPixel(const short int _x, const short int _y, const short int _r, const short int _g, const short int _b)
	{
		if ( isSafe(_x,nY-1-_y) == false )
		{ return; }
	
		// aScreenDataBuffer(_x,_y,0)=_r;
		// aScreenDataBuffer(_x,_y,1)=_g;
		// aScreenDataBuffer(_x,_y,2)=_b;
		// aScreenDataBuffer(_x,_y,3)=255;

		// Apparently textures are upside-down
		texScreen.setPixel(_x,nY-1-_y,0,_r);
		texScreen.setPixel(_x,nY-1-_y,1,_g);
		texScreen.setPixel(_x,nY-1-_y,2,_b);
		texScreen.setPixel(_x,nY-1-_y,3,255);
	}

	unsigned char getPixel(const short int _x, const short int _y, const short int _channel)
	{
		if ( aScreenDataBuffer.isSafe(_x,_y,_channel) )
		{
			return texScreen.uPixel(_x,_y,_channel);
		}
		return 0;
	}

	bool isSafe(const short int _x, const short int _y)
	{
		return( _x >= 0 && _x < nX && _y >= 0 && _y < nY );
	}

	void eventResize() override
	{
		std::cout<<"PIXELSCREEN RESIZED: "<<panelX1<<", "<<panelY1<<", "<<panelX2<<", "<<panelY2<<".\n";
	}

	bool keyboardEvent(Keyboard* _keyboard) override
	{
		return false;
	}

	void render() override 
	{
		// render state
		//texScreen.bloom();
		//std::cout<<"Panel is: "<<panelX1<<", "<<panelY1<<", "<<panelX2<<", "<<panelY2<<"\n";
		
		//texOverlay.fill(0);

		bindNearestNeighbour(&texScreen);
		Renderer::placeTexture4(panelX1,panelY1,panelX2,panelY2,&texScreen,false);
		unbind(&texScreen);
		
		for (int i=0;i<vSprite.size();++i)
		{
			// OpenGL uses different coordinates from pixel screen so we need to flip the y.
			Sprite* const sprite = vSprite(i);
			if ( sprite != 0 )
			{
				Renderer::placeTexture4(panelX1+(sprite->x1*scalingFactor),panelY1+(sprite->y1*scalingFactor),
				panelX1+(sprite->x2*scalingFactor),panelY1+(sprite->y2*scalingFactor),sprite->currentTexture(),false);
			}
		}
		
		// force an idle tick to update screen.
		//idleTick();
		

		
		bindNearestNeighbour(&texOverlay);
		Renderer::placeTexture4(panelX1,panelY1,panelX2,panelY2,&texOverlay,false);
		unbind(&texOverlay);
		
		// post-processing goes here
		
		if (scanLines)
		{
			// This has to be drawn as a render because the scanlines are sub-pixel scale.
			for (double _y=panelY2-(scalingFactor);_y>=panelY1;_y-=(scalingFactor*2))
			{
				Renderer::placeLineAlpha(0,0,0,90,panelX1,_y,panelX2,_y,scalingFactor);
			}
			// grid mode
			for (double _x=panelX2-(scalingFactor);_x>=panelX1;_x-=(scalingFactor*2))
			{
				Renderer::placeLineAlpha(0,0,0,45,_x,panelY1,_x,panelY2,scalingFactor);
			}
		}
	}

	void fillStatic(const unsigned char maxValue=255)
	{
		if ( maxValue == 0 )
		{
			return;
		}
		//aCharMode.fill(' ');

		int nPixels=nX*nY*4;

		//int i2 = Random::randomInt(1000);
		int i2 = rngLehmer.rand8();

		//Static only draws direct to texture, not to buffer.
		for (int i=0;i<nPixels;++i)
		{
			//aScreenDataBuffer.data[i]=rngPool[i2]%maxValue;
			//aScreenDataReal.data[i]=aScreenDataBuffer.data[i];
			texScreen.data[i]=rngPool[i2]%maxValue;

			++i2;
			if ( i2>=1000) { i2 = rngLehmer.rand8(); }
		}
	}

	void addSprite(Sprite *sprite)
	{
		vSprite.push(sprite);
	}
	void removeSprite(Sprite* sprite)
	{
		vSprite.erase(sprite);
	}

};

#endif