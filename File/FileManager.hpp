#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include <vector>

#include <Data/DataTools.hpp>

#include <experimental/filesystem> /* New c++14 experimental support for filesystems. Should be standard in C++17 */

/* File/FileManager.hpp
Library to make file IO easier. Also allows easier object serialisation.
*/

/* Filecontent must be manually deleted */
/* The non-static functions may be stupid... They are also */
/* glitchy, and I can't be assed fixing something that */
/* I may not ever use again. */


/* write: Means to immediately write something to a file. */
/* push: Means to queue something to be written later. */

/* FunctionManagerStatic
	Contains self-contained functions. Easier for once-off file IO.
*/

/* Object that contains data about a file. */

// class FileManager_File
// {
	// public:
	// bool exists;
	// int size;
	// std::string fileName;

// };


/* FileManager
	Allows more performance and flexibility compared to FileManagerStatic, for multiple IO operations with a file.
*/

class FileManager
{
	public:
	std::vector <unsigned char*> data;
	std::vector <int> nData;

	FileManager() {}

	void clearMemory()
	{
		/* clear the vectors here. */
	}



	/* Make sure the file has the correct header, so we know we are using the right file. */
	// static bool checkFileHeader(const std::string _file, const std::string _fileHeader)
	// {

	// }



	// static bool fileIsBlank(const std::string filePath)
	// {
		// FILE* pFile = fopen (filePath.c_str(),"r");
		// // obtain file size:
		// std::fseek (pFile , 0 , SEEK_END);
		// long lSize = ftell (pFile);
		// //std::rewind (pFile);
		// std::fclose (pFile);
		// if(lSize==0) { return true; }
		// return false;
	// }

	/* File status. I'll combine the above functions. */
	/* Possible values:
		1 : File doesn't exist.
		2 : File exists and is empty.
		3 : File exists and contains data.
	*/
	// static char fileStatus (const std::string filePath)
	// {
		// std::cout<<"Blah.\n";
		// return 0;
	// }

	/* Push a variable, such as an int or double, onto the data vector. */
	template <class T>
	void pushVariable ( T* _data )
	{
		/* Push data. */
		data.push_back(DataTools::toCharArray(_data));
		/* Push size of data in bytes. */
		nData.push_back(sizeof(T));
	}


	/* Add a char array to the data vector. */
	void addData(unsigned char* _data, int _nData)
	{
		data.push_back(_data);
		nData.push_back(_nData);
	}

	/* Add file contents into RAM. Returns false if the file won't open. */
	bool addFile(const char * filePath)
	{
		std::ifstream file;
		file.open(filePath, std::ios::in|std::ios::binary|std::ios::ate);
		if(file.is_open())
		{
			int size=file.tellg();
			nData.push_back(size);
			unsigned char* data2 = new unsigned char[size];
			file.seekg (0, std::ios::beg);
			file.read ((char*)data2, size);
			file.close();
			data.push_back(data2);
			return true;
		}
		return false;
	}
	bool addFile(std::string filePath)
	{ return addFile(filePath.c_str()); }

	bool saveToFile(std::string filePath)
	{
		std::ofstream file (filePath.c_str());
		if(file.is_open())
		{
			for(unsigned int i=0;i<data.size();++i)
			{ file.write((char*)data.at(i),nData.at(i)); }
			file.close();
			return true;
		}
		return false;
	}

	static unsigned char* getFile(std::string filePath, int* fileSize)
	{ return getFile(filePath.c_str(),fileSize); }
	static unsigned char* getFile(const char* filePath, int* fileSize)
	{
		std::ifstream file;
		file.open(filePath, std::ios::in|std::ios::binary|std::ios::ate);
		if(file.is_open())
		{
			*fileSize=file.tellg();
			unsigned char* data2 = new unsigned char [*fileSize];
			file.seekg (0, std::ios::beg);
			file.read ((char*)data2, *fileSize);
			file.close();
			return data2;
		}
		return 0;
	}
	static bool saveToFile(std::string filePath, unsigned char* data2, int dataSize)
	{
		std::ofstream file;
		file.open(filePath.c_str(), std::ios::out|std::ios::binary|std::ios::ate);
		if(file.is_open())
		{
			file.write((char*)data2,dataSize);
			file.close();
			return true;
		}
		return false;
	}

