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
	(*render)->Xsp[0][0] = (display)->xres / 2;
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

	(*render)->numlights = 0;

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

	GzMatrix identity = { 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 };

	//push Xsp
	GzPushMatrix(render, render->Xsp);
	NormalMatrix(render, identity);

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
	NormalMatrix(render, identity);

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

int NormalMatrix(GzRender *render, GzMatrix matrix)
{
	float temp = sqrt(pow(matrix[0][0], 2) + pow(matrix[0][1], 2) + pow(matrix[0][2], 2) + pow(matrix[0][3], 2));
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			matrix[i][j] /= temp;
		}
	}
	Matrixcopy(render->Xnorm[render->matlevel], matrix);
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
	NormalMatrix(render, matrix);

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
		if (nameList[i] == GZ_RGB_COLOR)
		{
			GzColor* color = (GzColor*)valueList[i];

			render->flatcolor[0] = color[i][0];
			render->flatcolor[1] = color[i][1];
			render->flatcolor[2] = color[i][2];
		}
		else if (nameList[i] == GZ_INTERPOLATE)
		{
			int* interpmode = (int*)valueList[i];
			render->interp_mode = *interpmode;
		}
		else if (nameList[i] == GZ_DIRECTIONAL_LIGHT)
		{
			GzLight *dl = (GzLight*)valueList[i];
			render->lights[render->numlights] = *dl;
			render->numlights++;
		}
		else if (nameList[i] == GZ_AMBIENT_LIGHT)
		{
			GzLight* al = (GzLight*)valueList[i];
			render->ambientlight = *al;
		}
		else if (nameList[i] == GZ_AMBIENT_COEFFICIENT)
		{
			GzColor* alColor = (GzColor*)valueList[i];

			render->Ka[0] = alColor[0][0];
			render->Ka[1] = alColor[0][1];
			render->Ka[2] = alColor[0][2];
		}
		else if (nameList[i] == GZ_DIFFUSE_COEFFICIENT)
		{
			GzColor* dColor = (GzColor*)valueList[i];

			render->Kd[0] = dColor[0][0];
			render->Kd[1] = dColor[0][1];
			render->Kd[2] = dColor[0][2];
		}
		else if (nameList[i] == GZ_SPECULAR_COEFFICIENT)
		{
			GzColor* sColor = (GzColor*)valueList[i];

			render->Ks[0] = sColor[0][0];
			render->Ks[1] = sColor[0][1];
			render->Ks[2] = sColor[0][2];
		}
		else if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT)
		{
			float* dCoeff = (float*)valueList[i];
			render->spec = *dCoeff;
		}
	}

	return GZ_SUCCESS;
}

void MatrixMultiply(GzMatrix dest, GzMatrix m1, GzMatrix m2)
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

