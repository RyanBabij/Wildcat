#pragma once
#ifndef WILDCAT_BOARD_VIEWER_BOARD_VIEWER_HPP
#define WILDCAT_BOARD_VIEWER_BOARD_VIEWER_HPP

#include <Container/ArrayS2/ArrayS2.hpp>
#include <Device/Display/DisplayInterface.hpp>
#include <Device/Mouse/MouseInterface.hpp>
#include <Render/Renderer.hpp>
#include <Interface/HasTexture.hpp>

/*
	#include <BoardViewer/BoardViewer.hpp>

	Code for rendering a generic tile-based 2D 'board'. The board is populated with objects that have a texture attached to them. The board is the visual interface through which the user may see the world. It also takes mouse input for panning the view, zooming, and also to determine if the player has clicked a tile.

	This class is useful because many of my games use this 'board' approach to rendering the world. However, there are performance issues in some cases, such as rendering a million tiles when the player is zoomed out very far. I am thinking about a solution for this.

	GridMode also suffers here. Instead of manually drawing a grid texture over each tile, we need to overlay a single grid texture over the while board. This will require dynamic texture creation at runtime, which I feel is the next step to improving performance. A tile based game will not update fluidly, but over discrete turns, so when a turn takes place, the board can be pre-rendered and then only that texture needs to be displayed for the next 30 frames or so. The only issue is zooming and panning over this pre-rendered texture.

	Okay... So I'm going to use the following principle...
	All of the objects in the world get a texture to render them with. Therefore, we can simply rip the world's object array, and use that for the texture array. Whenever an object in the 'world' is moved, the texture will move accordingly.
	Basically... Here we will use 'HasTexture' objects or something like that. The world will use 'WorldObject's, but WorldObjects will use the 'HasTexture' interface. So we can simply point to the WorldObject array from here.

	I think we need 2 implementations... One that does textures, and another that does colours. Although, tbh, you could treat the colour one as a single giant texture...

	Another issue is handling z levels. Some games might not want z levels.

	UPDATE: Actually this should be possible to handle if we are willing to accept a hard limit on the number of z levels. For example, a maximum of 6 would mean we can just create 6 seperate board layer arrays. However in the end a simple 3D array or vectors would maybe make more sense. Getting this to store well in memory may be an issue, depending on how that is handled... We might want to reserve space on the vectors in this case, though that is typically done automatically. Perhaps I ought to trust the compiler on this. Currently there is unlimited stacking. It may be worthwhile changing that. Maybe...

	The board viewer doesn't handle any movement of objects... The class above it should do that. The board viewer only deals with the view of the board at the given situation. The class that handles logic within the world is typically called 'World'.

	It seems that switching between colour and texture rendering can be very slow.

	CHANGELOG:

	0240752985: There was a bug in hovered tile calculations. I also moved it to mouseevent(). I'm not sure why it was in render() in the first place.

	0231714074: OVERHAULING THE CODE FOR LIKE THE 10TH TIME.

	ISSUES:

	THE MOST ANNOYING ISSUE HAS BEEN THAT THE CENTER TILES ARE NOT NECESSARILY IN THE CENTER OF THE SCREEN.

	CENTER Y IS OFFSET. I BELIEVE THIS HAS SOMETHING TO DO WITH THE TITLEBAR. I'M NOT 100% SURE... I BELIEVE THE CODE IS NOT 100% RELIABLE... BUT IT'S GOOD ENOUGH FOR NOW.

	BUT DON'T FORGET... WE USE FRACTIONS OF A TILE. IF I ASK TO CENTER ON (0,0)... THE BOTTOM LEFT CORNER OF TILE (0,0) WILL BE IN THE CENTER. CENTER ON (0.5,0.5) INSTEAD.


	New plan:

	Template approach is good. Things get complex when we want support for a vector on each tile, as well as just an individual object on each tile. This will require specialisation for functions that depend on this.

	I'll have to look into template specialisation to see if this is possible.

	Well instead of templates, we might try to genericise the container... In this case the vector/dummy vector. We can just have a generic List* thingy. A single entry can be passed in as a dummy list, with a hardcoded size of 1. This might invoke compiler optimisation, which could strip out the vector-specific code... Or it might not. Right now I might do better to just go with vectors of size 1. After all... We might later want stuff drawn over the world tiles... And that's when the vectors make life easy.

	So, we have a 3D array of Vectors. This is painful if we have a world that is only a 2D array of tiles... Here's an example:

			// THE FORMAT THAT BOARDVIEWER EXPECTS:
		ArrayS3 <Vector <HasXY* > * > aObjects;
			// NO LAYERS. NULL VECTOR POINTER.
		aObjects.init(_size,_size,1,0);
			// CREATE THE POINTERS.
		FOR_EACH_XY( aObjects,
			aObjects(_x,_y,0)=new Vector <HasXY*>;
		);

	So we've created a 3D array of dimensions (_x,_y,1). This is obviously a complicated way of creating a 2D array. There might be some elegant ways of solving this, but for now I will put up with this for the sake of simplicity. Additionally, we might hope that the compiler can optimise some of this nonsense away.

	We might create a specialisation of the ArrayS3, which is kind of an ArrayS2 squeezed into the ArrayS3 framework. Another option is creating some kind of system which allows 2D and 3D arrays to be used the same way. For example, we could allow a 2D array to be accessed using an x,y,z, by just throwing away the z. We could allow a 3D array to be accessed using x,y by assuming a z of 1.


	Render needs optimisation, since it seems to be a significant bottleneck. Could be achieved by stripping out Render:: stuff and replacing it with OpenGL code. Additionally the use of a texture atlas and mipmapping should help.

	UPDATE: There are two optimisation routes that seem reasonable from here. The one that will preserve the original plan will be using a texture atlas. This could greatly improve the amount of tiles we can render at once. However, another option is to render all the terrain as a single texture, and then render dynamic things on top of it. This deviates from the original plan, but might be considered the sane option. I'm leaning toward the latter right now. A combination of the two would also be possible.

	Since we have already had to introduce a distinction for objects which can be rendered at increased size (important objects that the player needs to be aware of at all zoom-levels)... Then really it seems that we should gravitate to the solution of making terrain its own layer, and then giving the objects a different set of rules.

	In addition we could probably break the board up into further distinctions, but that might be best left for 'World' to handle. But at the very least, we can have:
		* Terrain - Rendered first and as a single texture. Rendered at the given size. An interesting possibility is to use hq2x scaling on it to make it less pixelated.
		* Objects - Rendered as a array of vectors (stacks). Stack ordering is an issue to think about. Can be rendered at magnified size.
		* Overlays - Intangible tiles which are for interface purposes.

	Further distinction could be made between static objects... Things like walls, floors and trees. Stuff that rarely (or never) moves. These are overlaid on terrain. Statics will usually block line of sight to some extent, and could also block movement of actors. Statics may not normally stack, which can cut down on stacking complexity.

	Then there could be 'items'. Which can move, but don't move very often. They also tend to be small. Items don't normally block line of sight.

	Then there could be 'actors'. Things that move a lot. They are overlaid after items, and before overlays. There could be a limit on how many actors can be on a layer at once. Actors will usually perform pathfinding.

	After overlays, GUIs are typically overlaid, but this is not handled by the Board Viewer.

	So the original plan was to treat everything like a tile. This was seen to be too complex. It's like trying to design a game where every atom is modelled. Every game needs to make an abstraction on some level. The most fundamental abstraction is what I have just outlined. This would be good in an article. "Fundamentals of Game World Abstraction".

	To make a good definition of abstraction, I could start with a comparison between chess and warfare. Even supercomputers must abstract the world to some extent. Flight models could be covered briefly.

	023-251-4343: Did some datatype optimisation. No huge improvements. Texture atlas will probably have the biggest impact.

	023-258-6544: Finally fixed bug with centering. Seems to work fine now. No problems with Y centering... No need to introduce the Y_OFFSET value. Also a few other code optimisations.
*/

	// FIXES A STRANGE ISSUE WITH THE CENTER Y BEING OFFSET. I BELIEVE IT MAY HAVE SOMETHING TO DO WITH THE TITLE BAR BEING INCLUDED IN THE WINDOW SIZE CALCULATIONS... BUT I'M NOT SURE... FOR NOW I'M JUST GONNA USE THIS FIX.
