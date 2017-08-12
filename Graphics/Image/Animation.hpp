#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <string>

#include <Time/Timer.hpp>

//Just whack this here so I don't forget.
class Animation
{
	public:
	std::vector <Image> frame;
	unsigned int activeFrame;
	
	unsigned int x;
	unsigned int y;
	
	double speed;
	Timer timer;
	
	Animation()
	{
		activeFrame=0;
		x=0;
		y=0;
		speed=0;
		timer.init();
		timer.start();
	}
	//Vector of images
	void flip()
	{
		if(activeFrame>=frame.size()-1)
		{
			activeFrame=0;
		}
		else
		{
			activeFrame++;
		}
	}
	void tick()
	{
		//Frames will play at the same speed no matter how rarely polled.
		timer.update();
		if(timer.fullSeconds>=speed)
		{
			while(timer.fullSeconds>=speed)
			{
				timer.fullSeconds-=speed;
				flip();
			}
			//timer.start();
		}
	}
	void addFrame(Image image)
	{
		frame.push_back(image);
	}
	void addFrame(std::string fileName)
	{
		Image i;
		i.loadBmp(fileName);
		frame.push_back(i);
	}
	
	//For testing only... normally we want an image handler to directly use this.
	void render(unsigned int x, unsigned int y)
	{
		glRasterPos2i(x, y);
		glDrawPixels(frame.at(activeFrame).width, frame.at(activeFrame).height, GL_RGB, GL_UNSIGNED_BYTE, frame.at(activeFrame).getStream());
		glFlush();
	}
	//Use this to get the bytestream of the image.
	BYTE * getStream()
	{
		return frame.at(activeFrame).getStream();
	}
	ArrayS3 <BYTE> getArray()
	{
		return frame.at(activeFrame).getArray();
	}
};

#endif
