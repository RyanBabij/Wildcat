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

// EASI interpreter
#include <Game/EASI/EASI.cpp>

#include <Container/String/ANSI.hpp>

#include <Graphics/PixelScreen/PixelScreen.cpp>

// Stores any hardcoded programs that can run inside the Terminal
class Terminal_Program;

class Terminal: public GUI_Interface, public LogicTickInterface, public IdleTickInterface
{
   // Renders per-pixel output
   PixelScreen pixelScreen;
   
   // Keep track of screen state... Probably combo of char grid and ArrayS3 pixelgrid.
   
   
   unsigned short int cursorX,cursorY;
   unsigned short int nCharX,nCharY; // Number of characters per column/row. Determined by screen and font size.
   int cursorBlink; /* counts up from zero */
   
   bool errorScreenActive; /* true if player broke it */
   bool cursorBlinkOn; /* regulate cursor blink */
   
   bool bootScreen; /* True if boot should play */
   
   std::string strTyping;
   
   Timer timerTyping;
   Timer timerCursorBlink;
   
   std::string input;
   std::string finalInput;
   
   Vector <Terminal_Program*> vProgram;

   public:
   
   short int mouseX, mouseY; //in pixelscreen pixels.
   
   unsigned char amountStatic; // amount of static to render
   
   bool allowInput;
   bool cursorVisible; /* whether to render cursor */
   
   bool subpixelScaling; // Scale up by whole pixels or just fill entire space.
   
   // INIT
   Terminal();
   ~Terminal();
   void init() override;

   // IO STUFF
   
      // Write string at current cursor position.
   void write(std::string _str, bool moveCursor=true, bool instant=false);
   void write(unsigned char _char, bool moveCursor=true, bool instant=false);

   std::string getInput(); // Wait for input terminated with newline.
   
   void clearScreen(bool /* forced */);
   void fill(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
   
   void putCursor(unsigned short int _x, unsigned short int _y);
   
   void loadAudio(); // Load audio files and initialize audio player
   
   void errorScreen(); // Render the errorscreen.
   
   bool isSafe (unsigned short int _x, unsigned short int _y);
   
   // TEXT
   void newLine(short int amount=1);
   void shiftUp(short int amount);
   void type(std::string _str); // print string with typing effect.
   // advance cursor by amount. Scroll the screen if necessary.
   void advanceCursor(unsigned short int amount=1);
   void backspace(int amount=1);
   void putChar(const unsigned short int x, const unsigned short int y, const unsigned char _char);
   void putString(const unsigned short int x, const unsigned short int y, std::string _str);

   // rendering
   void placeTexture4(const int _x1, const int _y1, const int _x2, const int _y2, HasTexture* _texture, const bool preserveAspectRatio /* Default false. */);
   
   //pixelscreen
   void addSprite(Sprite* sprite);
   void setPixel(const short int _x, const short int _y, const short int _r, const short int _g, const short int _b);
   void setTextOverlay(std::string _strOverlay);
   void clearOverlay();

   // TERMINAL_PROGRAM
   void addProgram(Terminal_Program* _program);

   // GUI STUFF

   void render() override;
   bool renderProgram();
   bool keyboardEvent(Keyboard* /* _keyboard */) override;
   bool mouseEvent(Mouse* /* _mouse */) override;
   void eventResize() override;
   void idleTick() override;
   void setFont(Wildcat::Font* _font) override;



};

#endif
