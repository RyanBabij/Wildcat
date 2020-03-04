#pragma once
#ifndef WILDCAT_WINDOWS_HPP
#define WILDCAT_WINDOWS_HPP

/* #include <Wildcat/Windows.hpp>

	All code to provide compatibility with Windows OS. Mainly required because my G++ decided to stop defining WIN32 in windows.
*/

	// This is the internal definition to let us know we're compiling for Windows.
#define WILDCAT_WINDOWS
	// This is the official definition to let us know we're compiling for Windows.
#define WIN32
	// This excludes some APIs such as Cryptography, DDE, RPC, Shell, and Windows Sockets.
   // freeGLUT sets this as 1 so I should follow that convention
#define WIN32_LEAN_AND_MEAN 1


#endif
