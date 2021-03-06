#pragma once
#ifndef WILDCAT_STRING_STRING_CPP
#define WILDCAT_STRING_STRING_CPP

/* 
	#include <String/String.hpp>
#include <Render/Renderer.hpp> Created: 0223411322. Updated: 0223411322. Component of GaroSoft WildCat game engine.
	Wrapper and extender for STL strings.
*/

#include <iostream>
#include <string>

#include <Container/Vector/Vector.hpp>

class String
{
	private:
	
	public:
	std::string data;
	
	
	
	String ()
	{ data = ""; }
	String (std::string _data)
	{ data = _data; }
	
	void operator = (std::string _data)
	{
		data = _data;
	}
	
	String(const char* _str)
	{
		data = _str;
	}
	
	bool startsWith(std::string _str)
	{
		if (_str.length() <= data.length() )
		{
			for (unsigned int i=0;i<_str.length();++i)
			{
				if(data[i] != _str[i])
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}
	
		// WILL RETURN A STRING BETWEEN AN OPENING AND CLOSING TAG ENCLOSED BETWEEN ANGLE BRACKETS, THE CLOSING TAG MUST USE /.
		// EXAMPLE: <TAG>data</TAG>
		// ONLY THE TAG NAME NEEDS TO BE PASSED, IN THIS CASE "TAG".
		// CURRENTLY THE FUNCTION IS VERY BASIC AND ASSUMES THERE'S ONLY ONE OF EACH TAG. DOESN'T ACCOUNT FOR NESTING ETC.
		// ALL WE NEED TO DO IS FIND STARTING INDEX, ENDING INDEX, AND MAKE A SUBSTRING.
	String getTagContents(std::string _tagName)
	{
		String tagContent = "";
			// TAGS MUST CONTAIN AT LEAST 1 CHARACTER
		if (_tagName.length()>0)
		{
			std::string closingTag = "</"+_tagName+">";
			_tagName = "<"+_tagName+">";
			
			
			int index = search(_tagName);
			
			if ( index != -1 )
			{
				std::cout<<"Tag found at: "<<index<<".\n";
				index+=_tagName.size();
				
				int index2 = search(closingTag);
				
				if (index2 != -1)
				{
					std::cout<<"Ending tag found at: "<<index2<<".\n";
					
					if (index<index2)
					{
						int stringLength = index2-index;
						
						tagContent = data.substr(index,stringLength);
						
						
					}
					else
					{
						std::cout<<"Error: Tags are in wrong order.\n";
					}
				}
				else
				{
					std::cout<<"Ending tag not found.\n";
				}
			}
			else
			{
				std::cout<<"Tag not found.\n";
			}
		}
		
		return tagContent;
	}

	void operator += (std::string _str)
	{ data += _str; }
	
	void operator += (unsigned char _char)
	{ data += _char; }
	
	std::ostream& operator<< (std::ostream& os)
	{
		os << 5;
		return os;
	}
	
		/* cout compatibility. */
	friend std::ostream &operator<< (std::ostream &output, const String &_str)
	{ 
		output <<_str.data;
		return output;
	}

	int length()
	{
		return data.length();
	}
	int size() { return length(); }

	std::string toString()
	{
		return data;
	}
	
		/* Return true if _strSearch is located anywhere in this string. */
	bool contains (std::string _strSearch)
	{
		return (data.find(_strSearch)!=std::string::npos);
	}
	
	void addToFront(std::string wordz)
	{
		data = wordz+data;
	}
	void addToEnd(std::string wordz)
	{
		data += wordz;
	}
	
	Vector <std::string> ripTokens (std::string _strBegin, std::string _strEnd)
	{
		std::cout<<"RIPPING TOKES\n";
		Vector <std::string> tokes;
		
		for ( unsigned int i=0; i<data.size(); ++i)
		{
			if (matchesAtIndex (i, _strBegin))
			{
				i+=_strBegin.size();
				std::cout<<"Begin match.\n";
				
				std::string toke = "";
				
				for (;i < data.size(); ++i)
				{
					if (matchesAtIndex (i, _strEnd))
					{
						break;
					}
					toke+=data[i];
				}
				std::cout<<"Toke: "<<toke<<".\n";
				tokes.push(toke);
			}

			
		}
		
		return tokes;
	}
	
	bool matchesAtIndex (int _index, std::string _strSearch)
	{
		if ( _strSearch.size()+_index < data.size() )
		{
			for ( unsigned int i2=0;i2<_strSearch.size();++i2)
			{
				if ( data[_index+i2] != _strSearch[i2] )
				{
					return false;
				}
			}
			return true;
		}
		return false;
	}
	
		//0271435783 - Updated to return -1 instead of 0 if string can't be found. Obviously will cause problems.
	int search (std::string _strSearch)
	{
		for ( unsigned int i=0;i<data.size();++i)
		{
			if (data[i] == _strSearch[0])
			{
				//std::cout<<".";
				// Make sure we have room to search.
				if ( _strSearch.size() < (data.size()-i) )
				{
					//std::cout<<"\n";
					bool fullMatch = true;
					for ( unsigned int i2=1;i2<_strSearch.size();++i2)
					{
						//std::cout<<data[i+i2]<<"=="<<_strSearch[i2]<<"?\n";
					
						if ( data[i+i2] != _strSearch[i2] )
						{
							//std::cout<<"No match.\n";
							fullMatch=false;
							break;
						}
					}
					if (fullMatch==true)
					{
						//std::cout<<"Full match.\n";
						return i;
					}
				}
				else
				{
					//std::cout<<"No match.\n";
				}

			}
		}
		return -1;
	
	}

	/* Find one string and replace it with another string. Exact match only. */
	// RETURN NUMBER OF REPLACEMENTS.
	int replaceAll (std::string find, std::string replace)
	{
		int nReplacements = 0;
		if(data.length() > 0 && find.length() > 0)
		{
			for (unsigned int i=0;i<data.length();++i)
			{
				//std::cout<<"Checking: "<<data[i]<<".\n";
				/* Find first char. */
				if(data[i] == find[0])
				{
					//std::cout<<"First char match: "<<data[i]<<"->"<<find[0]<<". Checking strings.\n";
					
					std::string token = "";
					//token+=data[i];
					
					/* Make sure search token can fit. */
					//int charsLeft = data.length() - i;
					//std::cout<<"charsleft: "<<charsLeft<<".\n";
					
					if( data.length() - i >= find.length() )
					{
						//std::cout<<"Search token can fit.\n";
						unsigned int i3=0;
						bool matchFound = false;
						for (unsigned int i2=i;i3<find.length();++i2)
						{
							if(find[i3] == data[i2])
							{
								//std::cout<<"  match: "<<find[i3]<<"->"<<data[i2]<<".\n";
								matchFound=true;
							}
							else
							{
								//std::cout<<"  no match: "<<find[i3]<<"->"<<data[i2]<<".\n";
								matchFound=false;
								break;
							}
							++i3;
						}
						
						if(matchFound==true)
						{
							nReplacements+=1;
							//std::cout<<"Full match.\n";
							data.replace(i,find.length(),replace);
							i+=replace.length()-1;
							//std::cout<<"Result so far: "<<data<<".\n";
						}
						else
						{
							//std::cout<<"No match.\n";
						}
						
					}
					else
					{
						//std::cout<<"Search token cannot fit.\n";
					}
					
					//for (int i2=i;i2<data.length();++i2)
					//{
					//	token+=data[i2];
					//}
					
					
					
					
				}
			}
		}
	
		return nReplacements;
	}
	
};


#endif
