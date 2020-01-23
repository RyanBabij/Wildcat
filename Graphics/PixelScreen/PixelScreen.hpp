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
   
   Todo:
   
   * State updates should be decoupled from render calls.

*/

#include <Container/ArrayS3/ArrayS3.hpp>

class PixelScreen: public GUI_Interface, public IdleTickInterface
{
   ArrayS3 <unsigned char> aScreenDataBuffer; // Desired state of screen
   ArrayS3 <unsigned char> aScreenDataReal; // Actual state of screen after effects
   ArrayS2 <unsigned char> aCharMode; // Grid for drawing fonts onto screen.
    
   unsigned char rngPool [1000]; // for generating static
   
   
   Timer updateTimer;
   
   unsigned short int nX, nY; // number of pixels (not size of panel)
   unsigned short int nCharX, nCharY;

   public:
   
   Texture texScreen; // dynamically generated texture
   
   int fadeSpeed; // max rgb value change per frame
   double updatesPerSecond; // amount of times to update screen state per second
   // I might want to just link it with the global idleTick speed.
   // Or have a postRender() function which preps for the next render.
   
   PixelScreen(const unsigned short int _nX, const unsigned short int _nY) // define the size in pixels
   {
      nX=_nX;
      nY=_nY;
      
      nCharX=0;
      nCharY=0;
      
      active=false;
      panelX1=0; panelY1=0; panelX2=0; panelY2=0;
      font=0;
      aScreenDataBuffer.init(nX,nY,4,0); // RGBA
      aScreenDataReal.init(nX,nY,4,0); // RGBA
      aCharMode.init(0,0,' ');
      
      
      texScreen.create(nX,nY,1,true); // we might instead use this as render
      
      updateTimer.init();
      updateTimer.start();
      
      updatesPerSecond=60;
   }
   
   void init()
   {
      for (int i=0;i<1000;++i)
      {
         rngPool[i]=Random::randomInt(255);
      }
   }
   
   void clear()
   {
      aScreenDataBuffer.fill(0); // RGBA
      aScreenDataReal.fill(0); // RGBA
      aCharMode.fill(' ');
   }
   
   void putChar (const unsigned short int _x, const unsigned short int _y, const unsigned char _char)
   {
      aCharMode(_x,_y)=_char;
   }
   
   /* MouseInterface:: */
   bool mouseEvent (Mouse* _mouse) override
   {
      return false;
   }
   
   void idleTick() override
   { // I'm not sure if we should have an independent timer for this,
      // or just use logic ticks instead.
      // update state if conditions met
      
      if (font==0) { return; }
      updateTimer.update();
      
      int uUPS = 1000000/updatesPerSecond;
      
      if ( updateTimer.totalUSeconds > uUPS)
      {
         fillStatic(100);
         
         // update screen state
         //texScreen.fillChannel(Random::randomInt(3),Random::randomInt(255));
         //texScreen.fillChannel(Random::randomInt(3),Random::randomInt(255));
               
         // draw font
         for (int _y=0;_y<nCharY;++_y)
         {
            for (int _x=0;_x<nCharX;++_x)
            {
               //texRuntime.morphDown(font8x8.aTexFont[(unsigned char)aGlyph[_y][_x]],8*_x,8*_y,22);
               texScreen.morphDown(font->aTexFont[(unsigned char)aCharMode(_x,_y)],font->nX*_x,font->nY*_y,100);
               //texScreen.morphDown(font->aTexFont['A'],8*_x,8*_y,25);
            }
         }
               
         //draw scanlines
         for (int y=0;y<nY;y+=3)
         {
            for(int x=0;x<nX;++x)
            {
               //texScreen.setPixel(x,y,0,texScreen.uPixel(x,y,0)/2);
               //texScreen.setPixel(x,y,1,texScreen.uPixel(x,y,1)/2);
               //texScreen.setPixel(x,y,2,texScreen.uPixel(x,y,2)/2);
               // texScreen.setPixel(x,y,1,0);
               // texScreen.setPixel(x,y,2,0);
               // texScreen.setPixel(x,y,3,10);
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
   
   void eventResize() override
   {
   }
   
   bool keyboardEvent(Keyboard* _keyboard) override
   {
      return false;
   }
   
   void render() override
   {
      // render state
      texScreen.bloom();
      
      bindNearestNeighbour(&texScreen);
      Renderer::placeTexture4(panelX1,panelY1,panelX2,panelY2,&texScreen,false);
      unbind(&texScreen);
      
      for (int _y=panelY1;_y<panelY2;_y+=2)
      {
         Renderer::placeLineAlpha(0,0,0,200,panelX1,_y,panelX2,_y);
      }
   // for (int _x=panelX1;_x<panelX1+320*scalingFactor;_x+=scalingFactor)
   // {
      // //Renderer::placeLineAlpha(0,0,0,80,_x,panelY1,_x,panelY1+200*scalingFactor);
   // }
   }
   
   void setFont (Wildcat::Font* _font)
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
      //aCharMode.fill(' ');
      
      int nPixels=nX*nY*4;
      
      int i2 = Random::randomInt(1000);
      
      for (int i=0;i<nPixels;++i)
      {
         aScreenDataBuffer.data[i]=rngPool[i2];
         aScreenDataReal.data[i]=aScreenDataBuffer.data[i];
         texScreen.data[i]=aScreenDataReal.data[i];
         
         ++i2;
         if ( i2>=1000) { i2 = Random::randomInt(100); }
      }
   }
   
   void shiftCharUp(int amount)
   {

   }
};

#endif