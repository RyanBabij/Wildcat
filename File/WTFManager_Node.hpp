#pragma once
#ifndef WILDCAT_DATA_WTF_MANAGER_NODE_HPP
#define WILDCAT_DATA_WTF_MANAGER_NODE_HPP

/* Wildcat: WTFManager_Node
	#include <File/WTFManager_Node.hpp>

	Node class for WTFManager. Should be nested inside WTFManager and nowhere else.
	
	A WTFNode is a chunk of data. It may be a key and value(s), and/or it may contain sub-raws.

*/

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

#endif
