// OpenGL includes go here because some global objects depend on OpenGL defines.
#define GLEW_STATIC // static link GLEW 
// Need to figure out which of this is better. I think GLEW is more supported.
#include <Graphics/OpenGL/glew.h> // THIS CURRENTLY FIXES LINKER CRAP. Also allows RGBA_COMPRESSED, it would seem.
#define FREEGLUT_STATIC // static link freeGLUT
#include <Graphics/OpenGL/freeglut.h> //