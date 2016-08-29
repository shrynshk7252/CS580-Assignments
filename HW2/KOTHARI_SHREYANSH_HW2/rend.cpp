#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include	"disp.h"

short	ctoi(float color);

int GzNewRender(GzRender **render, GzDisplay *display)
{
/* 
- malloc a renderer struct
- span interpolator needs pointer to display for pixel writes
*/

	*render = new GzRender();

	if (display == NULL) 
		return GZ_FAILURE;

	(*render)->display = display;

	return GZ_SUCCESS;
}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/

	if (render == NULL) 
		return GZ_FAILURE;

	render->display = NULL;
	delete render;

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender	*render)
{
/* 
- set up for start of each frame - init frame buffer
*/

	if (render == NULL) 
		return GZ_FAILURE;
	
	if (render->display == NULL)
		return GZ_FAILURE;

	GzInitDisplay(render->display);

	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer *valueList) /* void** valuelist */
{
/*
- set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
- later set shaders, interpolaters, texture maps, and lights
*/

	if (render == NULL)  
		return GZ_FAILURE;
	
	if (nameList == NULL) 
		return GZ_FAILURE;
	
	if (valueList == NULL) 
		return GZ_FAILURE;
	

	for (int i = 0; i < numAttributes; i++) 
	{
		if (nameList[i] = GZ_RGB_COLOR) 
		{
			GzColor* color = (GzColor*)valueList[i];

			render->flatcolor[0] = color[i][0];
			render->flatcolor[1] = color[i][1];
			render->flatcolor[2] = color[i][2];
		}
	}

	return GZ_SUCCESS;
}


int GzPutTriangle(GzRender *render, int	numParts, GzToken *nameList,
	GzPointer *valueList) 
/* numParts - how many names and values */
{
/* 
- pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions 
- Invoke the scan converter and return an error code
*/
	if (render == NULL)
		return GZ_FAILURE;
	for (int i = 0; i < numParts; i++)
	{
		if (nameList[i] = GZ_NULL_TOKEN)
			continue;

		if (nameList[i] = GZ_POSITION)
		{
			GzCoord* v = (GzCoord*)valueList[i];

			for (int i = 0; i < 2; i++)								//Sorting the coordinates according to Y
			{
				float Min = v[i][1];
				int MinIndex = i;
				for (int j = i + 1; j < 3; j++)
				{
					if (v[j][1] < Min)
					{
						Min = v[j][1];
						for (int k = 0; k < 3; k++)
						{
							float temp = v[i][k];
							v[i][k] = v[j][k];
							v[j][k] = temp;
						}
					}
				}
			}
			
			float L02x = v[2][X] - v[0][X];							//Calculating Plane equation (A,B,C,D)
			float L02y = v[2][Y] - v[0][Y];
			float L02z = v[2][Z] - v[0][Z];

			float L12x = v[2][X] - v[1][X];
			float L12y = v[2][Y] - v[1][Y];
			float L12z = v[2][Z] - v[1][Z];

			float temp1, temp2, A, B, C, D;
			
			temp1 = L02y*L12z;
			temp2 = L02z*L12y;
			A = temp1 - temp2;

			temp1 = L02z*L12x;
			temp2 = L02x*L12z;
			B = temp1 - temp2;

			temp1 = L02x*L12y;
			temp2 = L02y*L12x;
			C = temp1 - temp2;

			D = -(A*v[2][X] + B*v[2][Y] + C*v[2][Z]);

			float maxX, maxY, minX, minY;							//Maximum and Minimum X,Y Values

			maxY = v[2][1];
			minY = v[0][1];

			maxX = v[0][0];
			minX = v[0][0];
			for (int i = 0; i < 3; i++)
			{
				if (v[i][0]>maxX)
					maxX = v[i][0];
				if (v[i][0]<minX)
					minX = v[i][0];
			}

			float L01A = v[0][1] - v[1][1];							//Finding Line Equations
			float L01B = -(v[0][0] - v[1][0]);
			float L01C = -L01B * v[1][1] - L01A * v[1][0];

			float L12A = v[1][1] - v[2][1];
			float L12B = -(v[1][0] - v[2][0]);
			float L12C = -L12B * v[2][1] - L12A * v[2][0];

			float L20A = v[2][1] - v[0][1];
			float L20B = -(v[2][0] - v[0][0]);
			float L20C = -L20B * v[0][1] - L20A * v[0][0];
			
			float Xintercept = -(L20B*v[1][1] + L20C) / L20A;		//Caluclating X Intercept

			GzIntensity red = ctoi(render->flatcolor[0]);
			GzIntensity green = ctoi(render->flatcolor[1]);
			GzIntensity blue = ctoi(render->flatcolor[2]);

			for (int j = minY; j < maxY; j++)
			{
				for (int i = minX; i < maxX; i++)
				{
					
					float L20 = L20A * i + L20B * j + L20C;			//Calculating LEE Values
					float L12 = L12A * i + L12B * j + L12C;
					float L01 = L01A * i + L01B * j + L01C;

					if (Xintercept > v[1][0])						//For Counter Clockwise
					{
						if (L12 <= 0 && L20 <= 0 && L01 <= 0)
						{
							GzDepth interpz = (-A*i - B*j - D) / C;

							GzIntensity r, g, b, a;
							GzDepth z;
							GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);

							if (interpz < z) 
								GzPutDisplay(render->display, i, j, red, green, blue, a, interpz);
						}
					}
					else											//For Clockwise
					{
						if (L12 > 0 && L20 > 0 && L01 > 0)
						{
							GzDepth interpz = (-A*i - B*j - D) / C;

							GzIntensity r, g, b, a;
							GzDepth z;
							GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);

							if (interpz < z)
								GzPutDisplay(render->display, i, j, red, green, blue, a, interpz);
						}
					}
				}
			}
		}
	}
	return GZ_SUCCESS;
	
}

/* NOT part of API - just for general assistance */

short	ctoi(float color)		/* convert float color to GzIntensity short */
{
  return(short)((int)(color * ((1 << 12) - 1)));
}