void Shader(GzRender *render, GzColor color, GzCoord normal)
{
	GzCoord z;
	z[X] = 0;
	z[Y] = 0;
	z[Z] = -1;

	GzCoord* Light = new GzCoord[render->numlights];

	int* lights = new int[render->numlights];

	float dp1, dp2;

	dp1 = normal[X] * z[X] + normal[Y] * z[Y] + normal[Z] * z[Z];

	for (int i = 0; i < render->numlights; i++)
	{
		GzCoord dir;

		dir[X] = render->lights[i].direction[X];
		dir[Y] = render->lights[i].direction[Y];
		dir[Z] = render->lights[i].direction[Z];

		dp2 = normal[X] * dir[X] + normal[Y] * dir[Y] + normal[Z] * dir[Z];

		if (dp1 >= 0 && dp2 >= 0)
		{
			lights[i] = 1;

			Light[i][X] = 2 * dp2*normal[X] - dir[X];
			Light[i][Y] = 2 * dp2*normal[Y] - dir[Y];
			Light[i][Z] = 2 * dp2*normal[Z] - dir[Z];

			float temp = sqrt(pow(Light[i][X], 2) + pow(Light[i][Y], 2) + pow(Light[i][Z], 2));

			Light[i][X] = Light[i][X] / temp;
			Light[i][Y] = Light[i][Y] / temp;
			Light[i][Z] = Light[i][Z] / temp;
		}
		else if (dp1 < 0 && dp2 < 0)
		{
			lights[i] = 2;

			Light[i][X] = 2 * dp2*(-normal[X]) - dir[X];
			Light[i][Y] = 2 * dp2*(-normal[Y]) - dir[Y];
			Light[i][Z] = 2 * dp2*(-normal[Z]) - dir[Z];

			float temp = sqrt(pow(Light[i][X], 2) + pow(Light[i][Y], 2) + pow(Light[i][Z], 2));

			Light[i][X] = Light[i][X] / temp;
			Light[i][Y] = Light[i][Y] / temp;
			Light[i][Z] = Light[i][Z] / temp;
		}
		else
		{
			lights[i] = 0;
			continue;
		}
	}

	GzColor SpecularLight = { 0, 0, 0 };
	for (int i = 0; i < render->numlights; ++i)
	{
		if (lights[i] == 0)
			continue;

		dp2 = z[X] * Light[i][X] + z[Y] * Light[i][Y] + z[Z] * Light[i][Z];

		if (dp2 < 0)
			dp2 = 0;

		if (dp2 > 1)
			dp2 = 1;

		SpecularLight[X] += render->lights[i].color[0] * pow(dp2, render->spec);
		SpecularLight[Y] += render->lights[i].color[1] * pow(dp2, render->spec);
		SpecularLight[Z] += render->lights[i].color[2] * pow(dp2, render->spec);
	}
	SpecularLight[X] *= render->Ks[X];
	SpecularLight[Y] *= render->Ks[Y];
	SpecularLight[Z] *= render->Ks[Z];

	GzColor DiffusedLight = { 0, 0, 0 };
	for (int i = 0; i < render->numlights; ++i)
	{
		GzCoord dir;

		dir[X] = render->lights[i].direction[X];
		dir[Y] = render->lights[i].direction[Y];
		dir[Z] = render->lights[i].direction[Z];

		float mul;
		if (lights[i] == 0)
			continue;
		if (lights[i] == 1)
		{
			mul = dir[X] * normal[X] + dir[Y] * normal[Y] + dir[Z] * normal[Z];

			DiffusedLight[X] += render->lights[i].color[0] * mul;
			DiffusedLight[Y] += render->lights[i].color[1] * mul;
			DiffusedLight[Z] += render->lights[i].color[2] * mul;
		}
		if (lights[i] == 2)
		{
			mul = dir[X] * (-normal[X]) + dir[Y] * (-normal[Y]) + dir[Z] * (-normal[Z]);

			DiffusedLight[X] += render->lights[i].color[0] * mul;
			DiffusedLight[Y] += render->lights[i].color[1] * mul;
			DiffusedLight[Z] += render->lights[i].color[2] * mul;
		}
	}
	DiffusedLight[X] *= render->Kd[X];
	DiffusedLight[Y] *= render->Kd[Y];
	DiffusedLight[Z] *= render->Kd[Z];

	GzColor AmbientLight = { 0, 0, 0 };
	AmbientLight[X] += render->ambientlight.color[0] * render->Ka[0];
	AmbientLight[Y] += render->ambientlight.color[1] * render->Ka[1];
	AmbientLight[Z] += render->ambientlight.color[2] * render->Ka[2];

	color[X] = SpecularLight[X] + DiffusedLight[X] + AmbientLight[X];
	color[Y] = SpecularLight[Y] + DiffusedLight[Y] + AmbientLight[Y];
	color[Z] = SpecularLight[Z] + DiffusedLight[Z] + AmbientLight[Z];
}

