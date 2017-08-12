#pragma once

/* Data/Serialiser.hpp
Allows objects to be more easily saved into data, and retrieved. Currently works by loading the provided data into RAM, and then saving it to file when asked.

2 vectors are used. One vector is a vector of char arrays. These are the bytes to be written to file. The second vector keeps track of the size of the char arrays.

*/

#include <fstream>
#include <iostream>

#include <Container/Vector/Vector.hpp>

#include <Data/DataTools.hpp>

class Serialiser
{
	public:
	
	Vector <char*> vData;
	Vector <int> vDataSize;
	
	char* fileReadData;
	int fileReadSize;
	int fileReadIndex;
	
	Serialiser()
	{ fileReadData=0; fileReadSize=0; fileReadIndex=0;
	}
	
	/* Push one piece of data. This can be something like an int, double, char, etc. */
	// template <class T>
	// void addData(T _data)
	// {
		// //std::cout<<"Sizeof: "<<sizeof(T)<<".\n";
	// //	std::cout<<"Serialising...\n";
		// std::ofstream ofs("serialisation", std::ios::binary);
// //std::cout<<"File ready...\n";
		// ofs.write((const char*)&_data, sizeof(T));
		// //std::cout<<"Saved...\n";
	// }
	template <class T>
	void pushData ( T* _data )
	{
		const int dataTypeSize = sizeof(T);
		//char* charArrayData = new char [dataTypeSize];
		//char* charArrayData = (char*) _data;
		
		//charArrayData = (char*)&_data;
		//charArrayData = (char*)_data;
		
		//vData.push(charArrayData);
		vData.push(DataTools::toCharArray(_data));
		
		//vData.push(charArrayData);
		vDataSize.push(dataTypeSize);
	}
	template <class T>
	void getData (T* _data)
	{
		*_data = DataTools::valFromCharArray <T> (&fileReadData[fileReadIndex]);
		fileReadIndex+=sizeof(T);
	}
	
	bool isNewFile()
	{
		if (fileReadSize==0) { return true; }
		return false;
	}
	
	void pushString(std::string str)
	{
		/* Doesn't include null terminator. */
		//const char* text = str.c_str();

		//vData.push(text2);
		
		char* text = new char [str.size()];
		
		for (unsigned int i=0;i<str.size();++i)
		{
			text[i]=str[i];
			//vData.push(str[i]);
		}
		vData.push(text);
		vDataSize.push(str.size());
	
	}
	/* Loads all the data into a string, stopping at, and not including, the terminator. The data index is advanced to after the terminator. */
	std::string getString(const char terminator)
	{
		std::string dataText="";
		
		while (fileReadIndex < fileReadSize && fileReadData[fileReadIndex] != terminator)
		{
			dataText+=fileReadData[fileReadIndex];
			++fileReadIndex;
		}
		/* Move the index past the terminator. */
		++fileReadIndex;
		
		return dataText;
		
	}
	bool canAccess (const int _index)
	{
		if(fileReadSize>_index)
		{
			return true;
		}
		return false;
	}
	/* Used to check if a given string is at this part of the file. */
	bool checkString (const std::string str)
	{
		for(unsigned int i=0;i<str.size();++i)
		{
			/* If we've run off the file, or the data doesn't match this part of the string, then the check returns false. */
			if(canAccess(fileReadIndex) == false || fileReadData[fileReadIndex] != str[i])
			{ return false; }
			++fileReadIndex;
		}
		return true;
	}
	
	char nextChar ()
	{
		if(canAccess(fileReadIndex))
		/* Return data, and then increment file read index. */
		{ return fileReadData[fileReadIndex++]; }
		return 0;
	}

	void saveToFile(const std::string fileName)
	{
		std::ofstream ofs(fileName.c_str(), std::ios::binary);
		
		for (int i=0;i<vData.size();++i)
		{
			//std::cout<<"Data size: "<<vDataSize(i)<<".\n";
			ofs.write(vData(i), vDataSize(i));
		}
		
		//ofs.write((const char*)&_data, sizeof(T));
	}
	
	
	/* Get the size of the data to be serialized. */
	int getBlockSize()
	{
		int totalDataSizeBytes=0;
		for (int i=0;i<vData.size();++i)
		{ totalDataSizeBytes+=vDataSize(i); }
		return totalDataSizeBytes;
	}
	
	/* Immediately add */
	//void appendData
	
	void appendToFile(const std::string fileName)
	{
		std::ofstream ofs(fileName.c_str(), std::ios::binary | std::ios::out | std::ios::app);
		
		for (int i=0;i<vData.size();++i)
		{ ofs.write(vData(i), vDataSize(i)); }
	}
	
	bool loadFile(const std::string fileName)
	{
		// std::ofstream ofs(fileName.c_str(), std::ios::binary);
		
		// for (int i=0;i<vData.size();++i)
		// {
			// std::cout<<"Data size: "<<vDataSize(i)<<".\n";
			// ofs.write(vData(i), vDataSize(i));
		// }
		delete [] fileReadData;
		fileReadIndex=0;
		fileReadSize=0;
		
		
		std::ifstream file;
		file.open(fileName.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
		if(file.is_open())
		{
			fileReadSize=file.tellg();
			fileReadData = new char[fileReadSize];
			file.seekg (0, std::ios::beg);
			//file.read ((char*)fileReadData, fileReadSize);
			file.read (fileReadData, fileReadSize);
			file.close();
			return true;
		}
		else
		{
			std::cout<<"File didn't open.\n";
		}
		return false;
		
		
	}
	template <class T>
	void putIntoDatatype(T* var)
	{
		//*var=fileReadData[fileReadIndex];
		//var=(T*)fileReadData[fileReadIndex];
		
		DataTools::fromCharArray(var, &fileReadData[0]);
	}
	
	void clearMemory()
	{
		//for (int i=0;i<vData.size();++i)
		//{ delete [] vData(i); }
		vData.clear();
		vDataSize.clear();
	}
	
};