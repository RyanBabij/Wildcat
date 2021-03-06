#pragma once
#ifndef WILDCAT_TEXTURE_TEXTURELOADER_HPP
#define WILDCAT_TEXTURE_TEXTURELOADER_HPP

/* #include <Texture/TextureLoader.hpp>

  Library to load textures and handle mipmaps. Currently only does OpenGL.

  NOTE: Beware of memory management. Texture data needs to be manually deleted in some cases.
*/

#if defined THREAD_ALL || defined THREADED_TEXTURE_LOADING
  #include <thread>
  #include <mutex>
  
  std::mutex MUTEX_TEXTURE_BIND;
#endif

#include <Graphics/Png/Png.hpp>
#include <Graphics/Texture/Texture.hpp>
#include <File/FileManager.hpp>

bool loadTextureMipmapRotate(const std::string filePath, GLuint* tex0, GLuint* tex90, GLuint* tex180, GLuint* tex270, const bool compress=false)
{ /* will rotate the image clockwise 90 degrees 4 times, and store each rotation as a texture */

	int fileSize;
	unsigned char* data = FileManager::getFile(filePath,&fileSize);

	if(data!=0)
	{
		Png png0(true);
		png0.load(data,fileSize);

		Texture* texture0 = new Texture;
		texture0->create(png0.nX,png0.nY,1);
		texture0->data=png0.data;

		Texture* texture90 = texture0->rotate90Clockwise();
		Texture* texture180 = texture90->rotate90Clockwise();
		Texture* texture270 = texture180->rotate90Clockwise();

		glGenTextures(1,tex0);
		glBindTexture(GL_TEXTURE_2D, *tex0);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		int i=0;
		while(texture0!=0)
		{
			glTexImage2D(GL_TEXTURE_2D, i++, GL_RGBA8, texture0->nX, texture0->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture0->data);
			Texture* temp=texture0;
			texture0 = texture0->createMipMap();
			delete temp;
		}
		glGenTextures(1,tex90);
		glBindTexture(GL_TEXTURE_2D, *tex90);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		i=0;
		while(texture90!=0)
		{
			glTexImage2D(GL_TEXTURE_2D, i++, GL_RGBA8, texture90->nX, texture90->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture90->data);
			Texture* temp=texture90;
			texture90 = texture90->createMipMap();
			delete temp;
		}
		glGenTextures(1,tex180);
		glBindTexture(GL_TEXTURE_2D, *tex180);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		i=0;
		while(texture180!=0)
		{
			glTexImage2D(GL_TEXTURE_2D, i++, GL_RGBA8, texture180->nX, texture180->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture180->data);
			Texture* temp=texture180;
			texture180 = texture180->createMipMap();
			delete temp;
		}
		glGenTextures(1,tex270);
		glBindTexture(GL_TEXTURE_2D, *tex270);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		i=0;
		while(texture270!=0)
		{
			glTexImage2D(GL_TEXTURE_2D, i++, GL_RGBA8, texture270->nX, texture270->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture270->data);
			Texture* temp=texture270;
			texture270 = texture270->createMipMap();
			delete temp;
		}

	}
	return false;
}

