#pragma once
#ifndef  COLLISION_COLLISION_HPP
#define  COLLISION_COLLISION_HPP

/*
#include <Collision/Collision.hpp>

Keeps track of the outer bounding box, and the inner collision data.

Types of collision data:

Line, circle, quad.

Outer collision data: One or more quads that allow a quick proximity check. It is expected that most checks for collision will return false, so this speeds up the negative results.

Inner collision data: A combination of several shapes which usually correspond approximately or exactly to the actual object. If the outer collision returns positive, then the more complex inner collision data is checked.

This library should probably make use of the Geometry library.

Need to write a function to load collision data files.

*/

#include <Container/Vector/Vector.hpp>
#include <Math/Geometry/Geometry.hpp>

/* Objects using physics will inherit PhysicsInterface. */

class CollisionData2D
{
	public:
	

	
	

	
};

class PhysicsInterface2D
{
	public:
	
	/* Stores the points used to find collisions. */
	//CollisionData2D collisionData;
	
	/* The point from which relative coordinate data is calculated. */
	//double centerX, centerY;
	Point2D <double> centerPoint;
	
	/* If the object has a bounding circle, set it here. The circle should encompass the entire object. Set to -1 to disable. */
	double boundingRadius;
	
	/* TODO: Bounding box data goes here. */
	
	/* TODO: Detailed collision data goes here. */
	
	
	//MathVector2D <double> movementVector;
	
	//MathVector2D <double> forceVector;
	
	
	void collisionCheck (PhysicsInterface2D* target)
	{
	
		/* Compare bounding circles if applicable. */
		if(boundingRadius>0 && target->boundingRadius>0)
		{
			//std::cout<<"Checking bounding circles.\n";
			/* Distance between 2 points. */
			const double distance = centerPoint.distanceTo(&target->centerPoint);
			//std::cout<<"Distance is: "<<distance<<".\n";
			
			/* If the 2 circles overlap or touch. */
			if(distance<=(boundingRadius+target->boundingRadius))
			{
				std::cout<<"Bounding circle collision.\n";
				
				/* Calculate point of contact. */
				std::cout<<"Point of contact.\n";
				
				/* Draw a line from one centerpoint to the other. The point of contact is along this line. */
				MathVector2D <double> reboundVector (centerPoint,target->centerPoint);
				/* Normalize, invert. */
				reboundVector.normalise();
				//reboundVector.invert();
				
				std::cout<<"Rebound vector: ";
				reboundVector.print();
				std::cout<<"\n";
				/* Apply rebound energy. */
				//reboundVector.increaseMagnitude(2);
				
				//movementVector.addVector(reboundVector);
				//movementVector.set(0,0,0,0);
				
				/* Subtract the target radius from the line to get the point of contact. */
				
				//centerPointLine.set(centerPoint,target->centerPoint);
				recieveForce(&reboundVector);
				
				
				//return true;
			}
		}
		
		
	
		//return false;
	}
	
	virtual void recieveForce(MathVector2D <double>* forceVector)
	{
	}
	
	virtual void updateCollisionData()
	{
	}
};

/* Keeps track of all objects that have inherited from PhysicsInterface. */

class PhysicsInterfaceManager2D
{
	public:
	
	Vector <PhysicsInterface2D*> vPhysicsInterface;

	void add(PhysicsInterface2D* physicsObject)
	{
		vPhysicsInterface.add(physicsObject);
	}
	
	void updateAll()
	{
		const int vPhysicsInterfaceSize = vPhysicsInterface.size();
		for(int i=0;i<vPhysicsInterfaceSize;++i)
		{
			vPhysicsInterface(i)->updateCollisionData();
		
		}
	}
	
	/* Look for collision events and pass them out. */
	void tickAll()
	{
	
		const int vPhysicsInterfaceSize = vPhysicsInterface.size();
		for(int i=0;i<vPhysicsInterfaceSize;++i)
		{
			//std::cout<<"Coll check.\n";
		
			/* Compare the current collision object with all other collision objects. */
			for(int i2=0;i2<vPhysicsInterfaceSize;++i2)
			{
				/* Don't check an object against itself. */
				if(i2!=i && vPhysicsInterface(i) != vPhysicsInterface(i2))
				{
					vPhysicsInterface(i)->collisionCheck(vPhysicsInterface(i2));
					// if(vPhysicsInterface(i)->collisionCheck(vPhysicsInterface(i2)))
					// {
						// std::cout<<"Collision detected.\n";
						
						// MathVector2D <double> forceVector;
						// forceVector.set(0,0,100,100);
						
						// vPhysicsInterface(i)->recieveForce(&forceVector);
						
					// }
				}
			
			}
		
		}
	
	}
};

