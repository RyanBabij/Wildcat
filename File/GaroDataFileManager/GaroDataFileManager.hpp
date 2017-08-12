#pragma once

/*

Garo Data File Manager Library
#include <File/GaroDataFileManager/GaroDataFileManager.hpp>

Provides a common interface for storing data for all of my programs. Useful for saving game data. See GaroDataFileManager.odt for more info.

*/
// #include <iostream>
// #include <fstream>

#include <File/FileManager.hpp>


/* A Garo Data Block is a small segment of a Garo Data File, that groups a file into organised blocks. One day I'll allow data blocks to have other data blocks within them. */
class DataBlock
{
	public:
	std::string header;
	Vector <char> vData;
};

/* Takes Garo Data Files and makes them easier to work with. */
/* Garo Data Files are the spec that my games follow, to make loading data from files easier. */

class GaroDataFileManager
{
	public:
	std::string fileHeader;
	bool invalidFile;
	Vector <DataBlock*> vDataBlock;
	
	GaroDataFileManager()
	{
		invalidFile=true;
		fileHeader="";
	}
	
	/* Returns the first datablock with the provided header. */
	DataBlock* getDataBlock(std::string searchHeader)
	{
		for (int i=0;i<vDataBlock.size();++i)
		{
			if(vDataBlock(i)->header == searchHeader)
			{ return vDataBlock(i); }
		}
		/* No datablock could be found. Return null pointer. */
		return 0;
	}
	/* Returns all datablocks with the provided header. */
	Vector <DataBlock*>* getAll(const std::string searchHeader)
	{
		std::cout<<"Getting data block: "<<searchHeader<<".\n";
		Vector <DataBlock*>* _vDataBlock = new Vector <DataBlock*>;
	
		for (int i=0;i<vDataBlock.size();++i)
		{
			if(vDataBlock(i)->header == searchHeader)
			{
				std::cout<<"Header match.\n";
				_vDataBlock->push(vDataBlock(i));
			}
		}
		return _vDataBlock;
	}
	
	/* Checks to make sure that a Garo Data File can be written. Returns true if the file is okay to use.
	If the file is invalid, it is backed up and deleted. If the file doesn't exist, it is created. */
	