bool loadTextureAutoMipmap(const std::string filePath, GLuint* textureID, const bool compress=false)
{
	glGenTextures(1,textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	int i=0;
	int fileSize;
	unsigned char* data = FileManager::getFile(filePath,&fileSize);
	if(data!=0)
	{
		Png png(true);
		png.load(data,fileSize);

		Texture* texture = new Texture;
		texture->create(png.nX,png.nY,1);
		texture->data=png.data;

		while(texture!=0)
		{
			glTexImage2D(GL_TEXTURE_2D, i++, GL_RGBA8, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);

			Texture* temp=texture;
			texture = texture->createMipMap();
			delete temp;
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool loadTextureAutoMipmap(const std::string filePath, Texture* tex, const bool compress=false)
{
	glGenTextures(1,&tex->textureID);
	glBindTexture(GL_TEXTURE_2D, tex->textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	int i=0;
	int fileSize;
	unsigned char* data = FileManager::getFile(filePath,&fileSize);
	if(data!=0)
	{
		Png png(true);
		png.load(data,fileSize);

		Texture* texture = new Texture;
		texture->create(png.nX,png.nY,1);
		texture->data=png.data;

		while(texture!=0)
		{
			glTexImage2D(GL_TEXTURE_2D, i++, GL_RGBA8, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);

			Texture* temp=texture;
			texture = texture->createMipMap();
			delete temp;
		}
	}
	else
	{
		return false;
	}
	return true;
}

/* Apparently you can get OpenGL to do the mipmap generation for you... Wish I knew that sooner.
However, old versions of OpenGL may not support this, so it is a good idea to fall back to manual mipmap generation. */
bool loadTextureAutoMipmapNative(const std::string filePath, GLuint* textureID, const bool compress=false)
{
	/* NOTE: It seems that the GLuint needs to be initialised (ie to 0) before being passed here, in some cases. */
	glGenTextures(1,textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	int fileSize;
	unsigned char* data = FileManager::getFile(filePath,&fileSize);
	if(data!=0)
	{
		Png png(true);
		png.load(data,fileSize);

		Texture* texture = new Texture;
		texture->create(png.nX,png.nY,1);
		texture->data=png.data;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
	}
	else
	{
		return false;
	}
	return true;
}
bool loadTextureAutoMipmapNative(const std::string filePath, Texture* texture, const bool compress=false)
{
	/* NOTE: It seems that the GLuint needs to be initialised (ie to 0) before being passed here, in some cases. */
	glGenTextures(1,&texture->textureID);
	glBindTexture(GL_TEXTURE_2D, texture->textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	int fileSize;
	unsigned char* data = FileManager::getFile(filePath,&fileSize);
	if(data!=0)
	{
		Png png(true);
		png.load(data,fileSize);

		texture->create(png.nX,png.nY,1);
		texture->data=png.data;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
	}
	else
	{
		return false;
	}
	return true;
}

/* Uses nearest-neighbour scaling. This is useful when you want to preserve the pixel outlines, such
as when using sprites, or going for a retro look. */
bool loadTextureNearestNeighbour(const std::string filePath, GLuint* textureID, const bool compress=false)
{
	/* NOTE: It seems that the GLuint needs to be initialised (ie to 0) before being passed here, in some cases. */
	glGenTextures(1,textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);
 	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	int fileSize;
	unsigned char* data = FileManager::getFile(filePath,&fileSize);
	if(data!=0)
	{
		Png png(true);
		png.load(data,fileSize);
		Texture* texture = new Texture;
		texture->create(png.nX,png.nY,1);
		texture->data=png.data;

      if (compress)
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
      }
      else
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
      }
      
		
		/* 0221216034 Memleak fix? */
		delete texture;
	}
	else
	{ return false; }
	return true;
}


/* New implementation for Texture class. */
/* NOTE: Try switching internal format from GL_RGBA8 to GL_RGBA4. Also try compression. */
bool loadTextureNearestNeighbour(const std::string filePath, Texture* texture, const bool compress=false)
{
	if(texture==0) {return false;}
  
  int fileSize;
  FileManager fm;
  
  unsigned char* data = fm.getFile(filePath,&fileSize);
  if(data!=0)
  {
    Png png(true);
    png.load(data,fileSize);

    png.getAverageColour();
    texture->create(png.nX,png.nY,1);
    texture->data=png.data;
    texture->averageRed = png.averageRed;
    texture->averageGreen = png.averageGreen;
    texture->averageBlue = png.averageBlue;

  }
  else
  { return false; }
  

  /* NOTE: It seems that the GLuint needs to be initialised (ie to 0) before being passed here, in some cases. */
  glGenTextures(1,&texture->textureID);
  glBindTexture(GL_TEXTURE_2D, texture->textureID);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  
      if (compress)
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
      }
      else
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
      }

	return true;
}
  // OpenGL doesn't like threads
bool preloadTextureNearestNeighbour(const std::string filePath, Texture* texture)
{
	if(texture==0) {return false;}
  
  int fileSize;
  FileManager fm;
  
  unsigned char* data = fm.getFile(filePath,&fileSize);
  if(data!=0)
  {
    Png png(true);
    png.load(data,fileSize);

    png.getAverageColour();
    texture->create(png.nX,png.nY,1);
    texture->data=png.data;
    texture->averageRed = png.averageRed;
    texture->averageGreen = png.averageGreen;
    texture->averageBlue = png.averageBlue;

  }
  else
  { return false; }
  
  
	return true;
}

/* New implementation for Texture class. */
/* NOTE: Try switching internal format from GL_RGBA8 to GL_RGBA4. Also try compression. */
// I'm switching to no compression, as it seems to cause FPS to halve on slower computers.
bool bindNearestNeighbour(Texture* texture, const bool compress=false)
{
	if(texture==0) {return false;}

  /* NOTE: It seems that the GLuint needs to be initialised (ie to 0) before being passed here, in some cases. */
  glGenTextures(1,&texture->textureID);
  glBindTexture(GL_TEXTURE_2D, texture->textureID);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  
  if (compress)
  {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
  }
  else
  {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
  }
	return true;
}

void unbind(Texture * texture) // free up graphics RAM
{
   glDeleteTextures(1, &texture->textureID);
}


/* New implementation for Texture class. */
/* NOTE: Try switching internal format from GL_RGBA8 to GL_RGBA4. Also try compression. */
bool loadTextureLinear(const std::string filePath, Texture* texture, const bool compress=false)
{

	if(texture!=0)
	{
		/* NOTE: It seems that the GLuint needs to be initialised (ie to 0) before being passed here, in some cases. */
		glGenTextures(1,&texture->textureID);
		glBindTexture(GL_TEXTURE_2D, texture->textureID);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		int fileSize;
		unsigned char* data = FileManager::getFile(filePath,&fileSize);
		if(data!=0)
		{
			Png png(true);
			png.load(data,fileSize);

			png.getAverageColour();
			texture->create(png.nX,png.nY,1);
			texture->data=png.data;
			texture->averageRed = png.averageRed;
			texture->averageGreen = png.averageGreen;
			texture->averageBlue = png.averageBlue;
         
         if (compress)
         {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
         }
         else
         {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
         }
         
		}
		else
		{ return false; }
		return true;

	}
	return false;
}

bool loadTextureRotate(const std::string filePath, Texture* tex0, Texture* tex90, Texture* tex180, Texture* tex270, const bool compress=false)
{ /* will rotate the image clockwise 90 degrees 4 times, and store each rotation as a texture */

	int fileSize;
	unsigned char* data = FileManager::getFile(filePath,&fileSize);

	if(data!=0)
	{
		Png png(true);
		png.load(data,fileSize);
		png.getAverageColour();

		tex0->create(png.nX,png.nY,1);
		tex0->data=png.data;
		tex0->averageRed = png.averageRed;
		tex0->averageGreen = png.averageGreen;
		tex0->averageBlue = png.averageBlue;
		
		Png png2(true);
		png2.copy(&png);
		png2.rotate90Clockwise();
		tex90->create(png2.nX,png2.nY,1);
		tex90->data=png2.data;
		tex90->averageRed = png2.averageRed;
		tex90->averageGreen = png2.averageGreen;
		tex90->averageBlue = png2.averageBlue;
		
		Png png3(true);
		png3.copy(&png2);
		png3.rotate90Clockwise();
		tex180->create(png3.nX,png3.nY,1);
		tex180->data=png3.data;
		tex180->averageRed = png3.averageRed;
		tex180->averageGreen = png3.averageGreen;
		tex180->averageBlue = png3.averageBlue;
		
		Png png4(true);
		png4.copy(&png3);
		png4.rotate90Clockwise();
		tex270->create(png4.nX,png4.nY,1);
		tex270->data=png4.data;
		tex270->averageRed = png4.averageRed;
		tex270->averageGreen = png4.averageGreen;
		tex270->averageBlue = png4.averageBlue;

		//tex90 = tex0->rotate90Clockwise();
		//tex180 = text90->rotate90Clockwise();
		//tex270 = tex180->rotate90Clockwise();

		
		glGenTextures(1,&tex0->textureID);
		glBindTexture(GL_TEXTURE_2D, tex0->textureID);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
      
      if (compress)
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, tex0->nX, tex0->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex0->data);
      }
      else
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex0->nX, tex0->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex0->data);
      }

		glGenTextures(1,&tex90->textureID);
		glBindTexture(GL_TEXTURE_2D, tex90->textureID);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

      if (compress)
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, tex90->nX, tex90->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex90->data);
      }
      else
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex90->nX, tex90->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex90->data);
      }
		
		glGenTextures(1,&tex180->textureID);
		glBindTexture(GL_TEXTURE_2D, tex180->textureID);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

      if (compress)
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, tex180->nX, tex180->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex180->data);
      }
      else
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex180->nX, tex180->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex180->data);
      }
		
		glGenTextures(1,&tex270->textureID);
		glBindTexture(GL_TEXTURE_2D, tex270->textureID);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

      if (compress)
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA, tex270->nX, tex270->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex270->data);
      }
      else
      {
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex270->nX, tex270->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex270->data);
      }
		

		return true;
	}
	return false;
}

