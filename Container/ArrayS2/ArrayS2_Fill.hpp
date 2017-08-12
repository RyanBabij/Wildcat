/* ArrayS2_Fill.hpp
	Interface of fill functions.
*/

	// FILL ALL INDEXES WITH THE PASSED VALUE
void fill(ARRAYS2_T value);

	// FILL THE AREA WITHIN THE GIVEN COORDINATES WITH THE PASSED VALUE.
void fill(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, ARRAYS2_T value);

void fill2Safe(int x1, int y1, int x2, int y2, ARRAYS2_T value);

void fillFromCenterPoint(const int _centerX, const int _centerY, const int _radius, ARRAYS2_T _value);

	// FILL ALL NEIGHBORS WHICH ARE EQUAL TO THE VALUE AT THE INITIAL POINT
void paintBucketFill ( const int _startX, const int _startY, const ARRAYS2_T _fillValue, bool includeDiagonals );

void floodFill ( const int _startX, const int _startY, const ARRAYS2_T _fillValue, bool includeDiagonals = true);


// 2 TYPES OF FLOOD FILL:
// ONE WILL WRITE THE DESIRED VALUE TO THE FILLED ENTRIES. (IF YOU WANT TO FILL EVERYTHING WITH A VALUE).
// ONE WILL RETURN A VECTOR OF HASXY OF ENTRIES CONTAINED IN THE AREA.

//Vector <HasXY> * floodFillVector ( const int _startX, const int _startY, bool includeDiagonals );