class Collision2D
{
	public:
	
	//bool active; /* Set to false for no possible collisions with this object. */
	
	/* NOTE: We need to have relative and absolute coords here to reduce uneccessary coordinate recalculations... */
	
	double rotation;
	Point2D <double> centerPoint;
	
	/* Outer bounding box for quick check. */
	//double outerX1,outerY1,outerX2,outerY2,outerX3,outerY3,outerX4,outerY4;
	Quad2D <double> outerHitBox;
	
	/* TODO: Inner collision data. */
	/* Collection of lines. */
	Vector <Line2D <double> *> vLine;
	/* Circles */
	/* Quads. */
	
	Collision2D()
	{
		rotation=0;
	//	active=false;
	}
	
	void addLine(const double _x1, const double _y1, const double _x2, const double _y2)
	{
		Line2D <double> * line = new Line2D <double> (_x1,_y1,_x2,_y2);
		vLine.push(line);
	}
	
	void setOuter(const double _outerX1, const double _outerY1, const double _outerX2, const double _outerY2, const double _outerX3, const double _outerY3, const double _outerX4, const double _outerY4)
	{
		// outerX1=_outerX1;
		// outerY1=_outerY1;
		// outerX2=_outerX2;
		// outerY2=_outerY2;
		// outerX3=_outerX3;
		// outerY3=_outerY3;
		// outerX4=_outerX4;
		// outerY4=_outerY4;
		outerHitBox.setCoordinates(_outerX1,_outerY1,_outerX2,_outerY2,_outerX3,_outerY3,_outerX4,_outerY4);
	}
	
	bool collidingInner()
	{
	
	
	
		return false;
	}
	