bool loadTextureMipMapped(std::string basePath, int dimensions, GLuint* textureID) /* Deprecated */
{
	glGenTextures( 1, textureID );
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

	int i=0;
	while(true)
	{
		int fileSize;
		unsigned char* data = FileManager::getFile(basePath+"_"+DataTools::toString(dimensions)+"x"+DataTools::toString(dimensions),&fileSize);
		if(data!=0)
		{
			Texture texture;
			if(texture.load(data))
			{ glTexImage2D(GL_TEXTURE_2D, i++, GL_RGBA4, texture.nX, texture.nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		if(dimensions!=1)
		{
			dimensions/=2;
		}
		else
		{
			return true;
		}
	}
	return true;
}

bool loadTextureCropCenter(std::string filePath, const int croppedX, const int croppedY, GLuint* textureID)
{
	glGenTextures(1, textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	int fileSize;
	unsigned char* data = FileManager::getFile(filePath,&fileSize);

	if(data!=0)
	{
		Png png(true);
		png.load(data,fileSize);

		Texture* texture = new Texture;
		texture->create(png.nX,png.nY,1);
		texture->data=png.data;

		Texture* texture2 = new Texture;
		texture2=texture->cropCentered(croppedX,croppedY);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture2->nX, texture2->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture2->data);

	}
	return true;
}
bool loadTexture(std::string filePath, GLuint* textureID)
{
	glGenTextures(1, textureID);
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);


	int fileSize;
	unsigned char* data = FileManager::getFile(filePath,&fileSize);

	Png png(true);
	png.load(data,fileSize);

	Texture* texture = new Texture;
	texture->create(png.nX,png.nY,1);
	texture->data=png.data;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->nX, texture->nY, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
	return true;
}

#endif
