#pragma once
#ifndef STRING_ANSI_HPP
#define STRING_ANSI_HPP

/* Wildcat: ANSI
	#include <String/ANSI.hpp>
	
	The ANSI class strips ANSI escape sequences from a string and provides
	them in a state which can easily be read. Only some codes are implemented.
	
	Notes on ANSI codes:
	
	0 - Reset all to defaults.
	1 - Bold or increased intensity.
	30-37 - Set foreground colour.
	40-47 - Set background colour.
	
	Colours:
	
	Black
	Red
	Green
	Yellow
	Blue
	Magenta
	Cyan
	White
	
	
	ANSI Summary
	
	There are two bytes to indicate an ANSI escape sequence (CSI). The first is ESC / 27 / 0x1B.
	This is a standard ASCII escape code. Next ANSI is specified using  ESC [, 155, 0x9B.
	
	The code can then be followed by any number of "parameter bytes" in the range 0x30–0x3F.
	These correspond to ASCII characters: 0–9:;<=>?
	The code can then be followed by any number of "intermediate bytes" in the range 0x20–0x2F/
	These correspond to ASCII space, and also: !"#$%&'()*+,-./
	The ANSI escape code is then finalised with a single "final byte" in the range 0x40–0x7E. The final
	byte indicates the actual operation.
	
	However for the sake of simplicity, we will assume that all codes will contain 0+ numbers separated by ;, and then a final byte.
	
	ANSII is designed to output text on grids, so ANSII processes the string and outputs the result onto an array.


	ANSI TYPE LIST
	
	FOREGROUND_BLACK 30
	FOREGROUND_RED 31
	FOREGROUND_GREEN 32
	FOREGROUND_YELLOW 33
	FOREGROUND_BLUE 34
	FOREGROUND_MAGENTA 35
	FOREGROUND_CYAN 36
	FOREGROUND_WHITE 37
*/

#include <iostream>
#include <string>
#include <regex>

//#define FOREGROUND_BLACK 30
//#define FOREGROUND_RED 31

class ANSI_Code
{
	public:
	
		// COLOUR
	std::string type;
		// BLACK, RED, ETC
	std::string subType;
	
	//int code1;
	//int code2;
	
	Vector <int> vParam; /* Vector of parameter bytes, converted to numbers */
	unsigned char finalByte;
	
	int codeSize; // How far to skip ahead after reading code.

	ANSI_Code(std::string _code, int pos=0) //assume we are being passed from the parameter bytes.
	{
		
		//build vector of parameter bytes, separated by ;.
		// terminate at value 0x40 or higher.
		
		std::string currentParam;
		for (unsigned int i=pos;i<_code.size();++i)
		{
			if ( _code[i] >= 64 ) // If we are at final byte
			{
				pushParam(currentParam);
				currentParam="";
				finalByte = _code[i];
				codeSize=i-pos;
				return;
			}
			else if (DataTools::isNumeric(_code[i]))
			{
				currentParam+=_code[i];
			}
			else if (_code[i] == ';') // Param delimiter
			{
				pushParam(currentParam);
				currentParam="";
			}
		}
		
	}
	
	void pushParam(std::string _param="")
	{
		if ( _param!="")
		{
			vParam.push(DataTools::toInt(_param));
		}
	}
	
	bool isReset()
	{
		return (finalByte == 'm' && vParam.contains(0));
	}
	
	bool isForegroundColour()
	{
		if ( finalByte == 'm' )
		{
			for (int i=0;i<vParam.size();++i)
			{
				if ( vParam(i) >= 30 && vParam(i) <= 37 )
				{
					return true;
				}
			}
		}
		return false;
	}
	
	// Returns white if there is no colour specification.
	Colour getForegroundColour()
	{
		
		Colour c;
		
		if ( finalByte == 'm' )
		{
			for (int i=0;i<vParam.size();++i)
			{
				if ( vParam(i) >= 30 && vParam(i) <= 37 )
				{
					c.setANSI(vParam(i));
					return c;
				}
			}
		}
		c.set(255,255,255,255);
		return c;

	}
	
	// bool isBackgroundColour()
	// {
		// return true;
	// }
	
	// Colour getBackgroundcolour()
	// {
		// Colour c;
		// return c;
	// }

};

/*
ANSII string class allows colour codes only.
*/

class ANSI
{
	public:
	
	Vector <ANSI_Code*> vCode;
	
	std::string ansiString; /* STRIPPED ANSI STRING */
	Vector <Colour> vForegroundColour; /* Colour of each character */
	Vector <Colour> vBackgroundColour; /* Colour of each character */
	
	Colour defaultForegroundColour;
	Colour defaultBackgroundColour;
	
	Colour currentForegroundColour;
	Colour currentBackgroundColour;
	
	ANSI()
	{
		ansiString="";
		
		setDefaultForeground(255,255,255,255);
		setDefaultBackground(0,0,0,0);
	}
	
	void printColours()
	{
		std::cout<<"Printing colours:\n";
		for (int i=0;i<vForegroundColour.size();++i)
		{
			Colour c = vForegroundColour(i);
			std::cout<<"("<<(int)c.red<<","<<(int)c.green<<","<<(int)c.blue<<")";
		}std::cout<<"\n";
	}
	
	void setDefaultForeground(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		defaultForegroundColour.set(r,g,b,a);
		currentForegroundColour.set(defaultForegroundColour);
	}
	void setDefaultBackground(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		currentBackgroundColour.set(r,g,b,a);
		currentBackgroundColour.set(defaultBackgroundColour);
	}
	
	
	
