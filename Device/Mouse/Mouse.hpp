#ifndef DEVICE_MOUSE_HPP
#define DEVICE_MOUSE_HPP

class Mouse
{
	public:
	int x;
	int y;
	int lastX;
	int lastY;
	
	/* I should make it so that isLeftClick is only down for the first event. isLeftDown is down until mouse button is lifted. */
	
	bool isLeftClick;
	bool isRightClick;
	bool isMiddleClick;
	
	bool isLeftDown;
	
	bool isWheelUp;
	bool isWheelDown;
	
	Mouse()
	{
		isLeftClick=false;
		isRightClick=false;
		isMiddleClick=false;
		
		isLeftDown=false;
		
		isWheelUp=false;
		isWheelDown=false;
	}
	
	inline void move(int mouseX, int mouseY)
	{
		lastX=x;
		lastY=y;
		x=mouseX;
		y=mouseY;
	}
	
	void adjustRelative(int actualX, int actualY)
	{
		x-=actualX;
		y-=actualY;
		lastX-=actualX;
		lastY-=actualY;
		normalise();
	}
	void normalise()
	{
		if(x<0)
		{ x=0; }
		if(y<0)
		{ y=0; }
		if(lastX<0)
		{ lastX=0; }
		if(lastY<0)
		{ lastY=0; }
	}
	void printState()
	{
		std::cout<<"Mouse buttons clicked:\n";
		if(isLeftClick)
		{
			std::cout<<"Left\n";
		}
		if(isRightClick)
		{
			std::cout<<"Right\n";
		}
		if(isMiddleClick)
		{
			std::cout<<"Middle\n";
		}
		std::cout<<"Mouse location:\n" << x << ", " << y <<"\n";
		if (isWheelUp == true)
		{
			std::cout<<"Mousewheel up.\n";
		}
		else
		{
			std::cout<<"Mousewheel not up.\n";
		}
		if (isWheelDown == true)
		{
			std::cout<<"Mousewheel down.\n";
		}
		else
		{
			std::cout<<"Mousewheel not down.\n";
		}
	}
	bool inBounds(int _x1, int _y1, int _x2, int _y2)
	{
		// 0230734790: MAKE SURE THE COORDS ARE IN THE RIGHT ORDER.
	
		if ( _x1 > _x2 )
		{
			int _temp = _x1;
			_x1=_x2;
			_x2=_temp;
		}
		if ( _y1 > _y2 )
		{
			int _temp = _y1;
			_y1=_y2;
			_y2=_temp;
		}
	
		if(x>=_x1&&x<=_x2&&y>=_y1&&y<=_y2)
		{ return true; }
		return false;
	}
};

#endif
