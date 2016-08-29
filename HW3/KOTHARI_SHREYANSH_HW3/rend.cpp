/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define pi 3.1415

short	ctoi(float color);

int GzRotXMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
	
	float theta = degree*(pi / 180);

	mat[0][0] = 1;
	mat[1][1] = cos(theta);
	mat[1][2] = -sin(theta);
	mat[2][1] = sin(theta);
	mat[2][2] = cos(theta);

	return GZ_SUCCESS;
}


int GzRotYMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value

	float theta = degree*(pi / 180);


	mat[0][0] = cos(theta);
	mat[0][2] = sin(theta);
	mat[1][1] = 1;
	mat[2][0] = -sin(theta);
	mat[2][2] = cos(theta);

	return GZ_SUCCESS;
}


int GzRotZMat(float degree, GzMatrix mat)
{
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value

	float theta = degree*(pi / 180);

	mat[0][0] = cos(theta);
	mat[0][1] = -sin(theta);
	mat[1][0] = sin(theta);
	mat[1][1] = cos(theta);
	mat[2][2] = 1;

	return GZ_SUCCESS;
}


int GzTrxMat(GzCoord translate, GzMatrix mat)
{
// Create translation matrix
// Pass back the matrix using mat value

	mat[0][3] = translate[X];
	mat[1][3] = translate[Y];
	mat[2][3] = translate[Z];
	mat[3][3] = 1;

	return GZ_SUCCESS;
}


int GzScaleMat(GzCoord scale, GzMatrix mat)
{
// Create scaling matrix
// Pass back the matrix using mat value

	mat[0][0] = scale[X];
	mat[1][1] = scale[Y];
	mat[2][2] = scale[Z];

	return GZ_SUCCESS;
}


//----------------------------------------------------------
// Begin main functions

int GzNewRender(GzRender **render, GzDisplay	*display)
{
/*  
- malloc a renderer struct 
- setup Xsp and anything only done once 
- save the pointer to display 
- init default camera 
*/ 

	*render = new GzRender();

	if (display == NULL)
		return GZ_FAILURE;

	(*render)->display = display;
	(*render)->matlevel = -1;

	//Scale
	(*render)->Xsp[0][0] = (display)->xres/2;
	(*render)->Xsp[1][1] = -(display)->yres / 2;
	(*render)->Xsp[2][2] = MAXINT;

	//Translate
	(*render)->Xsp[0][3] = (display)->xres / 2;
	(*render)->Xsp[1][3] = (display)->yres / 2;
	(*render)->Xsp[2][3] = 0;

	//W
	(*render)->Xsp[3][3] = 1;

	//Rest
	(*render)->Xsp[0][1] = 0;
	(*render)->Xsp[0][2] = 0;
	(*render)->Xsp[1][0] = 0;
	(*render)->Xsp[1][2] = 0;
	(*render)->Xsp[2][0] = 0;
	(*render)->Xsp[2][1] = 0;
	(*render)->Xsp[3][0] = 0;
	(*render)->Xsp[3][1] = 0;
	(*render)->Xsp[3][2] = 0;


	//Camera
	(*render)->camera.position[X] = DEFAULT_IM_X;
	(*render)->camera.position[Y] = DEFAULT_IM_Y;
	(*render)->camera.position[Z] = DEFAULT_IM_Z;

	(*render)->camera.lookat[X] = 0;
	(*render)->camera.lookat[Y] = 0;
	(*render)->camera.lookat[Z] = 0;

	(*render)->camera.worldup[X] = 0;
	(*render)->camera.worldup[Y] = 1;
	(*render)->camera.worldup[Z] = 0;

	(*render)->camera.FOV = DEFAULT_FOV;

	return GZ_SUCCESS;

}


int GzFreeRender(GzRender *render)
{
/* 
-free all renderer resources
*/

	if (render == NULL) {
		return GZ_FAILURE;
	}

	delete render;

	return GZ_SUCCESS;
}