	bool collidingOuter(Collision2D* c)
	{
		//std::cout<<"Checking outers.\n";
	
		/* Common case: Line intersection betwen hitboxes. */
		if(outerHitBox.collidesWith(&c->outerHitBox))
		{
			//std::cout<<"Collision: Hitbox intersection.\n";
			//std::cout<<"Hitbox stats:\n";
			outerHitBox.printCoordinates();
			//std::cout<<"\n";
			c->outerHitBox.printCoordinates();
			//std::cout<<"\n";
			return true;
		}
		
		/* Special case: There may still be a collision without any line intersections if one of the hitboxes is lying completely within the other. We will check for this here. */
		/* Solution: Draw a point from the smallest point of the smaller hitbox to the smallest point of the larger hitbox. TBC. */
		
		/* Find the smallest hitbox. (This one will therefore potentially be inside the other.) */
		Quad2D <double> * smallerHitBox;
		Quad2D <double> * largerHitBox;
		const double thisHitBoxArea = outerHitBox.getArea();
		const double cHitBoxArea = c->outerHitBox.getArea();
		/* If this object's hitbox is smaller in area than the other hitbox. */
		if(thisHitBoxArea<cHitBoxArea)
		{
			largerHitBox=&c->outerHitBox;
			smallerHitBox=&outerHitBox;
		}
		else if (thisHitBoxArea>cHitBoxArea)
		{
			largerHitBox=&outerHitBox;
			smallerHitBox=&c->outerHitBox;
		}
		else
		{
			/* An unlikely possibility is that both hitboxes are exactly the same size. In this case it is impossible that they aren't colliding. The main possibility for this happening is if a hitbox is compared against itself... Though I think that would usually be unintentional. */
			return true;
		}
		
		/* Get a point guaranteed to lie outside of the larger quad. This is done by getting the lowest x and y coordinates, and then subtracting 1 from them. */
		Point2D <double> * externalPoint = largerHitBox->getPointOutside();
		
		/* Draw a line from one of the points of the inner hitbox, to this external point. */
		Line2D <double> line (0,0,0,0);
		
		
		/* Find smaller shape. */
		/* If this object's hitbox is smaller. */
		// if(outerHitBox.getArea() < c->outerHitBox.getArea() )
		// {
		
		
			// /* Make a line that goes from the smallest (closest to origin) point on this hitbox to the smallest point on the other hitbox. */

			
			// /* Point on smaller hitbox closest to origin. */
			
			// Line2D <double> l (0,0,0,0);
			
			
		// }
		// /* If the other object's hitbox is smaller. */
		// else
		// {
		// }
		
		
	
		/* Turn outer points into lines. */
		// Line2D <double> l1 (outerX1,outerY1,outerX2,outerY2);
		// Line2D <double> l2 (outerX2,outerY2,outerX3,outerY3);
		// Line2D <double> l3 (outerX3,outerY3,outerX4,outerY4);
		// Line2D <double> l4 (outerX4,outerY4,outerX1,outerY1);
		
		// Line2D <double> l1C (c->outerX1,c->outerY1,c->outerX2,c->outerY2);
		// Line2D <double> l2C (c->outerX2,c->outerY2,c->outerX3,c->outerY3);
		// Line2D <double> l3C (c->outerX3,c->outerY3,c->outerX4,c->outerY4);
		// Line2D <double> l4C (c->outerX4,c->outerY4,c->outerX1,c->outerY1);
		
		// if(l1.collidesWith(&l1C) || l1.collidesWith(&l2C) || l1.collidesWith(&l3C) || l1.collidesWith(&l4C)
		// || l2.collidesWith(&l1C) || l2.collidesWith(&l2C) || l2.collidesWith(&l3C) || l2.collidesWith(&l4C)
		// || l3.collidesWith(&l1C) || l3.collidesWith(&l2C) || l3.collidesWith(&l3C) || l3.collidesWith(&l4C)
		// || l4.collidesWith(&l1C) || l4.collidesWith(&l2C) || l4.collidesWith(&l3C) || l4.collidesWith(&l4C) )
		// {
			// return true;
		// }
		/* Also check if shapes overlap. I'll do this by picking a point from the smaller shape, and then creating a line with a point that is definitely outside of the bigger shape. If this line collides with the bigger shape, then they are overlapping. */
		
		/* Find smaller shape. */
		
		
		
		
	
		/* Get largest and smallest X values. */
		// double smallestX=outerX1;
		// if(outerX2<smallestX) { smallestX=outerX2; }
		// if(outerX3<smallestX) { smallestX=outerX3; }
		// if(outerX4<smallestX) { smallestX=outerX4; }
		// double largestX=outerX1;
		// if(outerX2>largestX) { largestX=outerX2; }
		// if(outerX3>largestX) { largestX=outerX3; }
		// if(outerX4>largestX) { largestX=outerX4; }
		// std::cout<<"LargestX: "<<largestX<<".\n";
		// std::cout<<"SmallestX: "<<smallestX<<".\n";
		
		// /* Get largest and smallest X values. */
		// double smallestXC=c->outerX1;
		// if(c->outerX2<smallestXC) { smallestXC=c->outerX2; }
		// if(c->outerX3<smallestXC) { smallestXC=c->outerX3; }
		// if(c->outerX4<smallestXC) { smallestXC=c->outerX4; }
		// double largestXC=c->outerX1;
		// if(c->outerX2>smallestXC) { smallestXC=c->outerX2; }
		// if(c->outerX3>smallestXC) { smallestXC=c->outerX3; }
		// if(c->outerX4>smallestXC) { smallestXC=c->outerX4; }
		
		
		
		
		// if( (smallestX>=smallestXC && smallestX<=largestXC)
		// ||  (largestX>=smallestXC && largestX<=largestXC) )
		// {
			// std::cout<<"X collision.\n";
			// return true;
		// }
		
		// if(outerX1>c->outerX1 && outerX1<c->outerX2)
		// {
			// std::cout<<"X collision.\n";
			// return true;
		// }
	
	
		return false;
	}
	
	// bool collidingWith(Collision2D* c)
	// {
		// /* Check outer. */
	
		// /* Check inners. */
	
		// return false;
	// }
	
};

/* Handles all rigid body collisions. */
class CollisionManager2D
{
	public:
	
	Vector <Collision2D*> vCollision;
	
	
};

#endif
