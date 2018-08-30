#ifndef WILDCAT_SAVEFILEMANAGER_HPP
#define WILDCAT_SAVEFILEMANAGER_HPP

#include <Data/DataTools.hpp>

/* Wildcat:SaveFileManager.hpp
  #include <File/SaveFileManager.hpp>
  
  Class to help turning game state into a file, and loading it back into a game. Uses FileManager for most File IO.
  
  Works by loading the save file as a string, then performing operations to extract useful information from the string.

*/

#include <File/FileManager.hpp>

class SaveFileManager
{
	public:
  std::string data;

	SaveFileManager() {}

  
	~SaveFileManager() {}
};
#endif
