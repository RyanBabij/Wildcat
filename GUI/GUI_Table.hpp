#pragma once
#ifndef WILDCAT_GUI_GUI_TABLE_HPP
#define WILDCAT_GUI_GUI_TABLE_HPP

/*
	#include <GUI/GUI_Table.hpp>
		TABLE. Allows the organised display of a large amount of data.
*/

#include <GUI/GUI.hpp>
#include <Container/Table/Table.hpp>

class GUI_Table: public GUI_Interface
{
	private:
		Vector <std::string> vColumnHeading;
		Vector <int> vColumnWidth;
		Vector <std::string> vColumnQuery;
	public:
	
	Table2* table;
	

	
	void addColumn(const std::string _columnHeading, const std::string _columnQuery, const int _columnWidth)
	{
		vColumnHeading.push(_columnHeading);
		vColumnQuery.push(_columnQuery);
		vColumnWidth.push(_columnWidth);
	}
	
	bool isAscending;
	int lastSortedIndex;
	
	int alpha;
	
		// SET TO TRUE IF THE USER HAS CLICKED A ROW ON THE TABLE.
	bool clickedRow;
		// SET TO THE SLOT OF THE LAST ROW THAT THE USER HAS CLICKED.
	int lastRowClicked;
	int lastRowClickedOriginal;

	bool highlightSelectedRow;
	
	GUI_Table() : GUI_Interface() /* super */
	{
		table=0;
		alpha=255;
		
		isAscending=false;
		lastSortedIndex=-1;
		
		clickedRow=false;
		lastRowClicked=-1;
		lastRowClickedOriginal=-1;

		highlightSelectedRow=true;
	}
	
	void render()
	{
		if (active==false)
		{
			return;
		}
		if ( table==0 )
		{
			return;
		}
		Renderer::placeColour4a(150,150,150,alpha,panelX1,panelY1,panelX2,panelY2);
		
			// RENDER TABLE VIEW.
		
		int currentX=panelX1+2;
		for (int i=0;i<vColumnHeading.size();++i)
		{
			int currentY = panelY2-30;
			// if ( vColumnHeading.isSafe(i)==true)
			// {
					// // DRAW COLUMN HEADING.
			font8x8.drawText(vColumnHeading(i),panelX1+currentX,panelY2-2,panelX1+currentX+vColumnWidth(i),panelY2-12);
			// }

				// DRAW COLUMN ENTRIES.
			for (int i2=0;i2<table->nRows() && currentY>panelY1;++i2)
			{
				//font8x8.drawText(table->get(i,i2),panelX1+currentX,currentY,panelX1+currentX+vColumnWidth(i),currentY+12);
				font8x8.drawText(table->get(vColumnQuery(i),i2),panelX1+currentX,currentY,panelX1+currentX+vColumnWidth(i),currentY+12);
				currentY-=12;
			}
			currentX+=vColumnWidth(i);
		}

		if ( lastRowClicked != -1 )
		{
				Renderer::placeColour4a(250,250,250,60,panelX1,(panelY2-30)-(12*lastRowClicked),panelX2,(panelY2-30)-(12*lastRowClicked)+12);
		}

	}
	
	bool mouseEvent (Mouse* _mouse)
	{
	
		if ( active==false)
		{
			return false;
		}
		if ( table==0 )
		{
			return false;
		}
		if ( _mouse->isLeftClick==true )
		{

			// RENDER TABLE VIEW.
		
		int currentX=panelX1+2;
		for (int i=0;i<vColumnHeading.size();++i)
		{
			int currentY = panelY2-30;
			if ( _mouse->inBounds(panelX1+currentX,panelY2-2,panelX1+currentX+vColumnWidth(i),panelY2-12) )
			{
				if ( i!=lastSortedIndex)
				{
					table->sortAscendingBy(vColumnQuery(i));
					isAscending=true;
				}
				else
				{
					if ( isAscending==true )
					{
						table->sortDescendingBy(vColumnQuery(i));
						isAscending=false;
					}
					else
					{
						table->sortAscendingBy(vColumnQuery(i));
						isAscending=true;
					}
				}
				lastSortedIndex=i;
			}
				// DRAW COLUMN ENTRIES.
			for (int i2=0;i2<table->nRows() && currentY>panelY1;++i2)
			{
				if ( _mouse->inBounds(panelX1+currentX,currentY,panelX1+currentX+vColumnWidth(i),currentY+12) )
				{
					clickedRow=true;
					lastRowClicked=i2;
					//lastRowClickedOriginal=i;
					std::cout<<"Lastrowclicked: "<<i2<<".\n";
					//std::cout<<"Lastrowclickedoriginal: "<<i<<".\n";
				}
				currentY-=12;
			}
			currentX+=vColumnWidth(i);
		}
		
		
		
		//vColumnHeading.push(_columnHeading);
		//vColumnQuery.push(_columnQuery);
		//vColumnWidth.push(_columnWidth);
				//std::cout<<"me\n";
			
			//int currentX=panelX1+2;
			//for (int i=0;i<vColumnHeading.size();++i)
			//{
			//	int currentY = panelY2-30;
				//if ( vColumnHeading.isSafe(i)==true)
				//{
						// DRAW COLUMN HEADING.
						
						// if ( _mouse->inBounds(panelX1+currentX,panelY2-2,panelX1+currentX+vColumnWidth(i),panelY2-12) )
						// {
							// //std::cout<<"Clicked column "<<i<<".\n";
							
							// if ( i!=lastSortedIndex)
							// {
								// //std::cout<<"Ascending.\n";
								// table->sortAscendingBy(i);
								// isAscending=true;
							// }
							// else
							// {
								// if ( isAscending==true )
								// {
									// //std::cout<<"Ascending.\n";
									// table->sortDescendingBy(i);
									// isAscending=false;
								// }
								// else
								// {
									// table->sortAscendingBy(i);
									// isAscending=true;
								// }
							// }
							
							// //table->sortAscendingBy(i);
							// lastSortedIndex=i;
							
						// }
						
					//font8x8.drawText(vColumnHeading(i),panelX1+currentX,panelY2-2,panelX1+currentX+100,panelY2-12);
				//}

				// DRAW COLUMN ENTRIES.
				// for (int i2=0;i2<table->nRows();++i2)
				// {
					// //font8x8.drawText(table->get(i,i2),panelX1+currentX,currentY,panelX1+currentX+100,currentY+12);
					// if ( _mouse->inBounds(panelX1+currentX,currentY,panelX1+currentX+vColumnWidth(i),currentY+12) )
					// {
						// //std::cout<<"Clicked row: "<<i2<<".\n";
						// clickedRow=true;
						// lastRowClicked=i2;
					// }
					// currentY-=12;
				// }
				// currentX+=vColumnWidth(i);
			//}
			
		}
	
		return false;
	}
};

#endif
