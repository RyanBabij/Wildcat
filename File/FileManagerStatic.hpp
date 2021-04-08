#ifndef FILEMANAGER_STATIC_HPP
#define FILEMANAGER_STATIC_HPP

#include <fstream> // file IO
#include <string> // filepaths

/*	FileManagerStatic
	#include <File/FileManagerStatic.hpp>

	Library to make file IO easier. This one has less power than FileManager, but is much easier to use. I find this one to be adequate most of the time.
	
	Notes:

	+ You don't need to close the file stream if the destructor is going to be called anyway. The destructor will take care of it.
	+ I changed the behaviour of 'writeString(~)'. It now creates a file if one doesn't exist. This could be bad.
	+ If you are writing thousands of lines, it is much, much faster to put the data into a string and write it with a single FileManager call.
	+ I am changing writeString(~) to a generic write(~). This means you can also pass chars.
	
	Todo:
	
	+ Function to load file data into char array.

*/


class FileManagerStatic
{
	public:
	
		// Clear the file (or create it if it doesn't exist), then write the string to it.
	static bool writeFreshString ( const std::string _text, const std::string _filePath)
	{
		makeNewFile(_filePath);
		return writeString(_text,_filePath);
	}

	static bool write (const char _c, const std::string _filePath)
	{
		// IF FILE DOESN'T EXIST, CREATE IT.
		if ( fileExists (_filePath) == false )
		{ makeNewFile(_filePath); }

		// DOUBLE CHECK FILE EXISTS NOW.
		if ( fileExists (_filePath) == false )
		{
			// RETURN ERROR.
			return false;
		}

		// ACTUAL WRITE CODE
		std::FILE * pFile = std::fopen (_filePath.c_str(),"a");
		if (pFile!=NULL)
		{
			std::fputc (_c,pFile);
			std::fclose (pFile);
			return true;
		}
		/* If there was an error, return false. */
		return false;
	}

	// WRITE A STRING TO A FILE. IF THE FILE DOESN'T EXIST, CREATE IT.
	// RETURN FALSE IF SOMETHING WENT WRONG.
	static bool writeString ( const std::string _text, const std::string _filePath)
	{
		// IF FILE DOESN'T EXIST, CREATE IT.
		if ( fileExists (_filePath) == false )
		{
			makeNewFile(_filePath);
		}

		// DOUBLE CHECK FILE EXISTS NOW.
		if ( fileExists (_filePath) == false )
		{
			// RETURN ERROR.
			return false;
		}


		std::FILE * pFile = std::fopen (_filePath.c_str(),"a");
		if (pFile!=NULL)
		{
			std::fputs (_text.c_str(),pFile);
			std::fclose (pFile);
			return true;
		}
		/* If there was an error, return false. */
		return false;
	}

		/* Overwrite the file with the given string from the given position. The file will remain the same size unless the write operation extends past the EOF. */
	static bool overwriteFromPos ( const std::string _text, const std::string _file, const int _index)
	{
		/* Open file in binary and append mode. */
		std::ofstream fileStream;
        fileStream.open(_file.c_str(),std::ios::in|std::ios::out|std::ios::binary|std::ios::ate);
        fileStream.seekp(_index);
        fileStream.write(_text.c_str(),_text.size());
		return true;
	}

	// RETURN TRUE IF FILE EXISTS. RETURN FALSE IF FILE DOESN'T EXIST.
	static bool fileExists(const std::string filePath)
	{
		std::fstream ifile(filePath.c_str());
		// IF THE FILE DOESN'T EXIST, RETURN FALSE.
		//if (ifile == 0) { return false; }
			// COMPARISON TO 0 DOESN'T SEEM TO WORK ANYMORE HERE.
		if (!ifile) { return false; }
		return true;
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

	// Empty the contents of a file, but keep the file. If there is no file, a blank file will be created.
	static void clearFile (const std::string filePath)
	{ makeNewFile(filePath); }

	/* Return the file size in bytes. Returns 0 if the file is blank or doesn't exist. */
	static int getFileSize(const std::string _filePath)
	{
		/* Create a blank file */
		int fileSize=0;
		/* Open file for reading. */
		std::FILE * pFile = std::fopen (_filePath.c_str(),"r");
		/* If the file opened for reading. */
		if (pFile!=NULL)
		{
			/* Get the file size. */
			std::fseek (pFile , 0 , SEEK_END);
			fileSize = ftell (pFile);
			std::fclose (pFile);
		}
		/* Return the file size. */
		return fileSize;
	}

	/* Check if the file contains the passed string at the given index. */
	static bool matches (const std::string file, int startIndex, const std::string str)
	{
		return ( getData (file,startIndex,startIndex + str.size()) == str );
	}

	/* Get data between (inclusive) the given points. If endIndex is out of bounds, it returns the string up to the EOF. */
	/* TODO: Allow backwards reading, by making startIndex larger than endIndex. */
	static std::string getData(const std::string file, int startIndex, const int endIndex)
	{
		if(startIndex<0||endIndex<0)
		{ return ""; }

		std::string fileData = "";


		/* Open file for reading. */
		std::FILE* pFile=std::fopen(file.c_str(),"r");
		/* If the file opened for reading. */
		if (pFile!=NULL)
		{
			/* Set the file position. */


				// LINUX G++ DIDN'T LIKE THIS.
			//std::fpos_t filePosition = startIndex;
			//std::fsetpos (pFile, &filePosition);
				// APPARENTLY I NEED TO USE THIS INSTEAD, FOR INTEGER STUFF.
			std::fseek(pFile,startIndex,SEEK_SET);

			while (startIndex<=endIndex)
			{

				const int fileChar = std::fgetc(pFile);

				/* If we're at the end of the file, return the string we have. */
				if(fileChar==EOF)
				{ return fileData; }
				/* Add the character to the string. */
				//std::cout<<"Pushing: "<<fileChar<<".\n";
				fileData.push_back((char)fileChar);
				//std::cout<<"Push.\n";

				//if(startIndex==endIndex) { std::cout<<"Endindex.\n"; return fileData; }
				++startIndex;
			}

		}
		return fileData;
	}

	/* Get data from the entire file, in a string. This could cause memory problems if the file is too large. */
	
		// There seems to be a problem loading PNGs like this. There might be an issue reading terminators or something.
		// I used a different method which works.
		//https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
	static std::string getData(const std::string file)
	{
		std::string strRet = "";

		std::ifstream file2;
		file2.open(file.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
		if(file2.is_open())
		{
				// THIS NEW WAY CONVERTS DIRECTLY INTO STRING.
			file2.seekg(0, std::ios::end);
				// RESERVING MEMORY FOR THE STRING MAKES THINGS FASTER.
			strRet.reserve(file2.tellg());
			file2.seekg(0, std::ios::beg);
			strRet.assign((std::istreambuf_iterator<char>(file2)),
						std::istreambuf_iterator<char>());
		}
		
		return strRet;
	}
	// ALIASES FOR static std::string getData(const std::string file)
	static std::string loadFile(const std::string file) { return getData(file);  }
	static std::string openFile(const std::string file) { return getData(file);  }
	static std::string open(const std::string file) { return getData(file);  }
	static std::string load(const std::string file) { return getData(file);  }
	static std::string getFile(const std::string file) { return getData(file);  }

};

#endif

