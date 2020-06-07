#pragma once
#ifndef WILDCAT_GRAPHICS_PIXELSCREEN_HPP
#define WILDCAT_GRAPHICS_PIXELSCREEN_HPP

/* Wildcat: PixelScreen
#include <Graphics/PixelScreen/PixelScreen.hpp>

A PixelScreen is an array of pixels which can be converted
to a texture and then rendered to a set of screen coordinates.
It can also have textures like fonts merged onto it.
The main purpose is to create a faux-retro screen monitor.

The main issue is how to size the pixels correctly, however
we could probably just let OpenGL handle it.

Additionally we need to worry about the performance of binding
textures at render. This has significant performance cost, but
fortunately there is a thing called Pixel Buffer Object which
apparently is designed for this sort of thing. Basically it's
like having a shader effect.

However for now the performance loss of binding a 320x200 texture
at runtime is acceptable, achieving about 70FPS on my dev builds.
Binding larger textures however becomes significantly more costly.

PixelScreen also has some effects it can do like basic glare effects
and whatnot.

PixelScreen should have filters which can be customised and overlaid
in any order. PixelScreen_Filter class with an execute function.

Filters can be blended, merged, or copied directly.



Todo:

* State updates should be decoupled from render calls.

*/

#include <Container/ArrayS3/ArrayS3.hpp>

#include <Math/Random/RandomLehmer.hpp>

#include <Interface/HasTexture.hpp>
// Sprite is an object which has a texture which is drawn to the screen.
// We don't bother blitting it or whatever, just render it with OpenGL.
// This means you should make sure the pixels match up
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

#include <Graphics/GUI/GUI_Interface.hpp>

class PixelScreen: public GUI_Interface, public IdleTickInterface
{
private:
	RandomLehmer rngLehmer;

	ArrayS3 <unsigned char> aScreenDataBuffer; // Desired state of screen
	//ArrayS3 <unsigned char> aScreenDataReal; // Actual state of screen after effects
	ArrayS2 <unsigned char> aCharMode; // Grid for drawing fonts onto screen.

	unsigned char rngPool [1000]; // for generating static

	Timer updateTimer;

	unsigned short int nX, nY; // number of pixels (not size of panel)
	unsigned short int nCharX, nCharY;

	Vector <Sprite*> vSprite;


public:
	std::string strOverlay; // simple text overlay for hud or help menus.
	// in future I'll probably replace it with a separate char array.

	Texture texScreen; // dynamically generated texture
	Texture texOverlay; // dynamically generated texture. Text/hud overlay

	double scalingFactor; // how many times the standard resolution to scale up

	// EFFECTS
	bool enableEffects;

	int fadeSpeed; // max rgb value change per frame
	double updatesPerSecond; // amount of times to update screen state per second
	bool scanLines;
	bool charLayer;
	// I might want to just link it with the global idleTick speed.
	// Or have a postRender() function which preps for the next render.

	unsigned char amountStatic; // maximum static val. 0 = disabled.

	short int mouseX,mouseY; //pixel coords

	PixelScreen(const unsigned short int _nX, const unsigned short int _nY) // define the size in pixels
	{
		init (_nX,_nY);
	}

	void init(const unsigned short int _nX, const unsigned short int _nY)
	{
		nX=_nX;
		nY=_nY;

		nCharX=0;
		nCharY=0;

		active=false;
		panelX1=0; panelY1=0; panelX2=0; panelY2=0;
		font=0;
		aScreenDataBuffer.init(nX,nY,4,0); // RGBA
		//aScreenDataReal.init(nX,nY,4,0); // RGBA
		aCharMode.init(0,0,' ');


		texScreen.create(nX,nY,1,true); // we might instead use this as render
		texScreen.fill(0);

		texOverlay.create(nX,nY,1,true); // we might instead use this as render
		texOverlay.fill(0);

		updateTimer.init();
		updateTimer.start();

		enableEffects=false;
		updatesPerSecond=60;
		scanLines=false;
		charLayer=false;
		amountStatic=0;

		mouseX=-1;
		mouseY=-1;
	}

	void init() override
	{
		for (int i=0;i<1000;++i)
		{
			//rngPool[i]=Random::randomInt(255);
			rngPool[i]=rngLehmer.rand8();
		}
	}

