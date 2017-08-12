#pragma once
#ifndef FILE_FILELOG_HPP
#define FILE_FILELOG_HPP

/* File/FileLog.hpp Created <0221743513. Updated 0221743513.
	I made this so that I can write data to a text file without needing to fuck around with the file manager.
	
	The rules of logging:
		* If there is an error, abort silently.
		* If a non-existent file is referred to, silently create it.
		
*/

#include <string>

#include <File/FileManagerStatic.hpp>
#include <Container/Vector/Vector.hpp>

class FileLog
{
	public:
	//FileManager file;
	
	//Vector <std::string> vMessage;
	std::string messages;
	std::string filePath;
	
	//std::string lastMessage;
	//int nRepetitions;
	
	FileLog()
	{
		filePath="";
		//lastMessage="";
		//nRepetitions=0;
	}
	
	static void log(const std::string _message, const std::string _filePath)
	{
		if (FileManagerStatic::fileExists(_filePath)==true)
		{
		}
		else
		{
			FileManagerStatic::makeNewFile(_filePath);
		}
		FileManagerStatic::writeString (_message,_filePath);
	}
	
		// Clear the file.
	static void clear(const std::string _filePath)
	{
		FileManagerStatic::makeNewFile(_filePath);
	}
	
	void log (const std::string message)
	{
		// file.load(path);
		// file.append(message);
		// file.save(path);
		//std::cout<<"Filelog doesn't actually do anything yet\n";
		
		// if(message==lastMessage)
		// {
			// ++nRepetitions;
		// }
		// else
		// {
			// /* Check if file exists. */
			// if(FileManagerStatic::fileExists(path) == false)
			// { FileManagerStatic::makeNewFile(path); }
			// /* Write the message. */
			// if(nRepetitions>0)
			// {
				// FileManagerStatic::writeString(DataTools::toString(nRepetitions) + "X: " +lastMessage,path);
			// }
			// else
			// { FileManagerStatic::writeString(lastMessage,path);
			// }
			
			
			// lastMessage=message;
			// nRepetitions=0;
			
		// }
		
		//vMessage.push(message);
		messages+=message;
		
	}
	
	void flush()
	{
		if(filePath!="")
		{
			/* Make file if it doesn't exist. */
			if(FileManagerStatic::fileExists(filePath) == false)
			{ FileManagerStatic::makeNewFile(filePath); }
			
			/* Write messages. */
			//const int nMessages = vMessage.size();
			//std::string allMessages = "";
			//for(int i=0;i<nMessages;++i)
			//{
			//	allMessages+=vMessage(i);
				//FileManagerStatic::writeString(vMessage(i),filePath);
			
			//}
			//vMessage.clear();
			
			FileManagerStatic::writeString(messages,filePath);
			messages="";
			
			// if(nRepetitions>0)
			// {
				// FileManagerStatic::writeString(DataTools::toString(nRepetitions) + "X: " +lastMessage,filePath);
			// }
			// else
			// { FileManagerStatic::writeString(lastMessage,filePath);
			// }
		}
	}
	
};

#endif
