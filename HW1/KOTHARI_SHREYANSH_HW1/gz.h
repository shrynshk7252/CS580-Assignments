/*
 * Gz.h - include file for rendering library 
*/

/*
 * universal constants
 */
#define GZ_SUCCESS      0
#define GZ_FAILURE      1


typedef void    *GzPointer;
typedef float   GzColor[3];

typedef short   GzIntensity;	/* 0-4095 in lower 12-bits - use for display RGBA */

typedef int	GzDepth;		/* signed int z for clipping */

#define RED     0        /* array indicies for color vector */
#define GREEN   1
#define BLUE    2

#define X       0        /* array indicies for 3D vectors */
#define Y       1
#define Z       2

#define U       0        /* array indicies for texture coords */
#define V       1

 