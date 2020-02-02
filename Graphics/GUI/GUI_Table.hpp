#pragma once
#ifndef WILDCAT_GUI_GUI_TABLE_HPP
#define WILDCAT_GUI_GUI_TABLE_HPP

/*
	#include <GUI/GUI_Table.hpp>
		TABLE. Allows the organised display of a large amount of data. I am thinking that a template and pointer design might be more efficient, but this system is at least easy to understand.
*/

#include <Graphics/GUI/GUI.hpp>
#include <Container/Table/Table.hpp>

class GUI_Table: public GUI_Interface
{
	private:
		Vector <std::string> vColumnHeading;
		Vector <int> vColumnWidth;
		Vector <std::string> vColumnQuery;
		
		int scrolledAmount; /* How many entries down we are scrolled. */
    
    // TextEntry: Filter input.
    GUI_TextEntry textEntryFilter;
    
    Colour colourNormal;
    Colour colourSelected;
    
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
	int lastClickedIndex;

	bool highlightSelectedRow;
	
	bool mousePanning;
	
	int tooltipX, tooltipY;
	
	GUI_Table() : GUI_Interface() /* super */
	{
		table=0;
		alpha=255;
		
		isAscending=false;
		lastSortedIndex=-1;
		
		clickedRow=false;
		lastRowClicked=-1;
		lastRowClickedOriginal=-1;
		
		lastClickedIndex = -1; /* Returns unsorted index */

		highlightSelectedRow=true;
		
		scrolledAmount=0;
		
		mousePanning=false;
		
		tooltipX=0;
		tooltipY=0;
    
		textEntryFilter.setColours(&colourNormal,&colourSelected);
		textEntryFilter.fieldName="Filter:";
		textEntryFilter.characterLimit=20;
		textEntryFilter.input = "";
    textEntryFilter.active=true;
    
	}
	
	void render()
	{
		if (active==false || table == 0)
		{ return; }

		Renderer::placeColour4a(150,150,150,alpha,panelX1,panelY1,panelX2,panelY2);
		
			// RENDER TABLE VIEW.
		
		int currentX=panelX1+2;
		for (int i=0;i<vColumnHeading.size();++i)
		{
			int currentY = panelY2-30;
			// if ( vColumnHeading.isSafe(i)==true)
			// {
					// // DRAW COLUMN HEADING.
			font8x8.drawText(vColumnHeading(i),panelX1+currentX,panelY2-2,panelX1+currentX+vColumnWidth(i),panelY2-12,false,true);
			// }
      
      currentY-=8;

				// DRAW COLUMN ENTRIES.
			for (int i2=scrolledAmount;i2<table->nRows() && currentY>panelY1;++i2)
			{
				//font8x8.drawText(table->get(i,i2),panelX1+currentX,currentY,panelX1+currentX+vColumnWidth(i),currentY+12);
        
        if ( textEntryFilter.input=="" || table->matchesFilter(textEntryFilter.input,&vColumnQuery,i2) )
        {
        
          font8x8.drawText(table->get(vColumnQuery(i),i2),panelX1+currentX,currentY,panelX1+currentX+vColumnWidth(i),currentY+12,false,true);
          currentY-=12;
        
        }
			}
			currentX+=vColumnWidth(i);
		}

		if ( lastRowClicked != -1 )
		{
				Renderer::placeColour4a(250,250,250,60,panelX1,(panelY2-38)-(12*lastRowClicked),panelX2,(panelY2-38)-(12*lastRowClicked)+12);
		}
		
		if ( getMaxRows() < table->nRows() )
		{
			
			Renderer::placeColour4a(100,100,100,255,panelX2-10,panelY1,panelX2,panelY2);
			double percentVisible = getMaxRows()/(double)table->nRows();
			
			//std::cout<<"max rows: "<<getMaxRows()<<".\n";
			double percentScrolled = ((scrolledAmount+getMaxRows())/((double)table->nRows()));
			//double percentScrolled = scrolledAmount/((double)table->nRows()-getMaxRows());
			//std::cout<<"percent scrolled: "<<percentScrolled<<".\n";
			
			int scrollBarHeight = panelNY*percentVisible;
			if ( scrollBarHeight <= 0 ) { scrollBarHeight=1; }
			int scrollBarDistance = panelNY*percentScrolled;
			//std::cout<<"scrollbardistance: "<<scrollBarDistance<<"\n";
			
			int _y1 = panelY2-scrollBarDistance;
			int _y2 = panelY2-(scrollBarDistance+scrollBarHeight);
			
			
			Renderer::placeColour4a(0,255,0,255,panelX2-10,panelY2-scrollBarDistance+scrollBarHeight,panelX2,panelY2-scrollBarDistance);
			
			//std::cout<<"starting coord: "<<panelY2-scrollBarDistance<<".\n";
			//std::cout<<"ending coord: "<<panelY2-(scrollBarDistance+scrollBarHeight)<<".\n";
			//std::cout<<"scrolled amount: "<<scrolledAmount<<".\n";

		}
		
		if (mousePanning && tooltipX > 0 && tooltipY > 0)
		{
			std::string strTooltip = DataTools::toString(scrolledAmount+1)+"-"+DataTools::toString(scrolledAmount+getMaxRows()+1)+"/"+DataTools::toString(table->nRows()+1);
			font8x8.drawText(strTooltip,tooltipX-160,tooltipY,tooltipX,tooltipY+12,true,true);
		}
    
    textEntryFilter.render();
	}
	
