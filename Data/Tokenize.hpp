#pragma once
#ifndef WILDCAT_DATA_TOKENIZE_HPP
#define WILDCAT_DATA_TOKENIZE_HPP

/*
	#include <Data/Tokenize.hpp>

	Tokenize::tokenize(data,'\n');


	REMEMBER THAT BOTH '\n' AND '\r' ARE NEWLINES. YOU NEED TO FILTER FOR BOTH OR YOU WILL GET PHANTOM NEWLINES.

	THIS LIB SHOULD BE EXPANDED FOR ALL DATA FILTERING.
*/

#include <Container/Vector/Vector.hpp>

class Tokenize
{
	public:
	Tokenize()
	{}

		// TOKENIZE USING A SINGLE TOKEN.
	static Vector <std::string> * tokenize ( std::string _data, char _delimiter)
	{
		Vector <std::string> * vTokenized = new  Vector <std::string>;

		std::string currentLine="";
		for ( unsigned int i=0;i<_data.size();++i)
		{
			if ( _data[i]==_delimiter )
			{
				if ( currentLine!="" )
				{
					vTokenized->push(currentLine);
				}
				currentLine="";
			}
			else
			{
				currentLine+=_data[i];
			}
		}
		
		// Push last token
		if ( currentLine!="" )
		{
			vTokenized->push(currentLine);
		}

		return vTokenized;
	}

		// TOKENIZE USING MULTIPLE TOKENS.
	static Vector <std::string> * tokenize ( std::string _data, Vector <char>* _vDelimiter)
	{
		if (_vDelimiter==0) { return 0; }

		Vector <std::string> * vTokenized = new  Vector <std::string>;

		std::string currentLine="";
		for ( unsigned int i=0;i<_data.size();++i)
		{
			bool delimFound=false;
			for ( int i2=0;i2<_vDelimiter->size();++i2)
			{
				if ( _data[i] == (*_vDelimiter)(i2) )
				{
					delimFound=true;
					break;
				}
			}

			if ( delimFound==true )
			{
				if ( currentLine.size() > 0 )
				{
					vTokenized->push(currentLine);
				}
				currentLine="";
			}
			else
			{
				currentLine+=_data[i];
			}
		}
      // Push final token
      if ( currentLine.size()>0)
      {
         vTokenized->push(currentLine);
      }
		return vTokenized;
	}
   
   static Vector <std::string> * tokenize ( std::string _data, std::string _strDelimiter)
   {
		Vector <std::string> * vTokenized = new  Vector <std::string>;

		std::string currentLine="";
		for ( unsigned int i=0;i<_data.size();++i)
		{
			bool delimFound=false;
			for ( unsigned int i2=0;i2<_strDelimiter.size();++i2)
			{
				if ( _data[i] == _strDelimiter[i2] )
				{
					delimFound=true;
					break;
				}
			}

			if ( delimFound==true)
			{
				if ( currentLine.size() > 0 )
				{
					vTokenized->push(currentLine);
				}
				currentLine="";
			}
			else
			{
				currentLine+=_data[i];
			}
		}
      // Push final token
      if ( currentLine.size()>0)
      {
         vTokenized->push(currentLine);
      }
      
		return vTokenized;
   }

};

#endif
