# Wildcat
The Wildcat library is my personal code library for game development. It contains code relating to the creation of videogames. I just add functions as I need them.

#### Audio
Contains code for playing audio, using generic Sound object. Currently only supports WAV with OpenAL.

#### Container

* 2D Array wrapper
* 3D Array wrapper
* Bitfield
* String (not used much)
* Table
* Vector
* Wordlist

#### Data
Code for operations which involve datatypes or standalone data, for example bitwise operations, conversions between datatypes, short mathematical operations, serialisation, tokenisation, checksum, debugging, etc.

#### Device
Code for handling display output, and keyboard and mouse input.

* DisplayInterface
* DisplayInterfaceManager
* Keyboard
* KeyboardInterface
* KeyboardInterfaceManager
* Mouse
* MouseInterface
* MouseInterfaceManager

#### File
Code for file operations. Allows easy creation, deletion, and modification of files. Also contains tools for savefiles. WTFManager is my attempt at a unified file format for saving/loading data.

* FileManager (Not used much)
* FileManagerStatic (for discrete independent file operations)
* WTFManager


#### Game
Any code which is mostly used for games. Name generators, Board object for 2D tile-based games, EASI programming language interpreter, World Generator.

* Board (Grid-style container for 2D game worlds)
* BoardViewer (Handles rendering of boards)
* EASI (BASIC-like programming language)
* Meditations (wiki)
* NameGen (name generator)
* Terminal (terminal emulator C64 style)
* WorldGenerator (generates heightmaps biomes and output map to PNG)

#### Graphics
Code relating to textures, images and fonts. Uses OpenGL, FreeGLUT, GLEW, lodepng, picopng. Also contains code for OpenGL rendering, with option for DirectX support later. Includes ugly but functional code for rendering custom GUI elements. Also includes code for loading PNGs into Textures. I think this has a lot of old unused stuff.

* Bitmap
* Colour
* Curses
* Font (Font loading/saving/rendering, currently only bitmap fonts supported)
* GUI
* Image
* OpenGL
* PixelScreen (Pixel array which can then be rendered)
* PNG (PNG loading/saving/modification)
* Region
* Render
* Texture (my container for all Textures, generally stored as PNG)

#### Interface
Code for various interfaces, like handling Idle cycles or ticks. Objects which return Textures and Coordinates.

* IdleTick (called when program is idle, useful for background processing)
* LogicTick (called at defined periods when game needs to update)
* HasClassID
* HasTexture (allows object to be rendered)
* HasXY (allows coordinate operations)

#### Lib
Any libs I include go here, currently FreeGLUT and GLEW.

#### Math
Code for mathematical algorithms, includng random number generation. Includes geometry, fractals, prime numbers, and expression evaluation.

* BasicMath
* Fractal (midpoint displacement and diamond-square)
* Geometry
* Primes (very basic prime checking/calculating)
* Random (C++11 Mersenne Twister and optimised Lehmer RNG).
* Shunting (used for evaluation of mathematical expressions)
* Operation

#### System
Stuff which relies on OS functionality. Sleep, threads (not so much with C++17), time and timers. OS-specific defines.

* Sleep (cross-platform sleep)
* Thread (atomic, mutex, thread)
* Time (calendar, timer)

### Contributions

Contributions welcome although it's a personal code library so unlikely anyone else will be using it.

### License

This code library will remain open source and public domain.

### Style guide

I prefer the Allman style. Column width is 120 (used to be 80 but modern apps seem to support 120). Function names should be descriptive rather than short.

Classes are UpperCamelCase, objects are camelCase, functions are camelCase. Iterators/indexes start with i, arrays start with a, vectors start with v, global variables/defines are UPPERCASE, local shadow variables start with an \_underscore.

Indentation is tabbed, best viewed at 3 spaces per tab.

Indentation is a mess. I'm in the process of moving from spaced indentation to tabbed indentation, because I discovered that you can set Github's tab width with .editorconfig.