int GzBeginRender(GzRender *render)
{
/*  
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 

	if (render == NULL)
		return GZ_FAILURE;

	if (render->display == NULL)
		return GZ_FAILURE;

	GzInitDisplay(render->display);

	//push Xsp
	GzPushMatrix(render, render->Xsp);

	//Matrix Xpi
	render->camera.Xpi[0][0] = 1;
	render->camera.Xpi[1][1] = 1;
	render->camera.Xpi[3][3] = 1;

	float theta = ((render->camera.FOV) / 2)*(pi / 180);

	render->camera.Xpi[2][2] = tan(theta);
	render->camera.Xpi[3][2] = tan(theta);

	render->camera.Xpi[0][1] = 0;
	render->camera.Xpi[0][2] = 0;
	render->camera.Xpi[0][3] = 0;
	render->camera.Xpi[1][0] = 0;
	render->camera.Xpi[1][2] = 0;
	render->camera.Xpi[1][3] = 0;
	render->camera.Xpi[2][0] = 0;
	render->camera.Xpi[2][1] = 0;
	render->camera.Xpi[2][3] = 0;
	render->camera.Xpi[3][0] = 0;
	render->camera.Xpi[3][1] = 0;

	//push Xpi
	GzPushMatrix(render, render->camera.Xpi);

	//Axis Calculation
	float temp, dotproduct = 0;

	//z-axis
	GzCoord z;

	z[X] = render->camera.lookat[X] - render->camera.position[X];
	z[Y] = render->camera.lookat[Y] - render->camera.position[Y];
	z[Z] = render->camera.lookat[Z] - render->camera.position[Z];

	temp = sqrt(pow(z[X], 2) + pow(z[Y], 2) + pow(z[Z], 2));

	z[X] = z[X] / temp;
	z[Y] = z[Y] / temp;
	z[Z] = z[Z] / temp;
	
	//y-axis
	GzCoord y;
	
	for (int i = 0; i < 3; i++)
		dotproduct += render->camera.worldup[i] * z[i];

	y[X] = render->camera.worldup[X] - (dotproduct*z[X]);
	y[Y] = render->camera.worldup[Y] - (dotproduct*z[Y]);
	y[Z] = render->camera.worldup[Z] - (dotproduct*z[Z]);

	temp = sqrt(pow(y[X], 2) + pow(y[Y], 2) + pow(y[Z], 2));

	y[X] = y[X] / temp;
	y[Y] = y[Y] / temp;
	y[Z] = y[Z] / temp;

	//x-axis
	GzCoord x;

	x[X] = y[Y] * z[Z] - y[Z] * z[Y];
	x[Y] = y[Z] * z[X] - y[X] * z[Z];
	x[Z] = y[X] * z[Y] - y[Y] * z[X];

	temp = sqrt(pow(x[X], 2) + pow(x[Y], 2) + pow(x[Z], 2));

	x[X] = x[X] / temp;
	x[Y] = x[Y] / temp;
	x[Z] = x[Z] / temp;

	//Matrix wi
	render->camera.Xiw[0][0] = x[X];
	render->camera.Xiw[0][1] = x[Y];
	render->camera.Xiw[0][2] = x[Z];

	render->camera.Xiw[1][0] = y[X];
	render->camera.Xiw[1][1] = y[Y];
	render->camera.Xiw[1][2] = y[Z];

	render->camera.Xiw[2][0] = z[X];
	render->camera.Xiw[2][1] = z[Y];
	render->camera.Xiw[2][2] = z[Z];

	render->camera.Xiw[3][3] = 1;

	render->camera.Xiw[0][3] = -((x[X] * render->camera.position[X]) + (x[Y] * render->camera.position[Y]) + (x[Z] * render->camera.position[Z]));
	render->camera.Xiw[1][3] = -((y[X] * render->camera.position[X]) + (y[Y] * render->camera.position[Y]) + (y[Z] * render->camera.position[Z]));
	render->camera.Xiw[2][3] = -((z[X] * render->camera.position[X]) + (z[Y] * render->camera.position[Y]) + (z[Z] * render->camera.position[Z]));

	render->camera.Xiw[3][0] = 0;
	render->camera.Xiw[3][1] = 0;
	render->camera.Xiw[3][2] = 0;

	//push Xiw
	GzPushMatrix(render, render->camera.Xiw);

	return GZ_SUCCESS;
}


int GzPutCamera(GzRender *render, GzCamera *camera)
{
/*
- overwrite renderer camera structure with new camera definition
*/
	if (render == NULL)
		return GZ_FAILURE;

	if (camera == NULL)
		return GZ_FAILURE;

	render->camera.FOV = camera->FOV;

	render->camera.lookat[X] = camera->lookat[X];
	render->camera.lookat[Y] = camera->lookat[Y];
	render->camera.lookat[Z] = camera->lookat[Z];

	render->camera.position[X] = camera->position[X];
	render->camera.position[Y] = camera->position[Y];
	render->camera.position[Z] = camera->position[Z];

	render->camera.worldup[X] = camera->worldup[X];
	render->camera.worldup[Y] = camera->worldup[Y];
	render->camera.worldup[Z] = camera->worldup[Z];

	float temp = sqrt(pow(render->camera.worldup[X], 2) + pow(render->camera.worldup[Y], 2) + pow(render->camera.worldup[Z], 2));

	render->camera.worldup[X] = render->camera.worldup[X] / temp;
	render->camera.worldup[Y] = render->camera.worldup[Y] / temp;
	render->camera.worldup[Z] = render->camera.worldup[Z] / temp;

	return GZ_SUCCESS;	
}

