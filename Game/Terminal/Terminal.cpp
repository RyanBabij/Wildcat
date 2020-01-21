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

   putCursor(0,0);

   loadAudio();

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

void Terminal::writeString(int _x, int _y, std::string _str, bool moveCursor, bool instant)
{
}

void Terminal::render()
{  
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
   return false;
}

void Terminal::shiftUp(short int amount)
{
}

void Terminal::eventResize()
{
   pixelScreen.setPanel(panelX1,panelY1,panelX2,panelY2);
}

void Terminal::idleTick()
{
   pixelScreen.idleTick();
}

void Terminal::setFont(Wildcat::Font* _font)
{
   font = _font;
   pixelScreen.setFont(_font);
}

#endif