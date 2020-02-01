#pragma once
#ifndef WILDCAT_GAME_TERMINAL_CPP
#define WILDCAT_GAME_TERMINAL_CPP

/* Wildcat: Terminal
   #include <Wildcat/Game/Terminal/Terminal.cpp>
   
   Implementation of Terminal.hpp
*/

#include "Terminal.hpp"
#include "Terminal_Program.cpp"

Terminal::Terminal(): pixelScreen(320,200)
{
   init();
}

Terminal::~Terminal()
{
   globalAudioPlayer.close();
}

// Initialize or reset Terminal to boot state
void Terminal::init()
{
   cursorX=0;
   cursorY=0;
   cursorBlink=0;
   errorScreenActive=false;
   cursorVisible=true;
   cursorBlinkOn=true;
   bootScreen=true;
   
   allowInput=true;
   
   strTyping="";
   input="";
   finalInput="";
   
   subpixelScaling=false;
   
   amountStatic=0;
   
   timerTyping.init();
   timerTyping.start();
   timerCursorBlink.init();
   timerCursorBlink.start();

   putCursor(0,0);

   loadAudio();
   
   //pixel count is currently hardcoded.
   nCharX=40;
   nCharY=25;

   //load basic programs
   //vProgram.clearPtr();
   // vProgram.push(new Program_Write(this));
   // vProgram.push(new Program_Run(this));

   clearScreen(true);
   //write any initial stuff here.
   
   pixelScreen.init();
}

std::string Terminal::getInput()
{
   if ( finalInput.size() == 0 )
   { return ""; }
   std::string retInput = finalInput;
   finalInput="";
   return retInput;
}

void Terminal::loadAudio()
{
}

void Terminal::clearScreen(bool forced) /* forced will instantly clear the screen, instead of using backlog */
{
   // clear internals and pixelScreen
   pixelScreen.clear();
}

void Terminal::fill(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a)
{
   // call terminal fill
   pixelScreen.fill(_r,_g,_b,_a);
}

void Terminal::write(const std::string _str, bool moveCursor, bool instant)
{
   for (unsigned int i=0;i<_str.size();++i)
   {
      write(_str[i]);
   }
}
void Terminal::write(const unsigned char _char, bool moveCursor, bool instant)
{
   timerCursorBlink.start(); // keep cursor on while typing
   cursorBlinkOn=true;
   
   if ( _char == '\n' )
   {
      newLine(1);
      return;
   }
   pixelScreen.putChar(cursorX,cursorY,_char);
   advanceCursor();
}

void Terminal::render()
{
   pixelScreen.amountStatic=amountStatic;
   pixelScreen.render();
   
   //render program if necessary
   
   for (int i=0;i<vProgram.size();++i)
   {
      if (vProgram(i)->active)
      {
         vProgram(i)->render();
      }
   }
   
   // render overlay text
}

void Terminal::putCursor(unsigned short int _x, unsigned short int _y)
{
   cursorX=_x;
   cursorY=_y;
}

bool Terminal::isSafe(unsigned short int _x, unsigned short int _y)
{
   return false;
}

void Terminal::backspace(int amount)
{
   if (cursorX == 0 )
   {
      return;
   }
   pixelScreen.putChar(cursorX,cursorY,' ');
   --cursorX;
   pixelScreen.putChar(cursorX,cursorY,1);
}

bool Terminal::keyboardEvent(Keyboard* _keyboard)
{
   if (_keyboard->keyWasPressed)
   {
      if (_keyboard->lastKey == 18) /* CTRL + R */
      {
         init();
      }
      else if (_keyboard->lastKey == 8 )
      {
         backspace();
      }
      else if (_keyboard->lastKey == 3 ) /* CTRL+C */
      {
         shutDown();
      }
      else if (allowInput)
      {
         // send keyboard to programs
         for (int i=0;i<vProgram.size();++i)
         {
            if ( vProgram(i)->active)
            {
               vProgram(i)->keyboardEvent(_keyboard);
            }
         }
         // Get whatever the user typed.
         if (_keyboard->lastKey == Keyboard::ENTER && strTyping.size()==0 )
         {
            newLine(1);
            finalInput=input;
            input="";
         }
         else if (strTyping.size()==0)
         {
            input+=_keyboard->lastKey;
            write(_keyboard->lastKey);
         }
      }
      _keyboard->clearAll();
      return true;
   }
   return false;
}

