#pragma once
#ifndef WILDCAT_GAME_TERMINAL_CPP
#define WILDCAT_GAME_TERMINAL_CPP

/* Wildcat: Terminal
   #include <Wildcat/Game/Terminal/Terminal.cpp>
   
   Implementation of Terminal.hpp
*/

#include "Terminal.hpp"

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
   
   strTyping="";
   input="";
   finalInput="";
   
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
   pixelScreen.render();
}

void Terminal::putCursor(unsigned short int _x, unsigned short int _y)
{
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
      // Get whatever the user typed.
      else if (_keyboard->lastKey == Keyboard::ENTER && strTyping.size()==0 )
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
      _keyboard->clearAll();
      return true;
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

void Terminal::eventResize()
{
   pixelScreen.setPanel(panelX1,panelY1,panelX2,panelY2);
}

void Terminal::idleTick()
{
   if (strTyping.size() > 0 )
   {
      timerTyping.update();
      
      //if (timerTyping.fullSeconds>0.08)
      if (timerTyping.fullSeconds>0.01)
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

#endif