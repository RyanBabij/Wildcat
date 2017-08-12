#ifndef GRAPHICS_2D_TILESET_HPP
#define GRAPHICS_2D_TILESET_HPP

#include <Data/Font.hpp>
#include <Container/ArrayS2.hpp>

#include <Graphics/Buffer.hpp>
#include <Graphics/2D/Lif.hpp>

#include <File/FileManager.hpp>
/* Deprecated */
class TileSet
{
	public:
	std::vector <Lif*> tile;
	
	//void load(std::string path)
	//{
	//	Lif* image = new Lif;
	//	FileManager file;
	//	image->load(file.loadObject(path));
	//	tile.push_back(image);
	//}
	~TileSet()
	{
		//for(unsigned int i=0;i<tile.size();++i)
		//{
			//delete tile.at(i);
		//}
	}
	inline Lif* operator() (unsigned int id)
	{
		return tile.at(id);
	}
	unsigned char* getRaw(int id)
	{
		return tile.at(id)->image.data;
	}
};

#endif