const int Y_OFFSET_FIX_VALUE = 60;


class RenderLast
{
	public:
	
	int x1,y1,x2,y2;

	int tex;

	RenderLast ( const int _x1, const int _y1, const int _x2, const int _y2, const int _tex )
	{
		x1=_x1;
		y1=_y1;
		x2=_x2;
		y2=_y2;
		tex=_tex;
	}

	void render()
	{
		glBindTexture( GL_TEXTURE_2D, tex);
		glBegin(GL_QUADS);
			glTexCoord2s(0,1);
			glVertex2s(x1,y1);
			glTexCoord2s(1,1);
			glVertex2s(x2,y1);
			glTexCoord2s(1,0);
			glVertex2s(x2,y2);
			glTexCoord2s(0,0);
			glVertex2s(x1,y2);
		glEnd();
	}
};

	// SOMETHING DRAWN OVER THE TILES.
// class Overlay: public HasTexture
// {
	// public:
	// int tileX1, tileX2, tileY1, tileY2;

	// Texture * overlayTexture;

	// Overlay()
	// {
		// tileX1=0;
		// tileY1=0;
		// tileX2=0;
		// tileY2=0;
		// overlayTexture=0;
	// }

	// Texture* currentTexture() { return overlayTexture; }
// };

/* Made it a GUI class, since it now functions in GUIs. */
template <class T>
class BoardViewer: public DisplayInterface, public MouseInterface
{
	public:

