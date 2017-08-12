#ifndef FILEMANAGER_STATIC_HPP
#define FILEMANAGER_STATIC_HPP

//#include <iostream>
#include <fstream>

// NEED TO SORT THESE OUT.
//#include <iostream>
//#include <fstream>
//#include <stdio.h>
//#include <sstream>
//#include <vector>

//#include <Data/DataTools.hpp>

/*	FileManagerStatic
	#include <File/FileManagerStatic.hpp>

	Library to make file IO easier. This one has less power than FileManager, but is much easier to use. I find this one to be adequate most of the time.

	Features:
		Load file into char array.
		Append strings to file.

	Todo:
		Garbage collection.

	Notes:

	+ You don't need to close the file stream if the destructor is going to be called anyway. The destructor will take care of it.
	+ I changed the behaviour of 'writeString(~)'. It now creates a file if one doesn't exist. This could be bad.

	+ If you are writing thousands of lines, it is much, much faster to put the data into a string and write it with a single FileManager call.

	+ I am changing writeString(~) to a generic write(~). This means you can also pass chars.

*/
#include <iostream>

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

	/* 0223355067 - Check if the file contains the passed string at the given index. */
	static bool matches (const std::string file, int startIndex, const std::string str)
	{
		return ( getData (file,startIndex,startIndex + str.size()) == str );
	}

	/* Get data between (inclusive) the given points. If endIndex is out of bounds, it returns the string up to the EOF. */
	/* TODO: Allow backwards reading, by making startIndex larger than endIndex. */
	static std::string getData(const std::string file, int startIndex, const int endIndex)
	{
		/* if the startIndex is bigger than the endIndex, or the endIndex goes out of bounds. */
		//const int fileSize = getFileSize(file);

		//if(startIndex>endIndex)
	//	{ return ""; }


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

				// OLD WAY WHICH DOESN"T SEEM TO BE RELIABLE.
			// const unsigned int fileSize=file2.tellg();
			// std::cout<<"Filesize: "<<fileSize<<".\n";
			// char* fileData = new char[fileSize+1];
			// file2.seekg (0, std::ios::beg);
			// file2.read (fileData, fileSize);
			// file2.close();
				// // Hafta add the null terminator myself.
			// fileData [fileSize] = '\0';

			// strRet = fileData;

		}
		
		return strRet;
	}
	// ALIASES FOR static std::string getData(const std::string file)
	static std::string loadFile(const std::string file) { return getData(file);  }
	static std::string openFile(const std::string file) { return getData(file);  }
	static std::string open(const std::string file) { return getData(file);  }
	static std::string load(const std::string file) { return getData(file);  }
	static std::string getFile(const std::string file) { return getData(file);  }


	/*TODO: Function to put all data from a file into a char array. */

	/* Look for a certain byte in the file, from the given position. Will modify the position to be on the found byte, if there is one, otherwise, function will return false. */




	/* Move the current IO position on the file object. Can use negative and positive numbers. EOF or error returns false. */

	// NEEDS TO BE UPDATED FOR NON-fpos_t.
	static bool modifyFilePosition(std::FILE* pFile, const int positionModifier)
	{
		//std::cout<<"Moving file pointer.\n";
		//std::fpos_t filePosition;
		//std::fgetpos (pFile, &filePosition);
		//filePosition+=positionModifier;
		//std::fsetpos (pFile, &filePosition);
		return false;
	}



};

#endif

#ifdef NOPE

	// OLD CODE I NEED TO FIX ONE DAY

