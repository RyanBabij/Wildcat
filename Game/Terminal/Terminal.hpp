#pragma once
#ifndef WILDCAT_GAME_TERMINAL_HPP
#define WILDCAT_GAME_TERMINAL_HPP

/* Wildcat: Terminal
   #include <Wildcat/Game/Terminal/Terminal.hpp>
   
   Terminal is a class which creates a faux-retro computer terminal.
   The Terminal makes use of my EASI programming language, and also
   provides functions to provide output from the terminal, and handle
   player input.
   
   The terminal is also designed to run a set of Terminal_Program objects
   which can render to the terminal screen and receive keyboard and/or
   mouse input. Once EASI is fully developed it should be technically
   possible to write the same programs from within the Terminal itself.
   
   The class is intended for making hacking-style games, and additionally
   to provide a computer GUI for more complex games.
   
   Terminal handles IO and running programes, whereas PixelScreen handles
   the rendering stuff.
   
   Actual programs that use the Terminal class will implement their own
   program set, resolution and style.
   
   PixelScreen also handles font overlays, including PETSCII/ASCII stuff,
   so those effects should be moved to PixelScreen.
   
   Terminal will have an ASCII/PETSCII and graphics mode. These modes
   can also be overlaid.
   
   Terminal only handles the Terminal screen itself. Any external stuff like
   power buttons should be handled by a Menu.
*/

#include <string>

/* Audio stuffs. */
#include <Audio/Sound.hpp> // Generic sound data handler
#include <Audio/Wav.hpp>
#include <Audio/AudioPlayer.hpp> // Generic audio player
#include <Audio/AudioPlayer_OpenAL.hpp>
/* Global OpenAL audio player. */
AudioPlayer_OpenAL globalAudioPlayer;
/* Dial tones. */
Sound * dialTone[10];
Sound* sRing;

#include <System/Time/Timer.hpp>
Timer toneTimer; /* Keep track of when last tone was played */

#include <Data/Tokenize.hpp> // Tokenize console commands
#include <Data/DataTools.hpp> // Check commands

// #include "Server.hpp"
// #include "Operator.hpp"
//Operator op;

// EASI interpreter
#include <Game/EASI/EASI.cpp>

#include <Container/String/ANSI.hpp>

/* Class to store the Terminal's files which can be default system files,
or created/modified by the user. Files will be read/written to disk
so user can edit them externally if they prefer. Extension .txt is added
for readability but is not required. */

//#include "Terminal_Program.cpp"

//#include "MemoryMap.hpp"

// This needs to become timer-based
#define TERM_GLYPH_DELAY 4

//#include "Terminal_Screen.hpp"

#include <Graphics/PixelScreen/PixelScreen.cpp>

class Terminal: public GUI_Interface, public LogicTickInterface, public IdleTickInterface
{
   // Renders per-pixel output
   PixelScreen pixelScreen;
   
   // Keep track of screen state... Probably combo of char grid and ArrayS3 pixelgrid.
   
   
   int cursorX,cursorY;
   int cursorBlink; /* counts up from zero */
   
   bool errorScreenActive; /* true if player broke it */
   bool cursorVisible; /* regulate cursor blink */
   
   bool bootScreen; /* True if boot should play */

   public:
   
   // INIT
   Terminal();
   ~Terminal();
   void init();

   // IO STUFF
   
   void writeString(int _x, int _y, std::string _str, bool moveCursor, bool instant);
   void getInput(); // Wait for input terminated with newline.
   
   void clearScreen(bool /* forced */);
   void putCursor(unsigned short int _x, unsigned short int _y);
   
   void loadAudio(); // Load audio files and initialize audio player
   
   void errorScreen(); // Render the errorscreen.
   
   bool isSafe (unsigned short int _x, unsigned short int _y);
   
   void shiftUp(short int amount);


   // GUI STUFF

   void render() override;
   bool renderProgram();
   bool keyboardEvent(Keyboard* /* _keyboard */) override;
   void eventResize() override;
   void idleTick() override;
   void setFont(Wildcat::Font* _font) override;



};

#endif