		// THE BOARD OF TEXTURES TO BE RENDERED.

		// THE T SHOULD IMPLEMENT HASTEXTURE.
	ArrayS3 < Vector < T > * > *  aBoard;
	//ArrayS3 < HasTexture* > *  aBoard;
		// TRUE IF WE SHOULD NOT DRAW THE TILE.

	//Vector <Overlay*> vOverlay;


		// HERE IS THE NEW WAY THAT OVERLAYS WILL BE DONE. A 2 DIMESIONAL ARRAY TO ALLOW FOR QUICK UPDATES. IT'S BASICALLY LIKE HOW FOG OF WAR IS HANDLED.
		// OVERLAYS AND OTHER BOARD ARRAYS ARE POINTERS TO ALLOW FOR FAST DELETING, SWITCHING, ETC.
	ArrayS2 < HasTexture* > * aOverlay;
		// OVERLAYS ARE DISABLED BY DEFAULT.
	bool overlayActive;


		// CONSIDERING MULTIPLE SETTINGS: 0=NOT VISIBLE AT ALL. 1=TERRAIN VISIBLE. 2=EVERYTHING VISIBLE.
	ArrayS2 <char> * aFogOfWar;

	bool drawFog;


		// SHOULD BE POSSIBLE TO HAVE MULTIPLE OVERLAYS, THEREFORE THERE MUST BE AN OVERLAY OBJECT.
		// OVERLAYS
	bool squareOverlay;
		Texture * squareOverlayTexture;
		int squareOverlayRange;




	bool active;

	int tileSize; /* How big to render the tiles, in pixels. */
	double centerTileX, centerTileY; /* The tile at the center of the screen. */

		/* Store the last position of the mouse, so we can figure out which tile the mouse is on. */
	int lastMouseX, lastMouseY;


	/* Rendering coordinates. */
	int mainViewX1, mainViewX2, mainViewY1, mainViewY2;
	int mainViewNX, mainViewNY;

	bool panning;

	bool mousePanning; // TRUE IF THE PLAYER IS PANNING BY HOLDING DOWN THE MOUSE WHEEL.
	bool shiftPanning; // TRUE IF THE PLAYER IS PANNING BY HOLDING DOWN SHIFT.

	double panX, panY; /* Stores the last mouse position when panning. Needs to be a double. */

	//bool alternateTile;

		/* The current tile that the mouse is hovering over. Set to -1 if the mouse is not hovering over a tile. */
	int hoveredXTile, hoveredYTile;

	bool leftClickedTile;
	int leftClickedX, leftClickedY;

	bool gridMode;

	int MIN_ZOOM, MAX_ZOOM;