// **************************************







	/* Get all data blocks in the file with a specified header. */
	static std::string getData(const std::string filePath, std::string fileHeader, std::string dataBlockHeader)
	{
		std::cout<<"Get data.\n";

		/* Add STX and ETX to headers. */
		//fixHeader(&fileHeader);
		fixHeader(&dataBlockHeader);

		std::string fileData="";
		/* Open file for reading. */
		std::FILE* pFile=std::fopen(filePath.c_str(),"r");
		/* If the file opened for reading. */
		if (pFile!=NULL)
		{
			std::cout<<"File opened.\n";
			/* If the file header is valid. */
			if ( checkFileHeader (filePath,fileHeader)==true )
			{
				std::cout<<"File header valid.\n";

				/* Set the index to just after the file header. */
				std::fpos_t filePosition = fileHeader.size()+2;
				std::fsetpos (pFile, &filePosition);

				/* Look for sticks. */


				// /* The next byte should be STX. */
				// int fileByte = std::fgetc(pFile);

				// /* If we're at the end of the file. */
				// if(fileByte==EOF) { return fileData; }

				std::cout<<"Looking for sticks.\n";
				/* ASCII code 2: STX. */
				int sticksPos = filePosition;
				if(find(filePath, 2, &sticksPos))
				{
					std::cout<<"Sticks found: "<<sticksPos<<".\n";

					/* Get the header after STX. */
					const std::string header = getHeader(filePath,sticksPos);
					std::cout<<"Header is: "<<header<<".\n";

					/* If this is the specified data block header, then we will retrieve the data. Otherwise we will move onto the next header. */
					std::cout<<"Searching for header: "<<dataBlockHeader<<".\n";
					if ( header == dataBlockHeader)
					{
						std::cout<<"This header matches parameters. Putting data onto string.\n";

						/* The next data should be an int which provides the size of the data block in bytes. */


						/* Move read position to end of header. */
						//sticksPos += header.size();
						filePosition = sticksPos+header.size();
						std::fsetpos (pFile, &filePosition);
						//modifyFilePosition(pFile,header.size());
						//modifyFilePosition(pFile,(int)header.size());
						//modifyFilePosition(0,(int)header.size());
						// std::cout<<"Next byte: "<<std::fgetc(pFile)<<".\n";
						// std::cout<<"Next byte: "<<std::fgetc(pFile)<<".\n";
						// std::cout<<"Next byte: "<<std::fgetc(pFile)<<".\n";
						// std::cout<<"Next byte: "<<std::fgetc(pFile)<<".\n";

						/* The first bytes indicate the size of the data block. */
						const int sizeOfInt = sizeof(int);
						char* aDataBlockSize = new char [ sizeOfInt ];
						for (int i=0;i<sizeOfInt;++i)
						{
							int nextByte = std::fgetc(pFile);
							if (nextByte == EOF)
							{
								std::cout<<"File unexpectedly ended. Aborting.\n";
								return "";
							}
							aDataBlockSize[i]=(char)nextByte;
						}
						const int dataBlockSize = DataTools::valFromCharArray <int> (aDataBlockSize);
						delete [] aDataBlockSize;
						std::cout<<"Data block size is: "<<dataBlockSize<<". Moving to end of datablock.\n";

						filePosition+=sizeOfInt+dataBlockSize;
						std::fsetpos (pFile, &filePosition);

						int nextByte = std::fgetc(pFile);

						std::cout<<"Byte: "<<nextByte<<".\n";

						std::cout<<"Bypassing closing header.\n";
						std::cout<<"Header.size(): "<<header.size()<<".\n";
						filePosition+=header.size();
						std::fsetpos (pFile, &filePosition);
						nextByte=std::fgetc(pFile);
						std::cout<<"Byte: "<<nextByte<<".\n";
						if(nextByte==EOF)
						{
							std::cout<<"EOF. Returning.\n";
							return "";
						}
						else if (nextByte == 2)
						{
							std::cout<<"STX found again. Getting header.\n";
							const std::string header2=getHeader(filePath,(int)filePosition);
							std::cout<<"Header: "<<header2<<".\n";
						}

					}
					else
					{
						std::cout<<"This header doesn't match parameters.\n";
					}






				}
				else
				{
					std::cout<<"Sticks not found. Returning.\n";
					return fileData;
				}


			}

			// /* Get the next byte of the file. */
			// const int fileChar = std::fgetc(pFile);
			// /* If we're at the end of the file, return the string we have. */
			// if(fileChar==EOF)
			// { return fileData; }
			// /* Add the character to the string. */
			// fileData.push_back((char)fileChar);
		}
		return fileData;
	}

	/* Main function for all serialisation. */
	static bool appendCharArray(const std::string filePath, const char* data, const int nBytes)
	{
		/* Open file in binary and append mode. */
		std::ofstream fileStream;
		fileStream.open(filePath.c_str(), std::ios::binary|std::ios::app);
		/* Make sure the file is open for writing. */
		if(fileStream.is_open())
		{
			/* Write the variable, and provide the size of the variable in bytes. */
			fileStream.write(data,nBytes);
			fileStream.close();
			return true;
		}
		/* If an error occured, return false. */
		return false;
	}


	/* Append a variable, such as an int or double, to the end of a file.
		Usage:
		int var = 666;
		FileManagerStatic::appendVariable(fileName,&var);
	*/
	template <class T>
	static bool appendVariable ( const std::string filePath, T* _data )
	{
		/* Convert to char array, and pass to appendCharArray. */
		return (appendCharArray(filePath,DataTools::toCharArray(_data),sizeof(T)));

		// /* Open file in binary and append mode. */
		// std::ofstream fileStream;
		// fileStream.open(filePath.c_str(), std::ios::binary|std::ios::app);

		// if(fileStream.is_open())
		// {
			// /* Write the variable, and provide the size of the variable in bytes. */
			// fileStream.write(DataTools::toCharArray(_data),sizeof(T));
			// fileStream.close();
			// return true;
		// }
		// /* If an error occured, return false. */
		// return false;
	}

	static bool appendString (const std::string filePath, const std::string strData)
	{
		/* Convert to char array (c string), and pass to appendCharArray. */
		return (appendCharArray(filePath,strData.c_str(),strData.size()));
	}

	/* Breaks for some reason. */
	// static bool appendNull (const std::string filePath)
	// {
		// return (appendCharArray(filePath,'\0',1));
	// }

	/* Adds a string enclosed with STX (ASCII code: 2), and ETX (ASCII code: 3). */
	static bool appendHeader(const std::string filePath, std::string strData)
	{
		const char STX = 2;
		const char ETX = 3;
		strData.insert(strData.begin(),STX);
		strData.insert(strData.end(),ETX);

		return (appendString(filePath,strData));
	}

	/* Checks for STX and ETX ASCII codes, and checks header, returning true if header matches. Returns false if the file lacks STX and ETX, or header doesn't match. The passed string shouldn't have STX and ETX. */
	static bool checkFileHeader (const std::string filePath, std::string strHeader)
	{
		/* Enclose in STX and ETX. */
		fixHeader(&strHeader);

		/* Open file for reading. */
		std::FILE* pFile=std::fopen(filePath.c_str(),"r");
		/* If the file opened for reading. */
		if (pFile!=NULL)
		{
			/* Check to see if the header matches so far. */
			for (unsigned int currentIndex=0; currentIndex<strHeader.size();++currentIndex)
			{
				/* Get the next character in the file. Must be int to check for EOF. */
				const int fileChar = std::fgetc(pFile);
				/* The file has ended before the string can be matched, so we return false. */
				if(fileChar==EOF)
				{ return false; }

				/* If character doesn't match string, return false. */
				if (strHeader[currentIndex]!=(char)fileChar)
				{ return false; }

			}
			/* Header matches string. Return true. */
			return true;
		}
		return false;
	}


	/* Returns a char array of the data block which has a certain header. Starts searching from a specified point in the file, so that this function can be used to find multiple data blocks. Updates index to the point where it left off. */
	// static char* findDataBlock ( int* startIndex, const std::string searchHeader )
	// {
	// }

	/* Each char contains a byte of data from the desired datablocks. The datablocks are squashed into the single vector. */
	/* Each data block contains a value indicating how many bytes until the next data block. We make use of this feature. In fact, this is necessary for such operations, as keywords alone cannot be considered reliable. */
	/* I could make a DataBlock class, to make serialisation easier. */
	//static Vector <char> getDataBlockIndexVector(std::string header, const std::string filePath, int currentIndex=0)
	//{
		//Vector <char> vDataBlocks;

		/* Add STX and ETX to string. */
		//fixHeader(&header);



		/* The provided index must either be pointing at the start of the file, or at the first character of a data block. In both cases, the first character should be ETX. */

		/* Open file and start searching for header. */
		/* Set the index to the start of the first datablock header. */


		/* Open file for reading. */
		// std::FILE* pFile=std::fopen(filePath.c_str(),"r");
		// /* If the file opened for reading. */
		// if (pFile!=NULL)
		// {

			// /* Get file size. */

			// /* Check to see if the header matches so far. */
			// for (unsigned int currentIndex=0; currentIndex<strHeader.size();++currentIndex)
			// {
				// /* Get the next character in the file. Must be int to check for EOF. */
				// const int fileChar = std::fgetc(pFile);
				// /* The file has ended before the string can be matched, so we return false. */
				// if(fileChar==EOF)
				// { return false; }

				// /* If character doesn't match string, return false. */
				// if (strHeader[currentIndex]!=(char)fileChar)
				// {
					// return false;
				// }



			// }
			// /* Header matches string. Return true. */
			// return true;

		// }
		// return false;



	//}

	/* Adds STX to beginning of string and ETX to end of string. This makes it a valid header which can be read by the file manager. */
	static void fixHeader(std::string* header)
	{
		const char STX = 2;
		const char ETX = 3;
		header->insert(header->begin(),STX);
		header->insert(header->end(),ETX);
	}

	/* Can return EOF. Returns -1 for error. */
	static int getByte (const std::string filePath, const int position)
	{
		/* Open file for reading. */
		std::FILE* pFile=std::fopen(filePath.c_str(),"r");
		/* If the file opened for reading. */
		if (pFile!=NULL)
		{
			/* Read byte from position. */
			std::fpos_t objFilePosition = position;
			std::fsetpos (pFile, &objFilePosition);
			return std::fgetc(pFile);
		}

		return -1;
	}

	/* Find the header at the given point. The given point must be on STX. Pos 0 is file header. */
	static std::string getHeader(std::string filePath, int filePosition=0)
	{
		std::string header="";

		/* Open file for reading. */
		std::FILE* pFile=std::fopen(filePath.c_str(),"r");
		/* If the file opened for reading. */
		if (pFile!=NULL)
		{
			/* Set file position. */
			std::fpos_t objFilePosition = filePosition;
			std::cout<<"Get header has file position: "<<filePosition<<".\n";
			std::fsetpos (pFile, &objFilePosition);
			/* Check for sticks. */
			int currentByte = std::fgetc(pFile);

			/* If we have read to the end of the file. */
			if (currentByte==EOF)
			{ return header; }

			/* If the byte is STX, then we are at the right spot. */
			if (currentByte==2)
			{
				std::cout<<"Header is ready to be read.\n";
				header+=(char)2;

				/* Read header into string until ETX is found. */
				while (true)
				{
					currentByte = std::fgetc(pFile);

					/* If we reached the EOF, then that's an error, and we will return an empty string. */
					if(currentByte == EOF)
					{ return ""; }
					else if(currentByte == 3)
					{
						std::cout<<"Header is good.\n";
						header+=(char)3;
						return header;
					}
					header+=(char)currentByte;
				}

			}
			else
			{
				std::cout<<"Header doesn't have the right position: "<<currentByte<<".\n";
			}

			/* Keep going until ETX found. */
		}

		return header;
	}

	/* TODO: Find the header at the given point, and move the read position ahead. The given point must be on STX. Pos 0 is file header. */

	/* Takes the index and works from that position until it finds a desired datablock. Then it modifies the index to the position of the next datablock. */
	// static void indexToDataBlock(int* index, const std::string filePath)
	// {
	// }







	// DOESNT WORK
	//static bool writeChar ( const char _char, const std::string _file)
	//{
		//std::string _text = ""+_char;

		//writeString(_text,_file);

		/* Open a file for appending. */
		// std::FILE * pFile = std::fopen (_file.c_str(),"a");
		// if (pFile!=NULL)
		// {
			// std::fputs (_text.c_str(),pFile);
			// std::fclose (pFile);
			// return true;
		// }
		/* If there was an error, return false. */
		//return false;
	//}

	static bool find(const std::string filePath, const char findByte, int* startPos)
	{
		//std::cout<<"Looking for byte: "<<(int)findByte<<".\n";

		/* Open file for reading. */
		std::FILE* pFile=std::fopen(filePath.c_str(),"r");
		/* If the file opened for reading. */
		if (pFile!=NULL)
		{
			/* Set position to provided index. */

			//BAD CODE.
			//std::fpos_t filePosition = *startPos;
			//std::fsetpos (pFile, &filePosition);
			// PROPER CODE?
			std::fseek(pFile,*startPos,SEEK_SET);

			int currentPos = *startPos;


			while (true)
			{
				int fileByte = std::fgetc(pFile);
				std::cout<<"Find: "<<fileByte<<".\n";

				if ( fileByte==EOF )
				{
					return false;
				}
				if ((char)fileByte == findByte)
				{
					/* Set index and return true for successful find. */
					std::fgetpos (pFile, &filePosition);
					/* I don't know why, but I need to increment file position to land on the right spot. */
					*startPos = filePosition-1; /* Decrement because fgetc increments the position. */
					//*startPos = filePosition+1;
					std::cout<<"Byte found. Pos: "<<*startPos<<". Val: "<<(int)findByte<<".\n";
					return true;
				}

			}
		}
		return false;
	}

#endif
