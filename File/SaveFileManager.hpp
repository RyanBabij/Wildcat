#ifndef WILDCAT_SAVEFILEMANAGER_HPP
#define WILDCAT_SAVEFILEMANAGER_HPP

#include <Data/DataTools.hpp>

/* Wildcat:SaveFileManager.hpp
  #include <File/SaveFileManager.hpp>
  
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
  
  
  The only invalid characters in these save files are square brackets. Anything else may be typed into the save files without causing problems.
  
  
  Pointer management:
  
  Okay, so saving and loading basic raw data is fine. You convert to string, save to file. Load from file, convert from string.
  
  The real issue is how to store links between objects.
  
  The most obvious solution to me is to give every single saveable object in the world a unique ID.
  
*/

#include <File/FileManager.hpp>

/* Objects that can load and save should use this interface. */

class SaveFileInterface
{
  private:
  long long int uid;
  
  public:
  static long long int currentID;

  
  SaveFileInterface()
  {
    uid = currentID++;
  }
  ~SaveFileInterface() {}
  
  long long int getUID()
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

  
};
long long int SaveFileInterface::currentID = 0;

class SaveFileManager
{
	public:
  std::string data;
  
  Vector <SaveFileInterface*> vSaveObjects;
  

	SaveFileManager() {}
	~SaveFileManager() {}
  
  
  // Load the savefile into RAM.
  void loadFile (std::string _file)
  {
    std::cout<<"Load file.\n";
    
    data = FileManager::getFileAsString(_file);
    
  }
  
  // Load the savefile into RAM.
  void saveFile (std::string _file)
  {
    std::cout<<"Save file.\n";
    data = "";
    for (int i=0;i<vSaveObjects.size();++i)
    {
      data += vSaveObjects(i)->getSaveData();
    }
    
    //data = FileManager::getFileAsString(_file);
    
  }
};
#endif
