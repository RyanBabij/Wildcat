#ifndef WILDCAT_DATA_WTF_HPP
#define WILDCAT_DATA_WTF_HPP

/* Wildcat:WTFManager
#include <Data/WTFManager.hpp> **/

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

*/
//A WTFNode is a chunk of data. It may be a key and value(s), and/or it may contain sub-raws.

class WTFNode
{
	private:
	WTFNode* parent;
	
	std::string id;
	Vector <std::string> vValue; //if the raw is key-values, store the values here.
	Vector <WTFNode*> vSubRaw; // all sub nodes go here to form tree structure.
	
	
		// multimap of subraws, keyed by ID
	//std::multimap <std::string, Raw*> mmSubRaw;
		// multimap of key, multi value pairs (null-string values mean key is a tag)
	//std::multimap <std::string, std::string> mmValue; 
	
	public:
	WTFNode()
	{
		parent=0;
		id="";
	}
	
	~WTFNode()
	{
		vValue.clear();
		vSubRaw.clear();
	}
	
	//parse the top layer and then recurse down to sub-raws.
	// return false if the data is invalid.
	// the raw data should be stripped by this point by the rawmanager.
	bool parse(std::string input, WTFNode* _parent=0)
	{
		//std::cout<<"Input: "<<input<<"\n";
		
		parent = _parent;
		
		Vector <std::string> vStrRoot;
		std::string strRoot="";
		
		bool readingID = true;
		std::string currentID = ""; // read in the id of the tag which should always follow an opening bracket
		std::string currentSub = ""; // the current value of the key-values
		
		bool readValues = true; // read in the values for the key.
		
		int currentLevel = 1;
		bool quotes = false; // ignore anything in quotes (only applies inside brackets)
		bool closedBracket = false; // ignore anything between a closing and opening bracket
		for (unsigned int i=1;i<input.size();++i)
		{
			if (input[i] == '\"')
			{
				quotes=!quotes;
			}
			
			if ( quotes == false && input[i] == '[' )
			{
				++currentLevel;
				closedBracket=false;
			}
			
			
			// build the level ID or obtain values
			if (readingID)
			{
				if (currentLevel==1)
				{
					if (input[i]==':') // this is a key-values or sub-raw.
					{
						//colon marks end of id
						readingID=false;
					}
					else if (input[i]==']') // this is a tag we can stop here.
					{
						readingID=false;
						readValues=false;
						//return true;
					}
					// else if (input[i]=='[') // this is a subraw
					// {
						// readingID=false;
						// readValues=false;
					// }
					else // keep building id
					{
						currentID+=(char)input[i];
					}
				}
				else
				{
					if (currentID.size()==0)
					{
						std::cout<<"Error: No raw ID. Aborting.\n";
						return false;
					}
					// reading subraw
					readingID=false;
					readValues=false;
				}
			}
			else if (readValues) // we are not reading the id, so we are reading either key-values or sub-raw
			{
				if ( currentLevel==1 )
				{
					if (input[i] == ',')
					{ // delimit multiple values
						vValue.push(currentSub); // note that null values may be pushed
						currentSub="";
					}
					else if (input[i] == ']')
					{ // end of values assignment
						vValue.push(currentSub); // note that null values may be pushed
						currentSub="";
					}
					else if (input[i] == '[')
					{
						//assume there are no values and we are progressing to subraws
						currentSub="";
					}
					else if (input[i] == ':') // ignore colons
					{
					}
					else
					{ // build current values
						currentSub+=(char)input[i];
					}
				}
			}
			
			// build the sub-levels which will be passed to subraws.
			if (currentLevel > 1 && ( closedBracket==false || input[i]==']'))
			{
				if ( quotes == true || ( input[i] != ' ' && input[i] != '\n' && input[i] != '\r' && input[i] != '\t') )
				{
					strRoot+=input[i];
				}					
			}
			
			if (quotes == false && input[i] == ']')
			{
				--currentLevel;
				if (currentLevel<0)
				{
					std::cout<<"Error: Too many closing brackets. Aborting.\n";
					return false;
				}
				
				closedBracket=true;
				
				// push the raw to the vector if we are back to level 0
				if ( currentLevel==1 && strRoot.size() > 0 )
				{
					vStrRoot.push(strRoot);
					strRoot="";
				}
			}
			
		}
		
		if (currentLevel!=0)
		{
			std::cout<<"Error: Bad bracket count. Aborting.\n";
			return false;
		}
		
		id = currentID;
		
		std::cout<<"Node built: "<<toString(true)<<"\n";


		if ( vStrRoot.size()==0 )
		{
			//std::cout<<"This leaf is finished.\n";
		}
		else
		{
			//std::cout<<"Raw parsed the following:\n";
			// for (int i=0;i<vStrRoot.size();++i)
			// {
				// //std::cout<<vStrRoot(i)<<"\n***\n";
			// }
			//std::cout<<"Parsing sub raws:\n";
			for (int i=0;i<vStrRoot.size();++i)
			{
				WTFNode* raw = new WTFNode();
				if (raw->parse(vStrRoot(i),this))
				{
					vSubRaw.push(raw);
				}
				else
				{
					std::cout<<"Error parsing a subraw. Aborting.\n";
					return false;
				}
			}
		}
		
		return true;
	}
	
