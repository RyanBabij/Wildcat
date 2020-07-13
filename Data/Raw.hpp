#ifndef WILDCAT_DATA_RAW_HPP
#define WILDCAT_DATA_RAW_HPP

/* Wildcat:Raw
#include <Data/Raw.hpp> **/

/**

A raw is a collection of keys and values which can be nested, and are generally used to define objects in games.



New version of SaveFileManager. I think the SaveFileManager was not a very good system because it didn't really
support nesting. With this system all data can be nested as much as you want.

All chunks of data and datatypes start with an open square bracket, then contain an ID for the data followed by a colon.

After the colon may follow the data, or sub-chunks of data. This allows for the data to be represented as a kind of
tree.

Example 1 - Simple data:

[NAME:"Garo"]
or
[Name:Garo]
The quotation marks escape all data until a closing quotation mark is found (note that an escape character for quotation
marks will probably need to be added later).

Example 2 - Multiple data values:

[NAME:"Garo","Bob"]

In this case the "NAME" field has 2 values. This means that data must be iterated through.


Example 3 - Nested data:

[CREATURE:WOLF
	[NAME:"Wolf","Wolves"]
	[AGE:7]
	[ANIMAL]
	[BODYPARTS:
		[HEAD:1]
		[LEGS:4]
		[TAIL:1]
	]
]

In this case "CREATURE" doesn't return data, but another subset of fields.

Also note that a tag may be defined without a value.


Future considerations:

Pointer handling - Pointers can be saved by giving objects unique IDs.
Binary data - Probably best handled with the old system.

*/

#include <File/FileManager.hpp>

#include <map>

//A raw is a chunk of data. It may be a key and value(s), and/or it may contain sub-raws.

class Raw
{
	private:
		// multimap of subraws, keyed by ID
	std::multimap <std::string, Raw*> mmSubRaw;
		// multimap of key, multi value pairs (null-string values mean key is a tag)
	std::multimap <std::string, std::string> mmValue; 
	
	public:
	Raw()
	{
	}
	
	~Raw()
	{
	}
	
	// add key and value to the multimap
	void add(std::string key, std::string value)
	{
	}
	void add(Raw* subRaw)
	{
	}
	
	Vector <std::string>* getValues (std::string key)
	{
		return 0;
	}
	
	
	Raw* getSubRaw(std::string key)
	{
		return 0;
	}
	
	
	
	
};

#endif
