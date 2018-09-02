#ifndef WILDCAT_SAVEFILEMANAGER_HPP
#define WILDCAT_SAVEFILEMANAGER_HPP

#include <Data/DataTools.hpp>
#include <File/FileManager.hpp>

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


/* Objects that can load and save should use this interface. */

class SaveFileInterface
{
  private:
    // uid is used to store pointers between objects.
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
  
  virtual void save()
  {
  }

  
};
long long int SaveFileInterface::currentID = 0;

class SaveFileManager
{
	public:
  std::string data;
  //std::string savePath;
  
  //Vector <SaveFileInterface*> vSaveObjects;
  
	//FileManager::createFile(worldFilePath);
  //FileManager::writeTag("LANDSEED",DataTools::toString(landmassSeed),worldFilePath);
  

	SaveFileManager()
  {
    data = "";
    //savePath = "";
  }
  
	~SaveFileManager() {}
  
  
  // Load the savefile into RAM.
  void loadFile (std::string _file)
  {
    std::cout<<"LOADING FILE: "<<_file<<".\n";
    data = FileManager::getFileAsString(_file);
    
  }
  
  // ADD VARIABLE TO SAVE STRING
  template <class T>
  void addVariable(std::string _tag, T _value)
  {
    std::cout<<"ADD VARIABLE\n";
    data+="["+_tag+":"+DataTools::toString(_value)+"]";
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
    std::cout<<"SAVING DATA TO: "<<_path<<".\n";
    
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
