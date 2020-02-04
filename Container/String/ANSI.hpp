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
   
   CURSOR MOVEMENT
   
   ANSI_Grid now supports scrolling. A buffer should be implemented to prevent input strings being too long.
   
   I made a big mistake and hardcoded this for 64x32 resolution, but now I realise I want to start out with 40x25 resolution. I'll fix it later but for now I just modified isSafe() for the new dimensions and that seems to be good enough for now.
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
					c.setANSI(vParam(i),vParam.contains(1));
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
	
	bool isSetCursor()
	{
		if ( finalByte == 'H' || finalByte == 'f'  )
		{
			return (vParam.size() >= 2);
		}
		return false;
	}
	bool isMoveCursor()
	{
		if ( finalByte == 'A' || finalByte == 'B' || finalByte == 'C' || finalByte == 'D'  )
		{
			return (vParam.size() == 1);
		}
		return false;
	}
	
	void setCursor(int& _x, int& _y)
	{
		if (isSetCursor())
		{
			_x = vParam(1);
			_y = vParam(0);
		}
	}
   void moveCursor(int& _x, int& _y)
   {
      --_x;
   }

};

/*
ANSII string class allows colour codes only.
It's really bad tbqh, don't use it.
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
      
      if ( _str.size()==1)
      {
         ansiString=_str;
         vForegroundColour.push(currentForegroundColour);
         vBackgroundColour.push(currentBackgroundColour);
      }
		
		for (unsigned int i=0;i<_str.size()-1;++i) // note that 1 char strings aren't processed.
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
	
	ANSI_Grid(unsigned short int nX=0, unsigned short int nY=0)
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
		return (_x >= 0 && _x < 40 && _y >= 0 && _y < 25);
		//return (_x >= 0 && _x < 64 && _y >= 0 && _y < 48);
	}
   
   void shiftUp()
   {
      //shift all lines up
      for (int _x=0;_x<64;++_x)
      {
         for (int _y=0;_y<47;++_y)
         {
            aGlyph[_y][_x] = aGlyph[_y+1][_x];
            aColour[_y][_x] = aColour[_y+1][_x];
         }
      }
      //blank lowest line
      for (int _x=0;_x<64;++_x)
      {
         aGlyph[47][_x] = ' ';
         aColour[47][_x].set(255,255,255,255);
      }
   }
	
	// Move cursor one space right, or to beginning of new line.
	void advanceCursor()
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
            //shift everything up 1 row
            shiftUp();
            cursorX=0;
			}
	}
	
	void newLine()
	{
			if (isSafe(0,cursorY+1))
			{
				cursorX=0;
				++cursorY;
			}
         else
         {
            shiftUp();
            cursorX=0;
         }
	}
	
	void resetColour()
	{
		currentBackgroundColour = defaultBackgroundColour;
		currentForegroundColour = defaultForegroundColour;
	}
	
	void read(std::string _str)
	{
      //_str="TEST1\nTEST2\nTEST3\n\n\n\n\\n\n\n\n\nn\n\n\n\n\n\n\n\n\n\n\n\ntest4\n\n\n\n\n\n\n\n\n\n\n\ntest5\n\n\n\n\n\n\n\n\nTEST^\nTEST7\n\n\nTEST8\ntest9\ntest10\n\n\n\n\n";
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
				else if (_code->isSetCursor())
				{
					_code->setCursor(cursorX,cursorY);
				}
            else if (_code->isMoveCursor())
            {
               _code->moveCursor(cursorX,cursorY);
            }
				// position string iterator past the code
				if (_code != 0)
				{
					i += _code->codeSize+2;
					delete _code;
				}
			}
			else if (_str[i] == '\n')
			{
            newLine();
			}
         else if (_str[i] == '\b')
         {
            backspace();
         }
			else
			{
				aGlyph[cursorY][cursorX] = _str[i];
				aColour[cursorY][cursorX] = currentForegroundColour;
				
				advanceCursor();
			}

		}
	}
   
   void backspace() // Could be \b or an ANSI code.
   {
      //move cursor back one space and erase whatever's there
      
      if (isSafe(cursorX-1,cursorY))
      {
         --cursorX;
         aGlyph[cursorY][cursorX] = ' ';
         aColour[cursorY][cursorX] = defaultForegroundColour;
      }
      else if (isSafe(0,cursorY-1))
      { // backspacing up a row is considered erasing the \n.
         cursorX=63; // this should be fixed, but we need to implement null chars.
         --cursorY;
         aGlyph[cursorY][cursorX] = ' ';
         aColour[cursorY][cursorX] = defaultForegroundColour;
      }
   }
};

#endif