bool Terminal::mouseEvent(Mouse* _mouse)
{
   if ( _mouse->inBounds(panelX1,panelY1,panelX2,panelY2) == false )
   { return false; }
   
   pixelScreen.mouseEvent(_mouse);
   
   mouseX=pixelScreen.mouseX;
   mouseY=pixelScreen.mouseY;
   
   for (int i=0;i<vProgram.size();++i)
   {
      if ( vProgram(i)->active)
      {
         vProgram(i)->mouseEvent(_mouse);
      }
   }
   return false;
}

void Terminal::shiftUp(short int amount)
{
   pixelScreen.shiftCharUp(amount);
}

void Terminal::newLine(short int amount)
{
   if ( cursorVisible )
   {
      pixelScreen.putChar(cursorX,cursorY,' ');
   }
   cursorX=0;
   ++cursorY;
   
   if (cursorY >= nCharY )
   {
      cursorY=nCharY-1;
      shiftUp(1);
   }
   
   if ( cursorVisible )
   {
      pixelScreen.putChar(cursorX,cursorY,1);
   }
}

void Terminal::advanceCursor(unsigned short int amount)
{
   if (font==0)
   { return; }

   ++cursorX;
   if ( cursorX>=nCharX )
   {
      cursorX=0;
      ++cursorY;
      
      if ( cursorY >= nCharY )
      {
         cursorY=nCharY-1;
         shiftUp(1);
      }
   }
   
   if (cursorVisible)
   {
      pixelScreen.putChar(cursorX,cursorY,1);
   }
}

void Terminal::type(std::string _str)
{
   strTyping+=_str;
}

   // PIXELSCREEN
void Terminal::addSprite(Sprite* sprite)
{
   pixelScreen.addSprite(sprite);
}
void Terminal::removeSprite(Sprite* sprite)
{
   pixelScreen.removeSprite(sprite);
}
void Terminal::setPixel(const short int _x, const short int _y, const short int _r, const short int _g, const short int _b)
{
   pixelScreen.setPixel(_x, _y, _r, _g, _b);
}
unsigned char Terminal::getPixel(const short int _x, const short int _y, const short int _channel)
{
   return pixelScreen.getPixel(_x,_y,_channel);
}

   // TERMINAL_PROGRAM
void Terminal::addProgram(Terminal_Program* _program)
{
   vProgram.push(_program);
}

void Terminal::eventResize()
{
   //determine the best scaling factor we can fit in the screenspace.
   
   // determine largest scaling multiples which fit into the panel area.
   // I also want to add support for subpixel scaling, although it might
   // make the screen look a bit funny.
   double scalingFactor;
   
   if ( panelNX < 320 || panelNY < 200 )
   {
      scalingFactor=1;
      pixelScreen.scalingFactor=1;
   }
   else
   {
      double scalingFactorX=(double)panelNX/320;
      double scalingFactorY=(double)panelNY/200;
      
      scalingFactor = scalingFactorX;
      if ( scalingFactorY < scalingFactor )
      { scalingFactor = scalingFactorY; }

      if ( subpixelScaling == false )
      {
         scalingFactor = floor(scalingFactor);
      }
      
      pixelScreen.scalingFactor = scalingFactor;
   }
   

   

   pixelScreen.setPanel(panelCenterX - (160*scalingFactor),panelCenterY - (100*scalingFactor),panelCenterX + (160*scalingFactor),panelCenterY + (100*scalingFactor));
}

void Terminal::idleTick()
{
   if (strTyping.size() > 0 )
   {
      timerTyping.update();
      
      if (timerTyping.fullSeconds>0.07)
      //if (timerTyping.fullSeconds>0.01)
      {
         write(strTyping[0]);
         strTyping.erase(0,1);
         timerTyping.start();
      }
   }
   
   if (cursorVisible)
   {
      timerCursorBlink.update();
      if (timerCursorBlink.fullSeconds > 0.5 )
      {
         timerCursorBlink.start();
         
         cursorBlinkOn=!cursorBlinkOn;
         
         if (cursorBlinkOn)
         {
            pixelScreen.putChar(cursorX,cursorY,1);
         }
         else
         {
            pixelScreen.putChar(cursorX,cursorY,' ');
         }
      }
   }

   
   pixelScreen.idleTick();
}

void Terminal::setFont(Wildcat::Font* _font)
{
   font = _font;
   pixelScreen.setFont(_font);
}

void Terminal::putChar(const unsigned short int x, const unsigned short int y, const unsigned char _char)
{
   pixelScreen.putChar(x,y,_char);
}
void Terminal::putString(const unsigned short int x, const unsigned short int y, const std::string _str)
{
   pixelScreen.putString(x,y,_str);
}

void Terminal::setTextOverlay(std::string _strOverlay)
{
   pixelScreen.strOverlay=_strOverlay;
}

void Terminal::clearOverlay()
{
   //pixelScreen.clearOverlay();
}

#endif