	BoardViewer()
	{
		active=false;

		overlayActive=false;

		squareOverlay=false;
		squareOverlayTexture=0;
		squareOverlayRange=0;

		tileSize=64;
		centerTileX=0; centerTileY=0;
		mainViewX1=0; mainViewX2=0;
		mainViewY1=0; mainViewY2=0;
		mainViewNX=0; mainViewNY=0;


		panning=false;
		shiftPanning=false;
		mousePanning=false;
		panX=0; panY=0;
		//alternateTile=0;

		lastMouseX=0;
		lastMouseY=0;

		hoveredXTile=0; hoveredYTile=0;

		leftClickedTile = false;
		leftClickedX = 0; leftClickedY = 0;

		drawFog=false;

			// NULL THE ARRAYS SO WE CAN NOT DRAW THEM IF WE DON'T NEED THEM.
		aBoard=0;
		aFogOfWar=0;
		aOverlay=0;


		gridMode=false;

		MIN_ZOOM = 1;
		MAX_ZOOM = 16;
		normaliseZoom();

	}

	void toggleGrid()
	{
		gridMode=!gridMode;
	}

		/* Keep the map visible on the screen so the player can't accidentally move it off screen and lose it. */
	inline void normaliseCoordinates()
	{
		if ( aBoard==0 || active==false ) { return; }

		if ( centerTileX < 0 )
		{ centerTileX=0; }
		else if ( centerTileX > aBoard->nX-1 )
		{ centerTileX = aBoard->nX-1; }

		if ( centerTileY < 0 )
		{ centerTileY=0; }
		else if ( centerTileY > aBoard->nY-1 )
		{ centerTileY = aBoard->nY-1; }
	}

	bool mouseEvent ( Mouse* mouse )
	{
		if ( mouse==0 || aBoard==0 || active==false ) { return false; }

		if (tileSize < MIN_ZOOM ) { tileSize = MIN_ZOOM; }
		if (tileSize > MAX_ZOOM ) { tileSize = MAX_ZOOM; }

//		if ( world==0 ) { return false; }

		lastMouseX=mouse->x;
		lastMouseY=mouse->y;


		// UPDATE HOVERED TILE COORDS.
		// MAKE SURE MOUSE IS WITHIN THE RENDERING VIEW
		if (lastMouseX >= mainViewX1 && lastMouseX <= mainViewX2 && lastMouseY >= mainViewY1 && lastMouseY <= mainViewY2 )
		{

			const float centerTileXMantissa = centerTileX - (int)centerTileX;
			const float centerTileYMantissa = centerTileY - (int)centerTileY;

			short int tileX=centerTileX;
			short int pixelTile = mainViewNX/2-(tileSize*centerTileXMantissa);

			// 0231745019: Changed >= to >.
			while(pixelTile>0)
			{
				--tileX;
				pixelTile-=tileSize;
			}
			const float pixelOffsetX = pixelTile;

			const short int revertTileX = tileX;

			short int tileY=centerTileY;
			// THIS IS THE OLD ONE WITH THE Y_OFFSET FIX.
			//pixelTile = mainViewNY/2 - Y_OFFSET_FIX_VALUE;
			pixelTile = mainViewNY/2-(tileSize*centerTileYMantissa);

			// 0231745019: Changed >= to >.
			while(pixelTile>0)
			{
				--tileY;
				pixelTile-=tileSize;
			}
			const float pixelOffsetY = pixelTile;



			const short int startingPX = mainViewX1 + pixelOffsetX;
			const short int startingPY = mainViewY1 + pixelOffsetY;

			const short int diffX = (lastMouseX - startingPX)/tileSize;
			const short int diffY = (lastMouseY - startingPY)/tileSize;

				// SET HOVERED TILE AS FIRST TILE DRAWN + PIXEL OFFSET.
			hoveredXTile = tileX + diffX;
			hoveredYTile = tileY + diffY;
		}
		else
		{
				// SET TO OUT OF BOUNDS VALUE.
			hoveredXTile=-1;
			hoveredYTile=-1;
		}

		// PANNING CODE
		// IF THE PLAYER IS HOLDING DOWN THE MOUSE WHEEL BUTTON, THEN THEY ARE PANNING.
		if(mouse->isMiddleClick==false)
		{
			mousePanning=false;
		}
		else if ( mouse->isMiddleClick)
		{
			mousePanning=true;
		}

			// IF THE PLAYER HAS JUST STARTED PANNING, THEN RESET PANNING COORDINATES.
		if (panning == false && (shiftPanning==true || mousePanning==true) )
		{
				panning=true;
				/* Set initial pan coordinates. */
				panX=mouse->x;
				panY=mouse->y;
		}
			// IF THE PLAYER IS NO LONGER HOLDING DOWN ANY PANNING BUTTON, STOP PANNING.
		else if ( panning == true && (shiftPanning==false && mousePanning==false) )
		{
			panning=false;
		}

			// HANDLE MOVEMENT WHILE PANNING.
		if(panning==true)
		{
			/* The view can be panned by holding down the middle mouse button
				and moving the mouse to drag the map. */
			centerTileX+=(panX-mouse->x)/tileSize;
			centerTileY+=(panY-mouse->y)/tileSize;
			panX=mouse->x;
			panY=mouse->y;
		}

		normaliseCoordinates();

		if ( mouse->isLeftClick == true )
		{
			if ( aBoard->isSafe(hoveredXTile,hoveredYTile,0) == true )
			{
				leftClickedTile = true;
				leftClickedX = hoveredXTile;
				leftClickedY = hoveredYTile;
			}

		}

		if(mouse->isWheelDown==true)
		{
			zoomOut();
			mouse->isWheelDown=false;
		}
		if(mouse->isWheelUp==true)
		{
			zoomIn();
			mouse->isWheelUp=false;
		}

		return false;
	}

