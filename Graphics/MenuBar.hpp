#ifndef GRAPHICS_MENUBAR_HPP
#define GRAPHICS_MENUBAR_HPP

#include <string>
#include <vector>

#include <Graphics/BufferInterface.hpp>

class MenuBar: public BufferInterface
{
	public:
	class Button
	{
		public:
		std::string text;
		Button()
		{
		}
		Button(std::string t)
		{
			text=t;
		}
		virtual void clicked()=0;
	};
	class Category
	{
		std::vector <Category> vSubCategory;
		std::vector <Button*> vButton;
		public:
		Category(){}
		void addButton(Button *button)
		{
			vButton.push_back(button);
		}
	};
	
	std::vector <Category> vCategory;
	MenuBar(){}
	void addCategory(Category category)
	{
		vCategory.push_back(category);
	}
	
	void buffer(ArrayS3 <unsigned char> &pixel)
	{
	}
	void mouseClick(int,int,int,int){}
	void mouseMove(int,int){}
};

#endif
