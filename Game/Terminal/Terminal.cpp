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
   bootScreen=true;
   
   strTyping="";
   
   timerTyping.init();
   timerTyping.start();

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
}

void Terminal::loadAudio()
{
}

void Terminal::clearScreen(bool forced) /* forced will instantly clear the screen, instead of using backlog */
{
   // clear internals and pixelScreen
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
         //backspace();
      }
      else if (_keyboard->lastKey == 3 ) /* CTRL+C */
      {
         shutDown();
      }
      // Get whatever the user typed.
      else if (_keyboard->lastKey == Keyboard::ENTER )
      {
         newLine(1);
      }
      else
      {
         write(_keyboard->lastKey);
      }
      _keyboard->clearAll();
      return true;
   }
   return false;
}

void Terminal::shiftUp(short int amount)
{
}

void Terminal::newLine(short int amount)
{
   cursorX=0;
   ++cursorY;
}

void Terminal::advanceCursor(unsigned short int amount)
{
   if (font==0)
   { return; }

   ++cursorX;
   if ( cursorX>nCharX )
   {
      cursorX=0;
      ++cursorY;
      
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
      
      if (timerTyping.fullSeconds>0.08)
      {
         write(strTyping[0]);
         strTyping.erase(0,1);
         timerTyping.start();
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