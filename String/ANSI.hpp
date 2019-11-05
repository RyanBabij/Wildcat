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
	
	There are two bytes to indicate an ANSI excape sequence. The first is ESC / 27 / 0x1B.
	This is a standard ASCII escape code. Next ANSI is specified using  ESC [, 155, 0x9B.
	
	The code can then be followed by any number of "parameter bytes" in the range 0x40 - 0x7E.
	These correspond to ASCII characters: 0–9:;<=>?
	The code can then be followed by any number of "intermediate bytes" in the range 0x20–0x2F/
	These correspond to ASCII space, and also: !"#$%&'()*+,-./
	The ANSI escape code is then finalised with a single "final byte" in the range 0x40–0x7E.


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

class ANSI_code
{
	public:
	
		// COLOUR
	std::string type;
		// BLACK, RED, ETC
	std::string subType;
	
	int code1;
	int code2;
	

	ANSI_code(std::string _code)
	{
		code1=0;
		code2=0;
		
		if (_code.find("30") != std::string::npos)
		{
				code1=30;
				code2=30;
		}
		else if (_code.find("31") != std::string::npos)
		{
				code1=31;
				code2=31;
		}
		else if (_code.find("37") != std::string::npos)
		{
				code1=37;
				code2=37;
		}
		else if (_code.find("0") != std::string::npos)
		{
				code1=0;
				code2=0;
		}
		
	}

	// ANSI_code(std::string _type)
	// {
		// type = _type;
		
		
		// if (_type.find("30") != std::string::npos)
		// {
				// type = "FOREGROUND_COLOUR";
				// subType = "BLACK";
		// }
		// else if (_type.find("31") != std::string::npos)
		// {
				// type = "FOREGROUND_COLOUR";
				// subType = "RED";
		// }
		
	// }
	
	bool isReset()
	{
		return (code1==0);
	}
	
	bool isForegroundColour()
	{
		return (code1 >= 30 && code1 <= 37 );
	}
	
	Colour getForegroundColour()
	{
		Colour c;
		c.setANSI(code1);
		return c;
	}
	
	bool isBackgroundColour()
	{
		return true;
	}
	
	Colour getBackgroundcolour()
	{
		Colour c;
		return c;
	}

};

/*
ANSI is a special string container that also stores the colour of each character
which can be defined using ANSI escape codes.
It will also support some other ANSI codes.
*/

class ANSI
{
	public:
	
	Vector <ANSI_code*> vCode;
	
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
		std::cout<<"Reading ansi\n"; 
		
		unsigned char escape1 = 27;
		unsigned char escape2 = '[';
		unsigned char finalByte = 'm';

		std::string escapeCode = "";
		
		for (unsigned int i=0;i<_str.size()-1;++i)
		{
			std::cout<<(int)_str[i]<<",";
			if ((unsigned char)_str[i]==escape1 && (unsigned char)_str[i+1] == escape2)
			{
				std::cout<<"SCSI\n";
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
						
						std::cout<<"Readin escape code\n";
						ANSI_code ansiCode (escapeCode);
						
						if (ansiCode.isForegroundColour())
						{
							std::cout<<"Foreground colour change\n";
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

#endif