	void setPanel( const int _x1, const int _y1, const int _x2, const int _y2)
	{
		mainViewX1 = _x1;
		mainViewY1 = _y1;
		mainViewX2 = _x2;
		mainViewY2 = _y2;

		mainViewNX = mainViewX2-mainViewX1;
		mainViewNY = mainViewY2-mainViewY1;
	}

	void setCenterTile (const double _centerTileX, const double _centerTileY)
	{
		centerTileX = _centerTileX;
		centerTileY = _centerTileY;
	}

	void centerOn (HasXY* _object, const int xOffset=0, const int yOffset=0)
	{
		centerTileX=(double)_object->x+0.5;
		centerTileY=(double)_object->y+0.5;
	}

		/* Convert tile coordinates to screen coordinates. */
	void toScreenCoords(const int _tileX, const int _tileY, int * _pixelX, int * _pixelY)
	{
			/* CONFUSING STUFF WHICH IS HARD TO EXPLAIN. */

		double centerTileXDecimal = centerTileX - (int)centerTileX;
		double centerTileYDecimal = centerTileY - (int)centerTileY;

		double pixelOffsetX = tileSize*centerTileXDecimal;
		double pixelOffsetY = tileSize*centerTileYDecimal;

		const int iCenterTileX = centerTileX;

		const int tilesToDrawX = (mainViewNX/tileSize) + 1;
		const int tilesToDrawY = (mainViewNY/tileSize) + 1;

		int tileX=centerTileX;
		int pixelTile = mainViewNX/2;

		// 0231745019: Changed >= to >.
		while(pixelTile>0)
		{
			--tileX;
			pixelTile-=tileSize;
		}
		const int revertTileX = tileX;

		int tileY=centerTileY;
		pixelTile = mainViewNY/2 - Y_OFFSET_FIX_VALUE;

		// 0231745019: Changed >= to >.
		while(pixelTile>0)
		{
			--tileY;
			pixelTile-=tileSize;
		}

		int nExtraXTiles = _tileX - tileX;
		int nExtraYTiles = _tileY - tileY;

		// 0231745019: Changed >= to >.
		if ( nExtraXTiles>0 && nExtraYTiles>0 )
		{
			*_pixelX = -pixelOffsetX + (tileSize * nExtraXTiles);
			*_pixelY = -pixelOffsetY + (tileSize * nExtraYTiles);
		}
		else
		{
			*_pixelX = -1000;
			*_pixelY = -1000;
		}
	}

