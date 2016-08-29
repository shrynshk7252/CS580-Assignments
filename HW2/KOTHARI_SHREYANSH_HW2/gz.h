/*
 * Gz.h 
 * csci 580   USC
*/

/*
 * universal constants
 */
#define GZ_SUCCESS      0
#define GZ_FAILURE      1


/*
 * name list tokens
 */
#define GZ_NULL_TOKEN           0	/* triangle vert attributes */
#define GZ_POSITION             1
#define GZ_NORMAL               2
#define GZ_TEXTURE_INDEX        3

/* renderer-state color values for default (flat) shading */
#define GZ_RGB_COLOR            99	/* we use RGB color space */

/*
 * flags fields for value list attributes
 */

typedef int     GzToken;
typedef void    *GzPointer;
typedef float   GzColor[3];
typedef short   GzIntensity;	/* 0 - 4095 in lower 12-bits */
typedef float   GzCoord[3];
typedef float	GzTextureIndex[2];
typedef float	GzMatrix[4][4];
typedef int	GzDepth;		/* z is signed for clipping */


#define RED     0		/* array indicies for color vector */
#define GREEN   1
#define BLUE    2

#define X       0	/* array indicies for position vector */
#define Y       1
#define Z       2

#define U       0	/* array indicies for texture coords */
#define V       1



