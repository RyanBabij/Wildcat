# Wildcat
The Wildcat library is my personal code library for game development. It contains code relating to the creation of videogames. I just add functions as I need them.

#### Audio
Contains code for playing audio, using generic Sound object. Currently only supports WAV with OpenAL.

#### Container
2D arrays, 3D arrays, vectors and strings.

#### Data
Code for operations which involve datatypes or standalone data, for example bitwise operations, conversions between datatypes, short mathematical operations, serialisation, tokenisation, checksum, debugging, etc.

#### Device
Code for handling display output, and keyboard and mouse input.

#### ErrorHandling
Probably should be in Debug.

#### File
Code for file operations. Allows easy creation, deletion, and modification of files. Also contains tools for savefiles.

#### Game
Any code which is mostly used for games. Name generators, Board object for 2D tile-based games, EASI programming language interpreter, World Generator.

#### Graphics
Code relating to textures, images and fonts. Uses OpenGL, FreeGLUT, GLEW, lodepng, picopng. Also contains code for OpenGL rendering, with option for DirectX support later. Includes ugly but functional code for rendering custom GUI elements. Also includes code for loading PNGs into Textures.

#### Interface
Code for various interfaces, like handling Idle cycles or ticks. Objects which return Textures and Coordinates.

#### Lib
Any libs I include go here, currently FreeGLUT and GLEW.

#### Math
Code for mathematical algorithms, includng random number generation. Includes geometry, fractals, prime numbers, and expression evaluation.

#### System
Stuff which relies on OS functionality. Sleep, threads (not so much with C++17), time and timers. OS-specific defines.