	bool prepareFile(const std::string FILE_PATH, std::string _fileHeader)
	{
		//std::cout<<"Preparing Garo Data File.\n";

		/* Open file for reading in binary mode. */
		std::FILE* pFile = std::fopen (FILE_PATH.c_str(),"rb");
		
		/* If the file doesn't exist or couldn't be opened. (No c standard way to know if a file exists.) */
		if(pFile==0)
		{
			// std::cout<<"File didn't open for reading.\n";
			// std::cout<<"Opening file for writing.\n";
		
			/* Open file for writing in binary mode. This will create a new file if the file doesn't exist. */
			pFile = std::fopen (FILE_PATH.c_str(),"wb");
			
			/* If the file couldn't be opened for writing, there is a problem. Return error. */
			if (pFile==0)
			{
				//std::cout<<"Error: Unable to open file.\n";
				return false;
			}
			else
			{
				//std::cout<<"File opened for writing.\n";
				
				/* Make sure the file is empty. If the file isn't empty, then there was an error opening the file for reading. Return error. */
				
				std::fseek (pFile , 0 , SEEK_END);
				if(ftell(pFile)==0)
				{
					std::cout<<"File is empty.\n";
					
					/* Now we can be fairly sure that the file didn't exist, and we can create a new Garo Data File here. */
					
					/* Add STX and ETX to file header. */
					const char STX = 2;
					const char ETX = 3;
					_fileHeader.insert(_fileHeader.begin(),STX);
					_fileHeader.insert(_fileHeader.end(),ETX);
					/* Write the string to the file. */
					fputs(_fileHeader.c_str(),pFile);
					std::cout<<"File header added. File prepared.\n";
				}
				else
				{
					/* If the file isn't empty, but didn't open for reading earlier, then there is a problem. */
					/* I've never had something like this happen, but... */
					std::cout<<"Error: File " << FILE_PATH << " didn't open for reading, but exists. Aborting.\n";
					return false;
				}
				
			}
			
			
		
		}
		/* If the file opened for reading, it exists. We must make sure it is a valid Garo Data File. */
		else
		{
			std::cout<<"File opened for reading.\n";
			
			/* Get file size. */
			std::fseek (pFile , 0 , SEEK_END);
			const unsigned int fileSize = ftell(pFile);
			/* Reset file position to start of file. */
			fseek(pFile, 0, SEEK_SET);
			
			
			/* File size must at least be large enough to accomodate file header. */
			if (fileSize < _fileHeader.size()+2)
			{
				std::cout<<"Error: File is too small to be valid.\n";
				std::fclose(pFile);
				return false;
			}
			
			


			std::cout<<"Filesize: "<<fileSize<<" bytes.\n";
			
			char fileData [fileSize+1]; /* File data + null terminator. */
			if(fgets(fileData,fileSize+1,pFile)!=0) /* Arg 2 must account for null terminator. */
			{
				/* Add STX and ETX to file header. */
				const char STX = 2;
				const char ETX = 3;
				_fileHeader.insert(_fileHeader.begin(),STX);
				_fileHeader.insert(_fileHeader.end(),ETX);
			
			
				/* Compare file data with expected file header */
				for (unsigned int i=0;i<fileSize+1;++i)
				{
					if(fileData[i] != _fileHeader[i])
					{ std::cout<<"File header doesn't match.\n"; return false; }
				}
				
				/* File headers match, so file is valid. */
				std::cout<<"File is valid.\n";
				return true;
				
			
			}
			/* If there was a failure reading the file. */
			else
			{
				std::cout<<"Failed to read file data.\n";
				return false;
			}
			
			std::fclose(pFile);
		}
		
	
		return false;
	}
	
	
	/* Load all data blocks from the file into RAM. */
	void loadFile(const std::string filePath)
	{
		std::cout<<"Loading Garo Data File: "<<filePath<<".\n";
		/* Make sure the file is valid. */
		
		/* Get the file header. */
		fileHeader = FileManagerStatic::getHeader(filePath,0);
		/* If the file header is invalid, then abort. */
		if (fileHeader=="") { std::cout<<"Didn't get file header.\n"; invalidFile = true; return; }
		/* The first byte after the fileHeader ETX must be STX for a data block. */
		
		/* Get data blocks. */
		int filePosition = fileHeader.size();
		
		/* Delete STX and ETX from file header. */
		fileHeader.erase(0,1);
		fileHeader.erase(fileHeader.size()-1,1);
		
		while (true)
		{

			DataBlock* db = new DataBlock;
			db->header = FileManagerStatic::getHeader(filePath,filePosition);
			
			if (db->header=="")
			{
				std::cout<<"db->header didn't load.\n";
			}
			
			
			std::cout<<"FileHeader: "<<fileHeader<<".\n";
			std::cout<<"DataBlock header: "<<db->header<<".\n";
			
			/* Get data block size. */
			filePosition+=db->header.size();
			
			
			/* Delete STX and ETX from data block header. */
			db->header.erase(0,1);
			db->header.erase(db->header.size()-1,1);
			
			//const int startIndex = fileHeader.size()+db->header.size();
			//const int endIndex = fileHeader.size()+db->header.size()+sizeof(int)-1;
			//std::cout<<"Index: "<<startIndex<<", "<<endIndex<<".\n";
			std::string bData = FileManagerStatic::getData(filePath,filePosition,filePosition+sizeof(int)-1);
			
			// for (unsigned int i=0;i<bData.size();++i)
			// {
				// std::cout<<(int)bData[i]<<" ";
			// } std::cout<<".\n";
			int dataBlockSize = DataTools::valFromCharArray <int> (bData.c_str());
			std::cout<<"Data block size: "<<dataBlockSize<<" bytes.\n";
			
			/* Load those bytes into db Data Block object. */
			filePosition+=sizeof(int);
			
			std::string dbData = FileManagerStatic::getData(filePath,filePosition,filePosition+dataBlockSize-1);
			
			std::cout<<"Bytes to load into data block object: ";
			for (unsigned int i=0;i<dbData.size();++i)
			{
				std::cout<<(int)dbData[i]<<" ";
			} std::cout<<".\n";
			
			filePosition+=dataBlockSize;
			
			/* If the next byte is ETX, then the load was successful. Otherwise, the file is invalid. */
			if(FileManagerStatic::getByte (filePath,filePosition)==3)
			{
				std::cout<<"ETX found. Data block is valid.\n";
			}
			else
			{
				std::cout<<"Error: ETX not found. Invalid file. Aborting.\n";
				invalidFile=true;
				return;
			}
			/* Move the file position to the next byte. */
			++filePosition;
			
			/* The Data Block object can now be fully built, and pushed onto the Data Block vector. */
			
			std::cout<<"Loading bytes into data block.\n";
			for (unsigned int i=0;i<dbData.size();++i)
			{
				db->vData.push(dbData[i]);
				//std::cout<<(int)dbData[i]<<" ";
			} //std::cout<<".\n";
			std::cout<<"Pushing data block onto vector.\n";
			vDataBlock.push(db);
			std::cout<<"Data block loaded into DataFileManager. Loading next data block.\n";
			

			/* if the next byte is EOF, then we are finished loading the file. */
			const int nextByte = FileManagerStatic::getByte (filePath,filePosition);
			if (nextByte == EOF)
			{
				std::cout<<"EOF. Loading complete.\n"; return;
			}
			else if ( nextByte == 2 )
			{
				std::cout<<"STX found. Ready to load next data block.\n";
			}
			else
			{
				std::cout<<"Unexpected character found. Aborting.\n";
				return;
			}
			std::cout<<"The next byte is: "<<FileManagerStatic::getByte (filePath,filePosition)<<".\n";
		}
	
		std::cout<<"Garo Data File is loaded.\n";
	}
	
	
};
