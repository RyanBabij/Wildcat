// //Class to return RGB values from palette values, using extended ASCII table (128-255).
// // 4 bytes for foreground, 4 bytes for background. Good for some basic expression
// // without getting too complex.
// // If background is equal to foreground colour, then background is transparent.

// //users can type in colour using ctrl+f and ctrl+b to cycle the colours.

// // This one uses the Windows 16 colour palette:

// /*
// 0 — black	8 — gray
// 1 — maroon	9 — red
// 2 — green	10 — lime
// 3 — olive	11 — yellow
// 4 — navy	12 — blue
// 5 — purple	13 — fuchsia
// 6 — teal	14 — aqua
// 7 — silver	15 — white

// 128 - black text on transparent
// 129 - maroon text on black
// 130 - green text on black
// 131 - olive text on black
// 132 - navy text on black
// 133 - purple text on black
// 134 - teal text on black
// 135 - silver text on black
// 136 - gray text on black
// 137 - red text on black
// 138 - lime text on black
// 139 - yellow text on black
// 140 - blue text on black
// 141 - fuchsia text on black
// 142 - aqua text on black
// 143 - white text on black

// 144 - black text on maroon
// 145 - maroon text on transparent
// 146 - green text on maroon

// etc...

// RGB values:

// black: 0,0,0
// maroon: 128,0,0
// green: 0,128,0
// olive: 128,128,0
// navy: 0,0,128
// purple: 128,0,128
// teal: 0,128,128
// silver: 192,192,192
// gray:
// red:
// lime:
// yellow:
// blue:
// fuchsia:
// aqua:
// white:

// */


// class Palette1616Windows
// {
	// public:
	
	// unsigned char foregroundR;
	// unsigned char foregroundG;
	// unsigned char foregroundB;
	
	// unsigned char backgroundR;
	// unsigned char backgroundG;
	// unsigned char backgroundB;
	
	// Palette1616Windows()
	// {
		// foregroundR=0;
		// foregroundG=0;
		// foregroundB=0;
		// backgroundR=0;
		// backgroundG=0;
		// backgroundB=0;
	// }
	
	// void loadChar(unsigned char _character)
	// {
		// if ( _character < 128)
		// {
			// // default is black text on transparent background
			// foregroundR=0;
			// foregroundG=0;
			// foregroundB=0;
			// backgroundR=0;
			// backgroundG=0;
			// backgroundB=0;
		// }
		// else
		// {
			// //get palette value by subtracting 128
			// // then isolate foreground and background value
			// //convert each to the 16 colours
			
			// _character -= 128;
			// unsigned char foregroundValue = _character % 16;
			// unsigned char backGroundValue = 0;
			// std::cout<<"Foreground value: "<<foregroundValue<<".\n";
			
		// }
	// }
	
	
// };

