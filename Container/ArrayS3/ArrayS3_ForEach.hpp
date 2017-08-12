	// FOREACH MACRO
	//#define FOR_EACH(a,x) const int _ARRAYSIZE=a.nX*a.nY*a.nZ; for (int _i=0;_i<_ARRAYSIZE;++_i){ x; }
	#define FOR_EACH_XY(_array,_code) for ( unsigned int _y=0;_y<_array.nY;++_y){for ( unsigned int _x=0;_x<_array.nX;++_x){_code;}}

	#define FOR_EACH_XY_POINTER(_array,_code) for ( unsigned int _y=0;_y<_array->nY;++_y){for ( unsigned int _x=0;_x<_array->nX;++_x){_code;}}




#ifdef WILDCAT_CONTAINER_ARRAYS3_ARRAYS3_FOREACH_EXAMPLE_CODE

	//EXAMPLE CODE:
	//OLD:

for ( unsigned int _y=0;_y<aObjects.nY;++_y)
{
	for ( unsigned int _x=0;_x<aObjects.nX;++_x)
	{
		aObjects(_x,_y,0)=&TEX_GRASS;
		if ( Random::oneIn(10)==true )
		{
			aObjects(_x,_y,1)=&TEX_GLOBALTREE;
		}
	}
}

	//NEW:

FOR_EACH_XY( aObjects,
	aObjects(_x,_y,0)=&TEX_GRASS;
	if ( Random::oneIn(10)==true )
	{
		aObjects(_x,_y,1)=&TEX_GLOBALTREE;
	}
);

#endif