	void resetColour()
	{
		currentForegroundColour.set(defaultForegroundColour);
		currentBackgroundColour.set(defaultBackgroundColour);
	}
	
	unsigned int size()
	{
		return ansiString.size();
	}
	
	// returns the amount of chars to skip ahead.
	int strip(std::string _str, int pos)
	{
		return 0;
	}
	
	// Process the escape code, and remember the position of it
	// void saveCode(std::string _code, int pos)
	// {
		// vCode.push(new ANSI_code (_code,pos));
	// }
	
	// Read in a string and build a Vector of the escape commands.
	// returns a string with all commands stripped.
	void read(std::string _str)
	{
		unsigned char escape1 = 27;
		unsigned char escape2 = '[';
		unsigned char finalByte = 'm';

		std::string escapeCode = "";
		
		for (unsigned int i=0;i<_str.size()-1;++i)
		{
			if ((unsigned char)_str[i]==escape1 && (unsigned char)_str[i+1] == escape2)
			{
				for (unsigned int i2=i+2;i2<_str.size();++i2)
				{
					//escapeCode += _str[i2];
					if ( _str[i2] == finalByte )
					{
						int len = (i2-i) +1;
						//Strip the entire escape sequence from the string
						_str.erase(i,len);
						
						//save the escape code and position it occurs.
						//saveCode (escapeCode,i);

						ANSI_Code ansiCode (escapeCode);
						
						if (ansiCode.isForegroundColour())
						{
							currentForegroundColour = ansiCode.getForegroundColour();
						}
						else if (ansiCode.isReset())
						{
							resetColour();
						}
						//ansiCode.read(escapeCode);
						
						// Move iterator back 1 to account for deletion.
						--i;
						
						escapeCode="";
						break;
					}
					else
					{
						escapeCode+= _str[i2];
					}
				}
			}
			else
			{
				ansiString += _str[i];
				vForegroundColour.push(currentForegroundColour);
				vBackgroundColour.push(currentBackgroundColour);
				
				if (i == _str.size() - 2)
				{
					ansiString+=_str[i+1];
					vForegroundColour.push(currentForegroundColour);
					vBackgroundColour.push(currentBackgroundColour);
				}
			}
		}

	// std::cout<<"\nStripped codes:\n";
	// for (int i=0;i<vCode.size();++i)
	// {
		// std::cout<<vCode(i)->type<<".\n";
	// } std::cout<<"end\n";

		//return _str;
	}
	
};


//Read ANSI string and output it to a grid, for use in Terminals.

class ANSI_Grid
{
	
	public:
	
	Vector <ANSI_Code*> vCode;
	
	std::string ansiString; /* STRIPPED ANSI STRING */
	//Vector <Colour> vForegroundColour; /* Colour of each character */
	//Vector <Colour> vBackgroundColour; /* Colour of each character */
	
	Colour defaultForegroundColour;
	Colour defaultBackgroundColour;
	
	Colour currentForegroundColour;
	Colour currentBackgroundColour;
	
	char aGlyph [48][64]; /* Final output goes here */
	Colour aColour [48][64];
	
	int cursorX, cursorY;
	
	ANSI_Grid()
	{
		ansiString="";
		cursorX=0;
		cursorY=0;
		
		defaultForegroundColour.set(255,255,255,255);
		defaultBackgroundColour.set(0,0,0,0);
		
		currentForegroundColour = defaultForegroundColour;
		currentBackgroundColour = defaultBackgroundColour;
		
		for (int _y=0;_y<48;++_y)
		{
			for (int _x=0;_x<64;++_x)
			{
					aGlyph[_y][_x] = ' ';
					aColour[_y][_x]=currentForegroundColour;
			}
		}
		

	}
	
	bool isSafe(int _x, int _y)
	{
		return (_x >= 0 && _x < 64 && _y >= 0 && _y < 64);
	}
	
	// Move cursor one space right, or to beginning of new line.
	// Return false if at end of screen.
	bool advanceCursor()
	{
			if (isSafe(cursorX+1,cursorY))
			{
				++cursorX;
			}
			else if (isSafe(0,cursorY+1))
			{
				cursorX=0;
				++cursorY;
			}
			else
			{
				return false;
			}
			return true;
	}
	
	bool newLine()
	{
			if (isSafe(0,cursorY+1))
			{
				cursorX=0;
				++cursorY;
				return true;
			}
			return false;
	}
	
	void resetColour()
	{
		currentBackgroundColour = defaultBackgroundColour;
		currentForegroundColour = defaultForegroundColour;
	}
	
	void read(std::string _str)
	{
		for (unsigned int i=0;i<_str.size();++i)
		{
			// check for ANSI escape code.
			if ( i < _str.size() -2 && _str[i] == 27 && _str[i+1] == '[')
			{
				// read in the code
				ANSI_Code * _code = new ANSI_Code(_str,i+2);
				
				if (_code->isForegroundColour())
				{
					currentForegroundColour = _code->getForegroundColour();
				}
				else if (_code->isReset())
				{
					resetColour();
				}
				
				// for now just skip the code
				if (_code != 0)
				{
					i += _code->codeSize+2;
					delete _code;
				}
			}
			else if (_str[i] == '\n')
			{
				if (newLine() == false)
				{
					return;
				}
			}
			else
			{
				aGlyph[cursorY][cursorX] = _str[i];
				aColour[cursorY][cursorX] = currentForegroundColour;
				
				if ( advanceCursor() == false )
				{
					return;
				}
			}

		}

	}
};

#endif