	void eventResize()
	{
		//std::cout<<"Table resized\n";
    textEntryFilter.setPanel(panelX1+20, panelY2-14, panelX2 -20, panelY2-24);

	}
	
	void clear()
	{
		vColumnHeading.clear();
		vColumnWidth.clear();
		vColumnQuery.clear();
		table=0;
		alpha=255;
		
		isAscending=false;
		lastSortedIndex=-1;
		
		clickedRow=false;
		lastRowClicked=-1;
		lastRowClickedOriginal=-1;
		
		lastClickedIndex = -1; /* Returns unsorted index */

		highlightSelectedRow=true;
		
		scrolledAmount=0;
	}
	
	bool mouseEvent (Mouse* _mouse)
	{
	
		if ( active==false || table == 0)
		{ return false; }
  
		if (_mouse->isLeftDown==false)
		{
			mousePanning=false;
		}
		
		if ( _mouse->isLeftClick==true || mousePanning )
		{
			
			// Check if scrollbar is clicked
			if ( getMaxRows() < table->nRows() && (mousePanning==true
			||
				(_mouse->y >= panelY1 && _mouse->y <= panelY2
				&& _mouse->x >= panelX2 - 10 && _mouse->x <= panelX2))
				)
			{
				//std::cout<<"scrollbar clicked\n";
				mousePanning = true;
				
				double percentVisible = getMaxRows()/(double)table->nRows();
				int scrollBarHeight = panelNY*percentVisible;
				
				double clickOffset = panelY2 - _mouse->y;
				clickOffset += scrollBarHeight/2;
				//std::cout<<"click offset: "<<clickOffset<<".\n";
				
				double percentClicked = clickOffset/panelNY;
				
				int rowClicked = (double)table->nRows() * percentClicked;
				
				if ( rowClicked > getMaxRows() )
				{
					scrolledAmount = rowClicked - getMaxRows();
					if ( scrolledAmount > table->nRows()-getMaxRows())
					{
						scrolledAmount = table->nRows()-getMaxRows();
					}
				}
				else
				{
					scrolledAmount = 0;
				}
				
				
				tooltipX=_mouse->x;
				tooltipY=_mouse->y;
				// Renderer::placeColour4a(100,100,100,255,panelX2-10,panelY1,panelX2,panelY2);

				
				// //std::cout<<"max rows: "<<getMaxRows()<<".\n";
				// double percentScrolled = ((scrolledAmount+getMaxRows())/((double)table->nRows()));
				// //double percentScrolled = scrolledAmount/((double)table->nRows()-getMaxRows());
				// std::cout<<"percent scrolled: "<<percentScrolled<<".\n";
				
				
				// int scrollBarDistance = panelNY*percentScrolled;
				// std::cout<<"scrollbardistance: "<<scrollBarDistance<<"\n";
				
				// int _y1 = panelY2-scrollBarDistance;
				// int _y2 = panelY2-(scrollBarDistance+scrollBarHeight);
				
				
				// Renderer::placeColour4a(0,255,0,255,panelX2-10,panelY2-scrollBarDistance+scrollBarHeight,panelX2,panelY2-scrollBarDistance);
				
				//std::cout<<"starting coord: "<<panelY2-scrollBarDistance<<".\n";
				//std::cout<<"ending coord: "<<panelY2-(scrollBarDistance+scrollBarHeight)<<".\n";
				//std::cout<<"scrolled amount: "<<scrolledAmount<<".\n";

			}
			
			if (mousePanning == false)
			{
        // Check if a column heading was clicked.
				int currentX=panelX1+2;
				for (int i=0;i<vColumnHeading.size();++i)
				{
					int currentY = panelY2-38;
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
					for (int i2=scrolledAmount;i2<table->nRows() && currentY>panelY1;++i2)
					{
						if ( _mouse->inBounds(panelX1+currentX,currentY,panelX1+currentX+vColumnWidth(i),currentY+12) )
						{
							clickedRow=true;
							lastRowClicked=i2-scrolledAmount;
							//lastRowClickedOriginal=i;
							//std::cout<<"Lastrowclicked: "<<i2<<".\n";
							lastClickedIndex = table->vID(i2);
							//std::cout<<"Last index: "<<lastClickedIndex<<".\n";
							//std::cout<<"Lastrowclickedoriginal: "<<i<<".\n";
						}
						currentY-=12;
					}
					currentX+=vColumnWidth(i);
				}
			}

      textEntryFilter.mouseEvent(_mouse);
      _mouse->isLeftClick=false;
		}
    
    if (_mouse->isRightClick )
    {
      // Check if a column heading was clicked.
      int currentX=panelX1+2;
      for (int i=0;i<vColumnHeading.size();++i)
      {
        int currentY = panelY2-30;
        if ( _mouse->inBounds(panelX1+currentX,panelY2-2,panelX1+currentX+vColumnWidth(i),panelY2-12) )
        {
          std::cout<<"FILTER\n";
          
          
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
        currentX+=vColumnWidth(i);
      }

    }
	
		return false;
	}
	
	
	bool keyboardEvent(Keyboard* _keyboard)
	{
		if ( active==true )
		{
			if(_keyboard->isPressed(Keyboard::UP))
			{
				highlightPrevious();
			}
			if(_keyboard->isPressed(Keyboard::DOWN))
			{
				highlightNext();
			}
			if (_keyboard->isPressed(Keyboard::PAGE_UP))
			{
				scrolledAmount-=getMaxRows();
				lastRowClicked=0;
				if ( scrolledAmount<0)
				{ 
					scrolledAmount=0;
				}
			}
			if (_keyboard->isPressed(Keyboard::PAGE_DOWN))
			{
				scrolledAmount+=getMaxRows();
				lastRowClicked=getMaxRows();
				
				
				while (lastRowClicked >= getMaxRows() )
				{
					--lastRowClicked;
					++scrolledAmount;
				}
				
				while ( scrolledAmount + getMaxRows() > table->nRows() )
				{
					--scrolledAmount;
				}
			}
      textEntryFilter.keyboardEvent(_keyboard);
		}
		return false;
	}
	
		// Return the maximum number of rows that can be displayed without scrolling.
	int getMaxRows()
	{
		int currentY = panelY2-30;
		int i=0;
		for (int i2=0;i2<table->nRows() && currentY>panelY1;++i2)
		{
			++i;
			currentY-=12;
		}
		return i;
	}
	
	void highlightNext()
	{
		++lastRowClicked;
		
		if ( lastRowClicked > table->nRows()-1 )
		{
			lastRowClicked = table->nRows()-1;
		}

			// Calculate how far down the page we are scrolled. Scroll table if we are at the bottom.
		while (lastRowClicked >= getMaxRows() )
		{
			--lastRowClicked;
			++scrolledAmount;
		}
		
		while ( scrolledAmount + getMaxRows() > table->nRows() )
		{
			--scrolledAmount;
		}
		//++scrolledAmount;
	}
	void highlightPrevious()
	{
		if ( lastRowClicked > 0 )
		{
			--lastRowClicked;
		}
		else
		{
			--scrolledAmount;
			if ( scrolledAmount<0)
			{ 
				scrolledAmount=0;
			}
		}

	}
  
  
	void setColours(const Colour* cNormal, const Colour* cSelected)
	{
		colourNormal.set(cNormal);
		colourSelected.set(cSelected);
    
    textEntryFilter.setColours(cNormal,cSelected);
	}
	
};

#endif