float Area(GzCoord v0, GzCoord v1, GzCoord v2)
{
	GzCoord line01 = { v1[X] - v0[X], v1[Y] - v0[Y], v1[Z] - v0[Z] };
	GzCoord line02 = { v2[X] - v0[X], v2[Y] - v0[Y], v2[Z] - v0[Z] };

	GzCoord xproduct = { (line01[Y] * line02[Z]) - (line01[Z] * line02[Y]), (line01[Z] * line02[X]) - (line01[X] * line02[Z]), (line01[X] * line02[Y]) - (line01[Y] * line02[X]) };
	float area = abs(.5 * (sqrt(pow(xproduct[X], 2) + pow(xproduct[Y], 2) + pow(xproduct[Z], 2)))); 

	return area;
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
		GzMatrix Xsm, Xn, temp;
		Matrixcopy(Xsm, render->Ximage[0]);
		Matrixcopy(Xn, render->Xnorm[0]);
		for (int i = 1; i <= render->matlevel; i++)
		{
			Matrixcopy(temp, Xsm);
			MatrixMultiply(Xsm, temp, render->Ximage[i]);

			Matrixcopy(temp, Xn);
			MatrixMultiply(Xn, temp, render->Xnorm[i]);
		}

		for (int i = render->matlevel; i >= 0; i--)
			GzPopMatrix(render);

		GzPushMatrix(render, Xsm);
		NormalMatrix(render, Xn);
	}

	float w;
	GzCoord* v = new GzCoord[3];
	GzCoord* vn = new GzCoord[3];

	bool cull = false;
	GzMatrix m1, m2;
	Matrixcopy(m1, render->Ximage[render->matlevel]);
	Matrixcopy(m2, render->Xnorm[render->matlevel]);

	for (int i = 0; i < numParts; i++)
	{
		if (nameList[i] == GZ_NULL_TOKEN)
			continue;

		if (nameList[i] == GZ_POSITION)
		{
			GzCoord* tempv = (GzCoord*)valueList[i];

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

				if (v[i][Z] < render->camera.position[Z])
				{
					cull = true;
					break;
				}
			}

			if (cull)
				break;
		}
		if (nameList[i] == GZ_NORMAL)
		{
			GzCoord* tempn = (GzCoord*)valueList[i];
			float temp;
			for (int i = 0; i < 3; i++)
			{
				temp = sqrt(pow(tempn[i][X], 2) + pow(tempn[i][Y], 2) + pow(tempn[i][Z], 2));

				tempn[i][X] = tempn[i][X] / temp;
				tempn[i][Y] = tempn[i][Y] / temp;
				tempn[i][Z] = tempn[i][Z] / temp;
			}

			for (int i = 0; i < 3; i++)
			{
				vn[i][X] = m2[0][0] * tempn[i][X] + m2[0][1] * tempn[i][Y] + m2[0][2] * tempn[i][Z];
				vn[i][Y] = m2[1][0] * tempn[i][X] + m2[1][1] * tempn[i][Y] + m2[1][2] * tempn[i][Z];
				vn[i][Z] = m2[2][0] * tempn[i][X] + m2[2][1] * tempn[i][Y] + m2[2][2] * tempn[i][Z];
			}

			for (int i = 0; i < 3; i++)
			{
				temp = sqrt(pow(vn[i][X], 2) + pow(vn[i][Y], 2) + pow(vn[i][Z], 2));

				vn[i][X] = vn[i][X] / temp;
				vn[i][Y] = vn[i][Y] / temp;
				vn[i][Z] = vn[i][Z] / temp;
			}
		}
	}
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
					float tempn = vn[i][k];
					v[i][k] = v[j][k];
					vn[i][k] = vn[j][k];
					v[j][k] = temp;
					vn[j][k] = tempn;
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

			GzDepth interpz = (-A*i - B*j - D) / C;

			GzCoord ThisPoint = { i, j, interpz };

			float TriangleArea = Area(v[0], v[1], v[2]);
			float A0 = Area(v[1], v[2], ThisPoint);
			float A1 = Area(v[0], v[2], ThisPoint);
			float A2 = Area(v[0], v[1], ThisPoint);

			if (render->interp_mode == GZ_COLOR)
			{
				GzColor c0, c1, c2;
				Shader(render, c0, vn[0]);
				Shader(render, c1, vn[1]);
				Shader(render, c2, vn[2]);

				float r = (A0*c0[0] + A1*c1[0] + A2*c2[0]) / TriangleArea;
				if (r > 1)
					r = 1;

				float g = (A0*c0[1] + A1*c1[1] + A2*c2[1]) / TriangleArea;
				if (g > 1)
					g = 1;
				
				float b = (A0*c0[2] + A1*c1[2] + A2*c2[2]) / TriangleArea;
				if (b > 1)
					b = 1;

				red = ctoi(r);
				green = ctoi(g);
				blue = ctoi(b);
			}
			else if (render->interp_mode == GZ_NORMALS)
			{
				GzCoord normalmode;

				normalmode[X] = (A0*vn[0][X] + A1*vn[1][X] + A2*vn[2][X]) / TriangleArea;
				normalmode[Y] = (A0*vn[0][Y] + A1*vn[1][Y] + A2*vn[2][Y]) / TriangleArea;
				normalmode[Z] = (A0*vn[0][Z] + A1*vn[1][Z] + A2*vn[2][Z]) / TriangleArea;

				float temp = sqrt(pow(normalmode[X], 2) + pow(normalmode[Y], 2) + pow(normalmode[Z], 2));

				normalmode[X] = normalmode[X] / temp;
				normalmode[Y] = normalmode[Y] / temp;
				normalmode[Z] = normalmode[Z] / temp;

				GzColor color;
				Shader(render, color, normalmode);

				red = (GzIntensity)ctoi(color[0]);
				green = (GzIntensity)ctoi(color[1]);
				blue = (GzIntensity)ctoi(color[2]);
			}

			GzIntensity r, g, b, a;
			GzDepth z;
			GzGetDisplay(render->display, i, j, &r, &g, &b, &a, &z);

			if (interpz < z)
			{
				if (Xintercept > v[1][0])						//For Counter Clockwise
				{
					if (L12 <= 0 && L20 <= 0 && L01 <= 0)
						GzPutDisplay(render->display, i, j, red, green, blue, a, interpz);
				}
				else											//For Clockwise
				{
					if (L12 > 0 && L20 > 0 && L01 > 0)
						GzPutDisplay(render->display, i, j, red, green, blue, a, interpz);
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

