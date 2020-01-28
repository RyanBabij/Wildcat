#pragma once
#ifndef WILDCAT_TERMINAL_PROGRAM_HPP
#define WILDCAT_TERMINAL_PROGRAM_HPP

/* Wildcat: Terminal_Program
   #include <Game/Terminal/Terminal_Program.hpp>
   
   Interface for programs to provide custom programs for Terminal class.
   
   Allows access to pixel output and mouse and keyboard input.
   
   In future it may be possible to replace this functionality with the EASI
   language, however this class will remain for anything that needs to be
   hardcoded.
*/ 

class Terminal;

class Terminal_Program
{
   public:
   Terminal * terminal; /* Link to Terminal to call system functions and access RAM */
   
   
   bool active; /* true means the program is running */
   //Terminal* terminal;
      /* If true, Terminal will constantly refresh render,
         otherwise render calls will be cumulative */
   bool graphicsMode;
   
   //bool isWaitingInput; // If the program is in ASCII mode, it must be able to flag for input.
   bool isBusy; // flag to terminal to indicate work
   
   std::string programName;
   
   Terminal_Program(Terminal*  test);
   //Terminal_Program();
   virtual ~Terminal_Program();
   
   /* Pass the commands written after the program name.
   Return empty string if init successful, otherwise return error message */
   virtual std::string init (Vector <std::string>* vArg);
   
   // Render and logic cycle are decoupled but will typically run at the same rate. */
   virtual std::string render(); /* render at set framerate */
   virtual void cycle(); /* advance 1 game cycle. Maybe return a status. */
   
   virtual void keyboardEvent (Keyboard*);
   virtual bool mouseEvent (Mouse*);
};


#endif