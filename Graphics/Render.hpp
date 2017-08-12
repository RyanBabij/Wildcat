#ifndef GRAPHICS_RENDERER_OPENGL_HPP
#define GRAPHICS_RENDERER_OPENGL_HPP

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <vector>

#include <Container/ArrayS3/ArrayS3.hpp>
#include <Math/Operation.hpp>

#include <Graphics/Buffer.hpp>


#include <Data/Font.hpp>
#include <Misc/TempFont.hpp>

#include <Device/Keyboard.hpp>
#include <Device/Mouse.hpp>

class OpenGL
{
	private:
	int fakeX, fakeY;
	bool flushPlease;
	unsigned int yOffset;
	
	unsigned int nX,nY;
	
	public:
	
	Keyboard* keyboard;
	Mouse* mouse;
	Font* font;
	
	ArrayS3 <unsigned char>* pixel;
	
	std::vector <Buffer*> buffer;
	
	OpenGL()
	{
		nX=0;
		nY=0;
		
		fakeX=1;
		fakeY=1;
		yOffset=0;
		flushPlease=false;
		
		keyboard=0;
		mouse=0;
		font=0;
	}
	
	void addBuffer(Buffer* b)
	{
		buffer.push_back(b);
		b->setKeyboard(keyboard);
		b->setMouse(mouse);
		b->setFont(font);
		b->setPixelMap(pixel);
	}
	inline void init(int numArguments, char ** argument)
	{
		glutInit(&numArguments, argument);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
		glutInitWindowSize(fakeX, fakeY);
		glutInitWindowPosition(0, 0);
		glutCreateWindow("Whee");
		glClearColor(0,0,0,0);
	}
	void set2DMode(int x1, int x2, int y1, int y2)
	{ gluOrtho2D(x1,x2,y1,y2); }
	void render()
	{
		//glClear(GL_COLOR_BUFFER_BIT);
		for(std::vector<Buffer*>::iterator i=buffer.begin();i!=buffer.end();++i)
		{
			(*i)->buffer();
		}
		//glFlush();
		//glutPostRedisplay();
	}
	inline void mainLoop()
	{ glutMainLoop(); }
	inline void reshape(int sX, int sY)
	{
		nX=sX;
		nY=sY;
		glutSetWindow(glutGetWindow());
		glutReshapeWindow(nX, nY);
		glViewport(0, 0, nX, nY);
		
		fakeX = Operation::nearestPowerOf2(nX);
		fakeY = Operation::nearestPowerOf2(nY);

		glClearColor(0,0,0,0);
	}
	inline void mouseClick (int clickType,int state,int mouseX,int mouseY)
	{
		//We use a different coordinate system from OpenGL, so we convert it here.
		//I'm guessing Opengl uses bottom left origin, and we're using a top left origin, cos the window is resized from the bottom right.
		mouseY=nY-mouseY-yOffset;
		//Translate glut defines into the mouse object variables.
		if(state==GLUT_DOWN)
		{
			if(clickType==GLUT_LEFT_BUTTON) { mouse->isLeftClick=true; }
			else if(clickType==GLUT_MIDDLE_BUTTON) { mouse->isMiddleClick=true; }
			else if(clickType==GLUT_RIGHT_BUTTON) { mouse->isRightClick=true; }
		}
		else if(state==GLUT_UP)
		{
			if(clickType==GLUT_LEFT_BUTTON) { mouse->isLeftClick=false; }
			else if(clickType==GLUT_MIDDLE_BUTTON) { mouse->isMiddleClick=false; }
			else if(clickType==GLUT_RIGHT_BUTTON) { mouse->isRightClick=false; }
		}
		//Remove negative mouse values.
		if(mouseX<0) { mouseX=0; }
		if(mouseY<0) { mouseY=0; }
		//Update Mouse
		mouse->move(mouseX,mouseY);
		for(std::vector<Buffer*>::iterator i=buffer.begin();i!=buffer.end();++i)
		{
			(*i)->mouseClick();
		}
	} 
	inline void mouseMove(int mouseX, int mouseY)
	{
		mouseY=nY-mouseY-yOffset;
		mouse->move(mouseX,mouseY);
		for(std::vector<Buffer*>::iterator i=buffer.begin();i!=buffer.end();++i)
		{ (*i)->mouseMove(); }
	}
	void mouseWheel (int wheel, int direction, int x, int y)
	{
		if(direction==1)
		{ mouse->isWheelUp=true; }
		else if(direction==-1)
		{ mouse->isWheelDown=true; }
		for(std::vector<Buffer*>::iterator i=buffer.begin();i!=buffer.end();++i)
		{ (*i)->mouseClick(); }
		mouse->isWheelUp=false;
		mouse->isWheelDown=false;
	}
	void keyPress(unsigned char key, int mouseX, int mouseY)
	{
		keyboard->keyWasPressed=true;
		keyboard->keyWasUnpressed=false;
	
		mouseMove(mouseX, mouseY);
		keyboard->keyDown(key);
		for(std::vector<Buffer*>::iterator i=buffer.begin();i!=buffer.end();++i)
		{
			(*i)->keyboardEvent();
		}
		flushPlease=true;
	}
	void keyUp(unsigned char key, int mouseX, int mouseY)
	{
		keyboard->keyWasPressed=false;
		keyboard->keyWasUnpressed=true;
	
		mouseMove(mouseX, mouseY);
		keyboard->keyUp(key);
		for(std::vector<Buffer*>::iterator i=buffer.begin();i!=buffer.end();++i)
		{
			(*i)->keyboardEvent();
		}
		flushPlease=true;
	}
	void specialKeyPress(unsigned char key, int mouseX, int mouseY)
	{
		keyboard->keyWasPressed=true;
		keyboard->keyWasUnpressed=false;
	
		mouseMove(mouseX, mouseY);
		keyboard->specialKeyDown(key);
		for(std::vector<Buffer*>::iterator i=buffer.begin();i!=buffer.end();++i)
		{
			(*i)->keyboardEvent();
		}
		flushPlease=true;
	}
	void specialKeyUp(unsigned char key, int mouseX, int mouseY)
	{
		keyboard->keyWasPressed=false;
		keyboard->keyWasUnpressed=true;
	
		mouseMove(mouseX, mouseY);
		keyboard->specialKeyUp(key);
		for(std::vector<Buffer*>::iterator i=buffer.begin();i!=buffer.end();++i)
		{
			(*i)->keyboardEvent();
		}
		flushPlease=true;
	}
};

#endif