	std::string toString(bool fullID=false)
	{
		std::string ret;
		
		if (fullID)
		{
			ret=getFullID();
		}
		else
		{
			ret=id;
		}
		
		if (vValue.size() > 0)
		{
			ret+=": ";
			
			for (int i=0;i<vValue.size();++i)
			{
				ret+=vValue(i);
				if ( i<vValue.size()-1 )
				{
					ret+=", ";
				}
			}
		}
		return ret;
	}
	
	std::string getID()
	{
		return id;
	}
	
	// returns the full namespace path down the tree.
	std::string getFullID()
	{
		if (parent==0)
		{
			return id;
		}
		return parent->getFullID() + "." + id;
	}
	
	// querying:
	// You can either get the values vector, or you can get all nodes on a layer.
	// I think multimap does hashmapping for performance but it's not too important either way
	// since raws are only loaded on startup.
	
	
	// Return values for this key
	Vector <std::string>* getValues(Vector <std::string>* vPath)
	{
		// return the value(s) of the request.
		if (vPath->size() == 0) // this is the matching tag
		{
			if (vValue.size()==0)
			{ return 0; }
			Vector <std::string> * vRet = vValue.copy();
			return vRet;
		}
		for (int i=0;i<vSubRaw.size();++i)
		{
			if ( (*vPath)(0) == vSubRaw(i)->getID() )
			{
				vPath->eraseSlot(0);
				return vSubRaw(i)->getValues(vPath);
			}
		}
		return 0;
	}
	// return all nodes on this namespace
	void getAll(std::string request)
	{
	}
	bool hasTag(Vector <std::string>* vPath)
	{
		if (vPath->size() == 0) // this is the matching tag
		{
			return true;
		}
		for (int i=0;i<vSubRaw.size();++i)
		{
			if ( (*vPath)(0) == vSubRaw(i)->getID() )
			{
				vPath->eraseSlot(0);
				return vSubRaw(i)->hasTag(vPath);
			}
		}
		return false;
	}
};

//Container for collection of raws. Can parse in a string and convert it into a Raw tree.
// RawManager is necessary to strip the data and manage the root (level 0) nodes.
class WTFManager
{
	Vector <WTFNode*> vRoot; // vector of all root (level 0) nodes.
	
	public:
	WTFManager()
	{
	}
	~WTFManager()
	{
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
			std::cout<<"Error: Bad square bracket count: "<<currentLevel<<".\n";
			return false;
		}
		else
		{
			//std::cout<<"Good square brackets count.\n";
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
		//std::cout<<"RAW recieved\n";
		
		// step 1: Remove all non-relevant data. That is: everything outside of square brackets.
		// and any whitespace outside of quotation marks.
		
		//std::cout<<"Verifying raw\n";
		
		if (verify(input) == false )
		{
			std::cout<<"Verification failed, parsing aborted.\n";
		}
		
		//std::cout<<"Stripping raw\n";
		//std::string input2 = strip(input);
		
		//std::cout<<"Stripped raw:\n"<<input2<<"\n";
		
		//std::cout<<"Begin building tree.\n";
		
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
		//std::cout<<"   Root nodes:\n";
		for (int i=0;i<vStrRoot.size();++i)
		{
			//std::cout<<vStrRoot(i)<<"\n*\n";
		}
		//std::cout<<"\n\n";
		
		for (int i=0;i<vStrRoot.size();++i)
		{
//			std::cout<<"Root:\n"<<vStrRoot(i)<<"\n\n";
			
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
			
			//std::vector strRootID = "";
			
			// read the root id
			
			// std::string strCurrentRoot = vStrRoot(i);
			// for (int i2=1;i2<strCurrentRoot.size();++i2)
			// {
				// if (strCurrentRoot[i] == ']') // this ID is a tag.
				// {
				// }
				// else if (strCurrentRoot[i] == ':')
				// {
				// }
			// }
			
			
		}
		std::cout<<"END\n";
		
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
	// return true if the path through the tree exists.
	bool hasTag (std::string _query)
	{
		if ( _query.size()==0 )
		{ return false; }
	
		//break query into delimiters.
		Vector <std::string> * vPath = DataTools::tokenize(_query,".");
		
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
	
};

#endif