	void render()
	{

			// ABORT CONDITIONS.
		if ( aBoard==0 || active==false || tileSize <= 0 ) { return; }

		normaliseZoom();

		// DRAW A REFERENCE CENTER POINT.
		const int centerX = mainViewNX/2;
		const int centerY = mainViewNY/2;

		const int _x1 = centerX;
		const int _x2 = centerX + 100;
		const int _y1 = centerY;
		const int _y2 = centerY + 100;

		// CHANGE THE DRAWING AREA TO PREVENT GRAPHICS SPILLING OUT OF THE DESIGNATED AREA.
		Renderer::saveViewPort();
		Renderer::resizeViewPort(mainViewX1,mainViewY1,mainViewX2,mainViewY2);

		const float centerTileXMantissa = centerTileX - (int)centerTileX;
		const float centerTileYMantissa = centerTileY - (int)centerTileY;

		short int tileX=centerTileX;
		short int pixelTile = mainViewNX/2-(tileSize*centerTileXMantissa);

		// 0231745019: Changed >= to >.
		while(pixelTile>0)
		{
			--tileX;
			pixelTile-=tileSize;
		}
		const float pixelOffsetX = pixelTile;

		const short int revertTileX = tileX;

		short int tileY=centerTileY;
			// THIS IS THE OLD ONE WITH THE Y_OFFSET FIX.
		//pixelTile = mainViewNY/2 - Y_OFFSET_FIX_VALUE;
		pixelTile = mainViewNY/2-(tileSize*centerTileYMantissa);

		// 0231745019: Changed >= to >.
		while(pixelTile>0)
		{
			--tileY;
			pixelTile-=tileSize;
		}
		const float pixelOffsetY = pixelTile;

			/* Pixel coords for leftmost tile. */
		Renderer::setTextureMode();

		const short unsigned int aBoardNZ = aBoard->nZ;

		Vector <RenderLast*> vLast;

		//for (short int currentY = -pixelOffsetY; currentY<mainViewNY; currentY+=tileSize)
		for (short int currentY = pixelOffsetY; currentY <= mainViewY2; currentY+=tileSize)
		{
			//for (short int currentX = pixelOffsetX; currentX<mainViewNX+tileSize; currentX+=tileSize)
			for (short int currentX = pixelOffsetX; currentX <= mainViewX2; currentX+=tileSize)
			{
					//std::cout<<"\n";
						// OUT OF BOUNDS/NULL RENDER
					if ( aBoard->isSafe(tileX,tileY,0)==false || (*aBoard)(tileX,tileY,0)==0 )
					{
							// THIS APPROACH IS SLOW FOR SOME REASON.

						//std::cout<<"1\n";
						//Renderer::setColourMode();
						//Renderer::placeColour4a(0,0,0,0,currentX,currentY,currentX+tileSize,currentY+tileSize);
						//Renderer::setTextureMode();
					}
						// FOG OF WAR RENDER.

						// IF THERE IS FOG, DRAW BLACKNESS.
					else if ( aFogOfWar!=0 && aFogOfWar->isSafe(tileX,tileY)==true && (*aFogOfWar)(tileX,tileY) == 0 )
					{
						//std::cout<<"2\n";
						// FOG OF WAR
						Renderer::setColourMode();
						Renderer::placeColour4a(0,0,0,0,currentX,currentY,currentX+tileSize,currentY+tileSize);
						Renderer::setTextureMode();

						//Renderer::placeTexture4(currentX,currentY,currentX+tileSize,currentY+tileSize,&TEX_COLOUR_GREY,false);
					}
						// NORMAL RENDER
					else
					{
						//std::cout<<"3\n";
						for ( unsigned short int _z=0;_z<aBoardNZ;++_z)
						{
							if ( (*aBoard)(tileX,tileY,_z)!=0 )
							{
								// NOT SURE IF CONSTING THIS DOES ANYTHING. NEED TO LOOK IT UP.
								const short unsigned int nSlots=(*aBoard)(tileX,tileY,_z)->size();
								//std::cout<<nSlots<<"\n";
								for ( short unsigned int i=0;i<nSlots;++i )
								{
									// ONLY RENDER SOMETHING IF IT HAS A TEXTURE.
									if ( (*(*aBoard)(tileX,tileY,_z))(i)->currentTexture()!=0 )
									{
										//Renderer::placeTexture4(currentX,currentY,currentX+tileSize,currentY+tileSize, (*(*aBoard)(tileX,tileY,_z))(i),false);

										//INLINED CODE FOR PERFORMANCE
										glBindTexture( GL_TEXTURE_2D, (*(*aBoard)(tileX,tileY,_z))(i)->currentTexture()->textureID);

										if ( (*(*aBoard)(tileX,tileY,_z))(i)->getMinSize() > tileSize )
										{
											const int offset = (*(*aBoard)(tileX,tileY,_z))(i)->getMinSize() / 2;
											vLast.push(new RenderLast(currentX-offset,currentY-offset,currentX+offset,currentY+offset,(*(*aBoard)(tileX,tileY,_z))(i)->currentTexture()->textureID));
											
											// glBegin(GL_QUADS);
												// glTexCoord2s(0,1);
												// glVertex2s(currentX-offset,currentY-offset);
												// glTexCoord2s(1,1);
												// glVertex2s(currentX+offset,currentY-offset);
												// glTexCoord2s(1,0);
												// glVertex2s(currentX+offset,currentY+offset);
												// glTexCoord2s(0,0);
												// glVertex2s(currentX-offset,currentY+offset);
											// glEnd();
										}
										else
										{
											glBegin(GL_QUADS);
												glTexCoord2s(0,1);
												glVertex2s(currentX,currentY);
												glTexCoord2s(1,1);
												glVertex2s(currentX+tileSize,currentY);
												glTexCoord2s(1,0);
												glVertex2s(currentX+tileSize,currentY+tileSize);
												glTexCoord2s(0,0);
												glVertex2s(currentX,currentY+tileSize);
											glEnd();
										}
									}
								}
							}
						}
							// DRAW PARTIAL FOG OVER THE TILE IF REQUIRED.
						if ( aFogOfWar!=0 && aFogOfWar->isSafe(tileX,tileY)==true && (*aFogOfWar)(tileX,tileY) == 1 )
						{
							// PARTIAL FOG
							Renderer::setColourMode();
							Renderer::placeColour4a(0,0,0,120,currentX,currentY,currentX+tileSize,currentY+tileSize);
							Renderer::setTextureMode();

								//Renderer::placeTexture4(currentX,currentY,currentX+tileSize,currentY+tileSize,&TEX_COLOUR_GREY,false);

						}

							// STATIC OVERLAY RENDERING.

						if ( overlayActive==true && aOverlay != 0 )
						{
							if ( (*aOverlay)(tileX,tileY)!=0 )
							{
								Renderer::placeTexture4(currentX,currentY,currentX+tileSize,currentY+tileSize, (*aOverlay)(tileX,tileY));
							}
						}

							// DYNAMIC OVERLAY RENDERING.

						if (squareOverlay==true && squareOverlayTexture != 0 && squareOverlayRange!=0)
						{
							const int borderX1 = hoveredXTile-squareOverlayRange;
							const int borderX2 = hoveredXTile+squareOverlayRange;

							const int borderY1 = hoveredYTile-squareOverlayRange;
							const int borderY2 = hoveredYTile+squareOverlayRange;


								// SQUARE BORDER OVERLAY.
							if ( tileX<=borderX2 && tileX >=borderX1 && tileY <=borderY2 && tileY >=borderY1 )
							{
									// BORDER
								if ( tileX==borderX1 || tileX==borderX2 || tileY==borderY1 ||tileY==borderY2 )
								{
									Renderer::setColourMode();
									Renderer::placeColour4a(0,0,0,100,currentX,currentY,currentX+tileSize,currentY+tileSize);
									Renderer::setTextureMode();
								}
									// INNER BIT
								else
								{
									Renderer::setColourMode();
									Renderer::placeColour4a(0,0,0,60,currentX,currentY,currentX+tileSize,currentY+tileSize);
									Renderer::setTextureMode();
								}
							}
							// if (
								// (tileX<=hoveredXTile+squareOverlayRange)&&(tileX>=hoveredXTile-squareOverlayRange)
							// &&(tileY<=hoveredYTile+squareOverlayRange)&&(tileY>=hoveredYTile-squareOverlayRange)
								// )
							// {
								// if ( tileX==hovered

							// }
						}

	//bool squareOverlay;
	//	Texture * squareOverlayTexture;
	//	int squareOverlayRange;

					}
					if ( gridMode==true)
					{
							//Renderer::placeColour4a(255,0,0,100,currentX,currentY,currentX+tileSize,currentY+tileSize);

							Renderer::placeTexture4(currentX,currentY,currentX+tileSize,currentY+tileSize,&TEX_OVERLAY_GRID,false);
					}

				++tileX;
			}

			tileX=revertTileX;
			++tileY;
		}
		
		for (int i=0;i<vLast.size();++i)
		{
			vLast(i)->render();
		}

		// DRAW A REFERENCE CENTER POINT.
		//const int centerX = mainViewNX/2;
		//const int centerY = mainViewNY/2;

		//Renderer::setColourMode();
		//Renderer::placeColour4(255,0,0,centerX-2,centerY-2,centerX+2,centerY+2);
		//Renderer::setTextureMode();

		// TEST TEXTURE ATLAS.
		//Renderer::setTextureMode();	//Renderer::placeTexture4(centerX,centerY,centerX+tileSize,centerY+tileSize,&TEX_OVERLAY_GRID,false);
		//Renderer::setTextureMode();
		//Renderer::placeTexture4(centerX,centerY,centerX+100,centerY+100,&ta.mainTexture,false);





		Renderer::restoreViewPort();
		normaliseCoordinates();
	}

