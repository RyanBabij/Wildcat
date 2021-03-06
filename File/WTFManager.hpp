#pragma once
#ifndef WILDCAT_DATA_WTF_MANAGER_HPP
#define WILDCAT_DATA_WTF_MANAGER_HPP

/* Wildcat: WTFManager
	#include <File/WTFManager.hpp>

	Wildcat Template File (WTF) manager. Class used for managing game data stored in a tree format known as raws.

*/

/**
A Wildcat Template File (WTF) is a collection of keys and values which can be nested, and are generally used to define
objects/templates in games. It is similar but not identical to the "raw" format of Dwarf Fortress.

WTF specification:

WTFs use a combination of [square brackets] and "quotation marks".

* Text outside of square brackets, for example at the beginning of a file, is ignored. This means files can be commented
freely as long as the comments don't contain square brackets.
* Text after a closing bracket and before an opening bracket is also ignored, allowing comments at the end of lines in
addition to formatting.
* Text immediately following an opening bracket is the ID of the template. A template cannot have a null ID.
* The template may then be one of three types:
	* If the ID is followed by a closing bracket, the template is a tag, aka a boolean. For example [TAG]
	* If the ID is followed by a colon, it is a key, value(s) pair, for example: [NAME:"Garo","Garos"], commas delimit
		multiple values. Values can be strings or numbers.
	* If the ID is followed by an opening bracket, it contains sub-templates. For example:
	[CREATURE
		[DEER
			[MAMMAL]
			[DESCRIPTION:"This is a deer."]
			[AGE:11]
		]
	]
	This allow for WTFs to form complex tree structures which can later be accessed using a namespace system such as:
	CREATURE.DEER.AGE to access key-values. Note that objects must have unique namespaces otherwise they could merge and
	cause unexpected results.
* All whitespace outside of quotation marks is ignored.
* WTFs with an unequal number of square brackets are invalid and will not load.
* WTFs with duplicate namespaces will merge if possible.

Future possibilities:
	* Merging raws with the same namespace (will require a merging priority system, and ability to delete/modify tags.
		A simple timestamp system might be the best option.)
	* Pointer handling - Pointers can be saved by giving objects unique IDs.
	* Binary data - Probably best handled with the old "chunk" system.
	
	This tree data could be read normally but collapsed into a hashmap for much faster lookups.

*/

#include "WTFManager_Node.hpp" // WTFNode

#include <Container/Vector/Vector.hpp>
#include <Data/DataTools.hpp>

// RawManager is necessary to parse the data and manage the root (level 0) nodes.
class WTFManager
{
	private:
	Vector <WTFNode*> vRoot; // vector of all root (level 0) nodes.
	
	public:
	WTFManager() { }
	
	~WTFManager()
	{
		// clear the nodes vector.
		for (int i=0;i<vRoot.size();++i)
		{
			delete vRoot(i);
		}
	}
	
