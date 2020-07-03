#ifndef WILDCAT_SAVEFILEMANAGER_HPP
#define WILDCAT_SAVEFILEMANAGER_HPP

#include <Data/DataTools.hpp>
#include <File/FileManager.hpp>

/* Wildcat:SaveFileManager
#include <File/SaveFileManager.hpp> **/

/**
Class to help turning game state into a file, and loading it back into a game. Uses FileManager for most File IO.

Works by loading the save file as a string, then performing operations to extract useful information from the string.

Specification: Anything outside of square brackets is ignored. Data must be enclosed in square brackets.
The format is:
[TAG:DATA]

Where TAG is a string identifier for the data, and DATA is the data associated with the tag.
Data can be nested, for example:

[HOUSE1:[HOUSENAME:My house][ROOM1:Bedroom[ITEM1:Bed][ITEM2:Bookshelf]][ROOM2:Lounge]]


It is important for chunks of data to have a unique ID so that they can be linked together. For example:

[PERSON1:[NAME:Ryan][Friends:PERSON2,PERSON3]]

The unique IDs allow for pointers to be saved.

Objects that can be saved should inherit a SaveFileInterface. They should return the following:

* A generic tag (must be unique among all generic tags)
* A unique ID (must be unique among all objects of that class)
* The data.

[TAG:ID:DATA]


The only invalid characters in these save files are square brackets (and probably some escape sequences).
Anything else may be typed into the save files without causing problems. This should allow easy commenting and
documentation.


Pointer management:

Okay, so saving and loading basic raw data is fine. You convert to string, save to file. Load from file, convert from string.

The real issue is how to store links between objects.

The most obvious solution to me is to give every single saveable object in the world a unique ID.

Another important thing should be the ability to group data into "chunks". For example if you have an array of 1000
small objects, you might want to have it all grouped into a single chunk of data without having 1000 sets of square
brackets and tag IDs.


Okay new plan. There are two types of savedata. There are individual pieces like objects. Then there are chunks which
are large collections of data. Breaking data into chunks will make searching faster if they have their own syntax.

*/


/* Objects that can load and save should use this interface. */

#include <Data/UID.cpp>

UID <long long unsigned int> SaveFileInterface_UID;

class SaveFileInterface
{
	private:
	// uid is used to store pointers between objects.
	long long unsigned int uid;

	public:

	SaveFileInterface()
	{
		uid = SaveFileInterface_UID.get();
	}
	~SaveFileInterface() {}

	long long unsigned int getUID()
	{
		return uid;
	}


	virtual std::string getSaveFileName()
	{
		return "";
	}

	virtual int getClassID()
	{
		return -1;
	}

	virtual std::string getSaveData()
	{
		return "";
	}

	virtual void save()
	{
	}

};

// Basically a container designed for saving large collections of data, for example array contents.
class SaveChunk
{
	public:

	Vector <std::string> vData;
	std::string name;
	char delimiter;

	SaveChunk(std::string _name)
	{
		name=_name;
		delimiter=',';
	}

	std::string operator() (const int _i)
	{
		if ( vData.isSafe(_i) ) { return vData(_i); }
		return "";
	}

	// Push a piece of data into the chunk.
	void add (std::string _data)
	{
		vData.push(_data);
	}

	std::string toString()
	{
		std::string ret = "";

		for (int i=0;i<vData.size();++i)
		{
			ret+=vData(i);
		}
		return ret;
	}

};

class SaveFileManager
{
	public:

	std::string data;

	SaveFileManager()
	{
		data = "";
	}

	~SaveFileManager()
	{}

	// Load the savefile into RAM.
	void loadFile (std::string _file)
	{
		//std::cout<<"LOADING FILE: "<<_file<<".\n";
		data = FileManager::getFileAsString(_file);
	}

	// ADD VARIABLE TO SAVE STRING
	template <class T>
	void addVariable(std::string _tag, T _value)
	{
		//std::cout<<"ADD VARIABLE\n";
		data+="["+_tag+":"+DataTools::toString(_value)+"]";
	}


	void addChunk (SaveChunk& _chonk)
	{
		data+="{CHONK:"+_chonk.name+":\n";

		for (int i=0;i<_chonk.vData.size();++i)
		{
			data+=_chonk.vData(i);
			data+=_chonk.delimiter;
		}
		data+="}\n";
	}
	SaveChunk* getChunk(std::string _tag)
	{
		SaveChunk * chonk = new SaveChunk(_tag);

		const std::string searchTag = "{CHONK:"+_tag+":";

		std::size_t found = data.find(searchTag);
		if (found!=std::string::npos)
		{
			found += searchTag.length()+2;

			std::string _saveData = "";
			while (found != std::string::npos && data[found] != '}')
			{
				if (data[found] == ',')
				{
					chonk->add(_saveData);
					_saveData="";
				}
				else
				{
					_saveData+=data[found];
				}

				++found;
			}
		}
		return chonk;
	}

	// RETURN DATA THAT MATCHES TAG.
	// IF CANNOT BE FOUND, RETURN EMPTY STRING.
	std::string loadVariableString (std::string _tag)
	{
		std::size_t found = data.find("["+_tag+":");
		if (found!=std::string::npos)
		{
			found += _tag.length() + 2;

			std::string _saveData = "";
			while (found != std::string::npos && data[found] != ']')
			{
				_saveData+=data[found];
				++found;
			}

			return _saveData;
		}

		std::cout<<"ERROR: DIDN'T FIND TAG: "<<_tag<<".\n";
		return "";
	}

	// Load the savefile into RAM.
	void saveToFile (std::string _path)
	{
		//std::cout<<"SAVING DATA TO: "<<_path<<".\n";

		//savePath = _path;

		FileManager::createFile(_path);
		FileManager::writeString(data, _path);

		//data = "";
		// for (int i=0;i<vSaveObjects.size();++i)
		// {
		// //data += vSaveObjects(i)->getSaveData();
		// vSaveObjects(i)->save();
		// }

		//data = FileManager::getFileAsString(_file);
	}

	// template <class T>
	// bool saveVariable(std::string _tag, T _value)
	// {
	// std::cout<<"SAVE VARIABLE\n";
	// //std::string saveData = DataTools::toString(_value);
	// std::string saveData = "["+_tag+":"+DataTools::toString(_value)+"]";
	// FileManager::writeString(saveData,savePath);
	// return false;
	// }

	// template <class T>
	// std::string toSaveData(std::string _tag, T _value)
	// {
	// //std::cout<<"SAVE VARIABLE\n";
	// //std::string saveData = DataTools::toString(_value);
	// std::string saveData = "["+_tag+":"+DataTools::toString(_value)+"]";
	// return saveData;
	// }


	// // Load the savefile into RAM.
	// void saveFile (std::string _path)
	// {
	// std::cout<<"Save file.\n";

	// savePath = _path;

	// FileManager::createFile(_path);

	// //data = "";
	// for (int i=0;i<vSaveObjects.size();++i)
	// {
	// //data += vSaveObjects(i)->getSaveData();
	// vSaveObjects(i)->save();
	// }

	// //data = FileManager::getFileAsString(_file);

	// }
};
#endif