int GzPushMatrix(GzRender *render, GzMatrix	matrix)
{
/*
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if (render == NULL) 
		return GZ_FAILURE;
	
	if (matrix == NULL) 
		return GZ_FAILURE;
	
	render->matlevel = render->matlevel + 1;
	if (render->matlevel >= MATLEVELS) 
		return GZ_FAILURE;
	
	Matrixcopy(render->Ximage[render->matlevel], matrix);

	return GZ_SUCCESS;
}

int GzPopMatrix(GzRender *render)
{
/*
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (render == NULL) 
		return GZ_FAILURE;
	

	if (render->matlevel < 0) 
		return GZ_FAILURE;
	
	(render->matlevel)--;

	return GZ_SUCCESS;
}


int GzPutAttribute(GzRender	*render, int numAttributes, GzToken	*nameList, 
	GzPointer	*valueList) /* void** valuelist */
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

void MatrixMultiply(GzMatrix dest,GzMatrix m1,GzMatrix m2)
{
	
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			dest[i][j] = 0;
			for (int k = 0; k < 4; k++)
			{
				dest[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
}

void Matrixcopy(GzMatrix m1, GzMatrix m2)
{

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m1[i][j] = m2[i][j];
			
		}
	}
}

int GzPutTriangle(GzRender	*render, int numParts, GzToken *nameList, GzPointer	*valueList)
/* numParts : how many names and values */
{
/*  
- pass in a triangle description with tokens and values corresponding to 
      GZ_POSITION:3 vert positions in model space 
- Xform positions of verts using matrix on top of stack 
- Clip - just discard any triangle with any vert(s) behind view plane 
       - optional: test for triangles with all three verts off-screen (trivial frustum cull)
- invoke triangle rasterizer  
*/ 
	if (render == NULL)
		return GZ_FAILURE;

	if (render->matlevel > 0)
	{
		GzMatrix Xsm, temp;
		Matrixcopy(Xsm, render->Ximage[0]);
		for (int i = 1; i <= render->matlevel; i++)
		{
			Matrixcopy(temp, Xsm);
			MatrixMultiply(Xsm, temp, render->Ximage[i]);
		}

		for (int i = render->matlevel; i >= 0; i--)
			GzPopMatrix(render);

		GzPushMatrix(render, Xsm);
	}

	for (int i = 0; i < numParts; i++)
	{
		if (nameList[i] = GZ_NULL_TOKEN)
			continue;

		if (nameList[i] = GZ_POSITION)
		{
			GzCoord* tempv = (GzCoord*)valueList[i];

			float w;
			GzCoord* v = new GzCoord[3];

			bool cull=false;
			GzMatrix m1;
			Matrixcopy(m1, render->Ximage[render->matlevel]);

			/*
			for (int i = 0; i < 3; i++)
			{
				v[i][X] = m1[0][0] * tempv[i][X] + m1[0][1] * tempv[i][Y] + m1[0][2] * tempv[i][Z] + m1[0][3] * 1.0;
				v[i][Y] = m1[1][0] * tempv[i][X] + m1[1][1] * tempv[i][Y] + m1[1][2] * tempv[i][Z] + m1[1][3] * 1.0;
				v[i][Z] = m1[2][0] * tempv[i][X] + m1[2][1] * tempv[i][Y] + m1[2][2] * tempv[i][Z] + m1[2][3] * 1.0;

				w = m1[3][0] * tempv[i][X] + m1[3][1] * tempv[i][Y] + m1[3][2] * tempv[i][Z] + m1[3][3] * 1.0;

				v[i][X] /= w;
				v[i][Y] /= w;
				v[i][Z] /= w;

				if (v[i][Z] < render->camera.position[Z])
				{
					cull = true;
					break;
				}
			}*/
			
			for (int i = 0; i < 3; i++)			
			{
				for (int j = 0; j < 3; j++)
					v[i][j] = m1[j][0] * tempv[i][X] + m1[j][1] * tempv[i][Y] + m1[j][2] * tempv[i][Z] + m1[j][3] * 1.0;;

				w = m1[3][0] * tempv[i][X] + m1[3][1] * tempv[i][Y] + m1[3][2] * tempv[i][Z] + m1[3][3] * 1.0;

				v[i][X] /= w;
				v[i][Y] /= w;
				v[i][Z] /= w;
			}

			if (cull)
				break;

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