	void clear()
	{
		aScreenDataBuffer.fill(0); // RGBA
		//aScreenDataReal.fill(0); // RGBA
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

	void putChar (const unsigned short int _x, const unsigned short int _y, const unsigned char _char)
	{
		aCharMode(_x,_y)=_char;
	}
	void putString (unsigned short int _x, const unsigned short int _y, std::string _str)
	{
		for (unsigned int i=0;i<_str.size();++i)
		{
			if ( aCharMode.isSafe(_x,_y))
			{
				aCharMode(_x,_y)=_str[i];
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
			//texScreen.fill(0);
			//fillStatic(amountStatic);
			//fill(255,0,0,255);

			//merge down the pixel buffer
			const int nPixel = nX*nY*4;
			for (int i=0;i<nPixel;++i)
			{
				// texScreen.data[i]=aScreenDataBuffer.data[i];
			}

			// update screen state
			//texScreen.fillChannel(Random::randomInt(3),Random::randomInt(255));
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

			// scanlines use OpenGL lines, so it's handled by Terminal
			// //draw scanlines
			// for (int y=0;y<nY;y+=3)
			// {
			// for(int x=0;x<nX;++x)
			// {
			// //texScreen.setPixel(x,y,0,texScreen.uPixel(x,y,0)/2);
			// //texScreen.setPixel(x,y,1,texScreen.uPixel(x,y,1)/2);
			// //texScreen.setPixel(x,y,2,texScreen.uPixel(x,y,2)/2);
			// // texScreen.setPixel(x,y,1,0);
			// // texScreen.setPixel(x,y,2,0);
			// // texScreen.setPixel(x,y,3,10);
			// }
			// }
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
		//std::cout<<"PIXELSCREEN RESIZED: "<<panelX1<<", "<<panelY1<<", "<<panelX2<<", "<<panelY2<<".\n";
	}

	bool keyboardEvent(Keyboard* _keyboard) override
	{
		return false;
	}

	// void Terminal::placeTexture4(const int _x1, const int _y1, const int _x2, const int _y2, HasTexture* _texture, const bool preserveAspectRatio)
	// {

	// Renderer::placeTexture4(_x1*scalingFactor,_y1*scalingFactor,_x2*scalingFactor,_y2*scalingFactor,_texture,true);
	// }


	void render() override 
	{
		// render state
		//texScreen.bloom();
		//std::cout<<"Panel is: "<<panelX1<<", "<<panelY1<<", "<<panelX2<<", "<<panelY2<<"\n";

		bindNearestNeighbour(&texScreen);
		Renderer::placeTexture4(panelX1,panelY1,panelX2,panelY2,&texScreen,false);
		unbind(&texScreen);

		if (enableEffects)
		{
			for (int i=0;i<vSprite.size();++i)
			{
				// OpenGL uses different coordinates from pixel screen so we need to flip the y.
				//int drawY1 = 
				Sprite* const sprite = vSprite(i);
				if ( sprite != 0 )
				{
					Renderer::placeTexture4(panelX1+(sprite->x1*scalingFactor),panelY1+(sprite->y1*scalingFactor),
					panelX1+(sprite->x2*scalingFactor),panelY1+(sprite->y2*scalingFactor),sprite->currentTexture(),false);
				}
			}
			
			//simple text overlay for now.

			texOverlay.fill(0);


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

							texOverlay.copyDown(font->aTexFont[(unsigned char)aCharMode(_x,_y)],font->nX*_x,font->nY*_y);
							//texScreen.morphDown(font->aTexFont[(unsigned char)aCharMode(_x,_y)],font->nX*_x,font->nY*_y,100);
						}
						//texRuntime.morphDown(font8x8.aTexFont[(unsigned char)aGlyph[_y][_x]],8*_x,8*_y,22);

						//texScreen.morphDown(font->aTexFont['A'],8*_x,8*_y,25);
					}
				}
			}
			bindNearestNeighbour(&texOverlay);
			Renderer::placeTexture4(panelX1,panelY1,panelX2,panelY2,&texOverlay,false);
			unbind(&texOverlay);

			//return;

			if (scanLines)
			{
				// this looks quite bad with non-integer scaling factors. I reckon a
				// texture overlay will probably look better.
				for (double _y=panelY2-(scalingFactor);_y>=panelY1;_y-=scalingFactor)
				{
					Renderer::placeLineAlpha(0,0,0,80,panelX1,_y,panelX2,_y,scalingFactor/2);
				}
				// for (int _x=panelX1;_x<panelX1+320*scalingFactor;_x+=scalingFactor)
				// {
				// //Renderer::placeLineAlpha(0,0,0,80,_x,panelY1,_x,panelY1+200*scalingFactor);
				// }
			}
		}
	}

	void setFont (Wildcat::Font* _font) override
	{
		if ( _font == 0 )
		{
			font=0;
			return;
		}
		font = _font;

		nCharX = nX / _font->nX;
		nCharY = nY / _font->nY;
		aCharMode.init(nCharX,nCharY,' ');
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

	void shiftCharUp(int amount)
	{

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

// Custom algorithms for filters and effects designed to be overlaid on the main screen
class PixelScreen_Filter
{


};

#endif