	void keyboardEvent(Keyboard* _keyboard)
	{
		//std::cout<<"KB EVENT()\n";
		if ( aBoard==0 || active==false ) { return; }
		// Pass keyboard event, instead of handling individual keypressed here.
//std::cout<<"KB EVENT 2\n";

		//if(_keyboard->keyWasPressed)
		//{
			if(_keyboard->isPressed(Keyboard::RIGHT))
			{
			}
			else if(_keyboard->isPressed(Keyboard::LEFT))
			{
			}
			else if(_keyboard->isPressed(Keyboard::UP))
			{
			}
			else if(_keyboard->isPressed(Keyboard::DOWN))
			{
			}
			// Zoom the main map in by one step.
			if(_keyboard->isPressed(Keyboard::EQUALS))
			{
				//std::cout<<"zoomin\n";
				//if (_keyboard->keyWasPressed)
				//{
					zoomIn();
				//}
			}
			// Zoom the main map out by one step.
			if(_keyboard->isPressed(Keyboard::MINUS))
			{
				//std::cout<<"zoomin\n";
				//if (_keyboard->keyWasPressed)
				//{
					zoomOut();
				//}
			}
			if(_keyboard->isPressed(Keyboard::SPACEBAR))
			{
			}
			if (_keyboard->isPressed(Keyboard::Q)|| _keyboard->isPressed(Keyboard::q))
			{
			}
			if(_keyboard->isPressed(Keyboard::ESCAPE))
			{
			}


			if ( _keyboard->isPressed(Keyboard::LEFT_SHIFT)==true || _keyboard->isPressed(Keyboard::RIGHT_SHIFT) == true )
			{
				std::cout<<"SHIFT PANNING.\n";
				shiftPanning=true;
				//panning=true;
				/* Set initial pan coordinates. */
				//panX=mouse->x;
				//panY=mouse->y;
			}
			else
			{
				shiftPanning=false;
			}


		//}
	}