	void filterText(std::string acceptableWords)
	{
		std::cout<<"Filtering...\n";
		std::vector <unsigned char> data2;

		bool recentNewline = false;

		for(unsigned int i=0;i<data.size();++i)
		{
			for(int i2=0;i2<nData.at(i);++i2)
			{
				//std::cout<<data.at(i)[i2];

				for(unsigned int i3=0;i3<acceptableWords.length();++i3)
				{
					if(data.at(i)[i2] == '\n' && recentNewline == false)
					{
						data2.push_back(' ');
						data2.push_back('*');
						data2.push_back(' ');
						recentNewline=true;
						break;
					}
					else if(data.at(i)[i2] == acceptableWords[i3])
					{
						//std::cout<<data.at(i)[i2];
						data2.push_back(data.at(i)[i2]);
						recentNewline=false;
						break;
					}
				}
			}
		}

		//unsigned char data3 [data2.size()];
		unsigned char* data3 = new unsigned char [data2.size()];
		for(unsigned int i=0;i<data2.size();++i)
		{
			data3[i]=data2.at(i);
		}

		std::ofstream file;
		file.open("textout.txt", std::ios::out|std::ios::binary|std::ios::ate);
		if(file.is_open())
		{
			file.write((char*)data3,data2.size());
			file.close();
		}
		delete data3;
	}

	
	static bool createDirectory(std::string _path)
	{
		if (CreateDirectory(_path.c_str(), NULL) ||
			ERROR_ALREADY_EXISTS == GetLastError())
		{
			// CopyFile(...)
		}
		else
		{
			return false;
		}
		return true;
	}
	
	// for some reason I'm having permission issues with this. Wait for g++ update. For now use DeleteDirectory instead.
	static bool removeDirectory(std::string _path)
	{
		
		
		
		//std::filesystem::remove_all("test");
		
		/* New c++17 feature. */
		namespace filesys = std::experimental::filesystem;
		filesys::remove(_path);
		
		return false;	
		

	}
	//https://stackoverflow.com/questions/734717/how-to-delete-a-folder-in-c
	
static int DeleteDirectory(const std::string &refcstrRootDirectory, bool bDeleteSubdirectories = true)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
	// subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	std::string     strFilePath;                 // Filepath
	std::string     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information


	strPattern = refcstrRootDirectory + "\\*.*";
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(FileInformation.cFileName[0] != '.')
			{
				strFilePath.erase();
				strFilePath = refcstrRootDirectory + "\\" + FileInformation.cFileName;

				if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if(bDeleteSubdirectories)
					{
						// Delete subdirectory
						int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
						if(iRC)
							return iRC;
					}
					else
						bSubdirectory = true;
				}
				else
				{
					// Set file attributes
					if(::SetFileAttributes(strFilePath.c_str(),
					FILE_ATTRIBUTE_NORMAL) == FALSE)
					return ::GetLastError();

					// Delete file
					if(::DeleteFile(strFilePath.c_str()) == FALSE)
					return ::GetLastError();
				}
			}
		} while(::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if(dwError != ERROR_NO_MORE_FILES)
		return dwError;
		else
		{
			if(!bSubdirectory)
			{
				// Set directory attributes
				if(::SetFileAttributes(refcstrRootDirectory.c_str(),
				FILE_ATTRIBUTE_NORMAL) == FALSE)
				return ::GetLastError();

				// Delete directory
				if(::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
				return ::GetLastError();
			}
		}
	}

	return 0;
}
	
	
		// https://stackoverflow.com/questions/8233842/how-to-check-if-directory-exist-using-c-and-winapi
	static bool directoryExists(std::string _dirName)
	{
		DWORD attribs = GetFileAttributesA(_dirName.c_str());
		if (attribs == INVALID_FILE_ATTRIBUTES)
		{
			return false;
		}
		return (attribs & FILE_ATTRIBUTE_DIRECTORY);
	}
	
	// Make a new blank file. If the file already exists, make it blank. Returns false if something went wrong.
	static bool makeNewFile(const std::string filePath)
	{
		/* Create a blank file */
		std::FILE * pFile = std::fopen (filePath.c_str(),"w");
		if (pFile!=NULL)
		{ std::fclose (pFile); }
		return true;
	}
		inline static bool createFile(const std::string filePath)
		{ return makeNewFile(filePath); }
	
	
	~FileManager() {}
};
#endif