	// do a quick bracket count just to be sure it's not bad data
	bool verify(const std::string input)
	{
		int currentLevel=0;
		for (unsigned int i=0;i<input.size();++i)
		{
			if ( input[i] == '[' )
			{
				++currentLevel;
			}
			else if (input[i] == ']')
			{
				--currentLevel;
				if ( currentLevel < 0 )
				{
					break;
				}
			}
		}
		
		if ( currentLevel != 0 )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	
	std::string strip(std::string input)
	{
		std::string strip = "";
		
		int currentLevel = 0;
		bool quotes = false; // ignore anything in quotes (only applies inside brackets)
		bool closedBracket = false; // ignore anything between a closing and opening bracket
		for (unsigned int i=0;i<input.size();++i)
		{
			if (currentLevel>0 && closedBracket==false && input[i] == '\"'			)
			{
				quotes=!quotes;
			}
			
			if ( quotes == false && input[i] == '[' )
			{
				++currentLevel;
				closedBracket=false;
			}

			if (currentLevel > 0 && closedBracket==false)
			{
				if ( quotes == true || ( input[i] != ' ' && input[i] != '\n' && input[i] != '\r' && input[i] != '\t') )
				{
					strip+=input[i];
				}					
			}
			
			if (quotes == false && input[i] == ']')
			{
				--currentLevel;
				closedBracket=true;
			}
			
		}
		return strip;
	}
	
	void clear()
	{
		for (int i=0;i<vRoot.size();++i)
		{
			delete vRoot(i);
		}
	}
	
	bool parse(const std::string input)
	{
		// step 1: Remove all non-relevant data. That is: everything outside of square brackets.
		// and any whitespace outside of quotation marks.
		
		if (verify(input) == false )
		{
			std::cout<<"Verification failed, parsing aborted.\n";
		}

		// read all root raws and parse them individually.
		Vector <std::string> vStrRoot;
		
		std::string strRoot = "";
		
		int currentLevel = 0;
		bool quotes = false; // ignore anything in quotes (only applies inside brackets)
		bool closedBracket = false; // ignore anything between a closing and opening bracket
		for (unsigned int i=0;i<input.size();++i)
		{
			if (currentLevel>0 && closedBracket==false && input[i] == '\"'			)
			{
				quotes=!quotes;
			}
			
			if ( quotes == false && input[i] == '[' )
			{
				++currentLevel;
				closedBracket=false;
			}
			
			if (currentLevel > 0 && ( closedBracket==false || input[i]==']'))
			{
				if ( quotes == true || ( input[i] != ' ' && input[i] != '\n' && input[i] != '\r' && input[i] != '\t') )
				{
					strRoot+=input[i];
				}					
			}
			
			if (quotes == false && input[i] == ']')
			{
				--currentLevel;
				closedBracket=true;
				
				// push the raw to the vector if we are back to level 0
				if ( currentLevel==0 && strRoot.size() > 0 )
				{
					vStrRoot.push(strRoot);
					strRoot="";
				}
			}
		}
		
		// we now have all the root nodes at this point. We take the data we need and then make the subnodes.
		for (int i=0;i<vStrRoot.size();++i)
		{
			// create a Raw root node and let the node parse the string
			WTFNode * rRoot = new WTFNode;
			if (rRoot->parse(vStrRoot(i)))
			{
				vRoot.push(rRoot);
			}
			else
			{
				delete rRoot;
				// abort
				clear();
				std::cout<<"ERROR: Parsing failed\n";
				return false;
			}
		}
		return true;
	}
	
	Vector <std::string> * getValues(std::string _query)
	{
		if ( _query.size()==0 )
		{ return 0; }
	
		//break query into delimiters.
		Vector <std::string> * vPath = DataTools::tokenize(_query,".");
		
		for (int i=0;i<vRoot.size();++i)
		{
			if ( vRoot(i)->getID() == (*vPath)(0) )
			{
				vPath->eraseSlot(0);
				Vector <std::string>* vRet = vRoot(i)->getValues(vPath);
				delete vPath;
				return vRet;
			}
		}
		
		delete vPath;
		return 0;
	}
	
	Vector <std::string> * delimitPath(std::string _query)
	{
		return DataTools::tokenize(_query,".");
	}

	// return the value attached to this path. If there are multiple it will return the first.
	// if there are none it will return empty string.
	std::string getValue(std::string _query)
	{
		if ( _query.size()==0 )
		{ return 0; }
	
		//break query into delimiters.
		Vector <std::string> * vPath = delimitPath(_query);
		
		for (int i=0;i<vRoot.size();++i)
		{
			if ( vRoot(i)->getID() == (*vPath)(0) )
			{
				vPath->eraseSlot(0);
				std::string strRet = vRoot(i)->getValue(vPath);
				delete vPath;
				return strRet;
			}
		}
		
		delete vPath;
		// couldn't find path, return empty string.
		return "";
	}
	
	
	// return true if the path through the tree exists.
	bool hasTag (std::string _query)
	{
		if ( _query.size()==0 )
		{ return false; }
	
		//break query into delimiters.
		Vector <std::string> * vPath = delimitPath(_query);
		
		for (int i=0;i<vRoot.size();++i)
		{
			if ( vRoot(i)->getID() == (*vPath)(0) )
			{
				vPath->eraseSlot(0);
				
				if (vRoot(i)->hasTag(vPath))
				{
					delete vPath;
					return true;
				}
			}
		}
		delete vPath;
		return false;
	}
	
	// print all raws, using full or relative path, with or without indenting
	std::string getAll(bool fullPath=true, bool indent=true)
	{
		std::string strAll = "";
		for (int i=0;i<vRoot.size();++i)
		{
			strAll+=vRoot(i)->getAll(fullPath,indent?0:-1)+"\n";
		}
		return strAll;
	}
	
	// return all subraws from the given namespace. Blank returns everything on layer 0.
	Vector <WTFNode*> * getAllSub(std::string _query)
	{
		//Vector <WTFNode*> vSubs = new Vector <WTFNode*> ();

		// Top-level layer
		if (_query.size() == 0)
		{
			// presumably the user wants all top-level nodes
			
			if (vRoot.size() == 0)
			{
				return 0;
			}
			else
			{
				return &vRoot;
			}
		}
		
		//Namespaced layer
		//break query into delimiters.
		Vector <std::string> * vPath = delimitPath(_query);
		
		for (int i=0;i<vRoot.size();++i)
		{
			// we found matching root node, search into here.
			if ( vRoot(i)->getID() == (*vPath)(0) )
			{
				vPath->eraseSlot(0);
				return vRoot(i)->getAll(vPath);
			}
		}
		
		delete vPath;
		return 0;

	}
	
	// return random node on the given namespace.
	// For example if we have COLOUR.RED, COLOUR.GREEN and COLOUR.BLUE
	// and call getRandom("COLOUR"), we should get either the RED, GREEN, or BLUE nodes.
	// namespaces should be divided using colon, eg:
	// COLOUR:RED
	WTFNode* getRandom(std::string _query, RandomInterface& rng)
	{
		
		// Top-level random
		if (_query.size() == 0)
		{
			// presumably the user wants a random top-level node
			
			if (vRoot.size() == 0)
			{
				return 0;
			}
			else
			{
				return vRoot(rng.rand32(vRoot.size()-1));
			}
		}
		
		//Namespaced random
		//break query into delimiters.
		Vector <std::string> * vPath = delimitPath(_query);
		
		for (int i=0;i<vRoot.size();++i)
		{
			// we found matching root node, search into here.
			if ( vRoot(i)->getID() == (*vPath)(0) )
			{
				vPath->eraseSlot(0);
				return vRoot(i)->getRandom(vPath,rng);
			}
		}
		
		delete vPath;
		return 0;
	}
	
};

#endif