	inline void zoomIn()
	{
		if ( hoveredXTile != -1 && hoveredYTile != -1)
		{
			// const int centerXPixel = (mainViewX2 - mainViewX1)+mainViewX1;
			// const int centerYPixel = (mainViewY2 - mainViewY1)+mainViewY1;
			centerTileX = hoveredXTile+0.5;
			centerTileY = hoveredYTile+0.5;
			// //glutWarpPointer(centerXPixel,10);
			// glutWarpPointer(windowWidth / 2, windowHeight / 2);

				//int ww = glutGet(GLUT_WINDOW_WIDTH);
				//int wh = glutGet(GLUT_WINDOW_HEIGHT);
			 
				//int dx = x - ww / 2;
				//int dy = y - wh / 2;

				glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
		}
		tileSize*=2;
		normaliseZoom();
		//if(tileSize>MAX_ZOOM)
		//{ tileSize=MAX_ZOOM; }
	}
	inline void zoomOut()
	{
		tileSize/=2;
		normaliseZoom();
		//if(tileSize>MIN_ZOOM)
		//{ tileSize/=2; }
	}

	void normaliseZoom()
	{
		if ( tileSize<MIN_ZOOM )
		{
			tileSize=MIN_ZOOM;
		}
		if ( tileSize > MAX_ZOOM )
		{
			tileSize=MAX_ZOOM;
		}
	}

		// MAKE THE OVERLAY ARRAY EXIST AND VALID. ALSO ENABLES IT.
	void initOverlay()
	{
		if ( aBoard!=0 )
		{
			if (aOverlay == 0)
			{
				aOverlay = new ArrayS2 <HasTexture*>;
			}
			else
			{
				delete aOverlay;
				aOverlay = 0;
				aOverlay = new ArrayS2 <HasTexture*>;
			}
			aOverlay->init(aBoard->nX,aBoard->nY,0);
			overlayActive=true;
		}
	}
	void clearOverlay()
	{
		initOverlay();
	}

};

#endif
