#pragma once
#ifndef WILDCAT_TERMINAL_PROGRAM_CPP
#define WILDCAT_TERMINAL_PROGRAM_CPP

/* Wildcat: Terminal_Program
   #include <Game/TerminalTerminal_Program.cpp>
   
   Implementation of Terminal_Program.
*/

#include <File/FileManagerStatic.hpp>
#include <File/FileManager.hpp>

#include "Terminal_Program.hpp"
#include "Terminal.hpp"

Terminal_Program::Terminal_Program(Terminal * termPtr)
{
   terminal=termPtr;
   active=false;
   programName="";
   graphicsMode=false;
   isBusy=false;
}

Terminal_Program::~Terminal_Program()
{
}

std::string Terminal_Program::init (Vector <std::string>* vArg)
{
   return "Error: This is a virtual function.";
}

std::string Terminal_Program::render()
{
   return "TEXT RETURN\n\nlol";
}
 
 void Terminal_Program::cycle()
 {
 }
 
void Terminal_Program::keyboardEvent (Keyboard* _keyboard)
{
   
}
bool Terminal_Program::mouseEvent (Mouse* _mouse)
{
   return false;
}

#endif