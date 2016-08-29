/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

GzColor	*image=NULL;
GzColor	*images = NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
	unsigned char		pixel[3];
	unsigned char     dummy;
	char  		foo[8];
	int   		i, j;
	FILE			*fd;

	if (reset)
	{          
		/* open and load texture file */
		fd = fopen ("texture", "rb");
		if (fd == NULL) 
		{
		  fprintf (stderr, "texture file not found\n");
		  exit(-1);
		}
		fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
		if (image == NULL) 
		{
		  fprintf (stderr, "malloc for texture image failed\n");
		  exit(-1);
		}
		for (i = 0; i < xs*ys; i++) 
		{	
			/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		reset = 0;          /* init is done */
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds -DONE*/

	if (u < 0)
		u = 0;
	else if (u > 1)
		u = 1;

	if (v < 0)
		v = 0;
	else if (v > 1)
		v = 1;

	/* determine texture cell corner values and perform bilinear interpolation -DONE*/
	
	float pointx = u * (xs - 1);
	float pointy = v * (ys - 1);
	float s = pointx - floor(pointx);
	float t = pointy - floor(pointy);

	int posA = floor(pointx) + xs * floor(pointy);
	int posB = ceil(pointx) + xs * floor(pointy);
	int posC = ceil(pointx) + xs * ceil(pointy);
	int posD = floor(pointx) + xs * ceil(pointy);

	/* set color to interpolated GzColor value and return -DONE*/

	for (i = 0; i < 3; i++)
	{
		color[i] = s * t * image[posC][i] + (1 - s) * t * image[posD][i] + s * (1 - t) * image[posB][i] + (1 - s) * (1 - t) * image[posA][i];
	}

	return GZ_SUCCESS;
}

/* Procedural texture function - Chequred board*/
int ptex_fun(float u, float v, GzColor color)
{
	/*
	unsigned char     dummy;
	char  		foo[8];
	FILE			*fd;
	if (reset)
	{
		fd = fopen("texture", "rb");

		if (fd == NULL)
		{
			fprintf(stderr, "texture file not found\n");
			exit(-1);
		}

		fscanf(fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		images = (GzColor*)malloc(sizeof(GzColor)*(xs + 1)*(ys + 1));
		for (int i = 0; i < xs; i++)
		{
			for (int j = 0; j < ys; j++)
			{
				if ((i % 20 <= 9 && j % 20 <= 9) || (i % 20 > 9 && j % 20 > 9))
				{
					images[i + xs*j][RED] = 1;
					images[i + xs*j][GREEN] = 0;
					images[i + xs*j][BLUE] = 0;
				}
				else
				{
					images[i + xs*j][RED] = 0;
					images[i + xs*j][GREEN] = 0;
					images[i + xs*j][BLUE] = 1;
				}
			}
			
		}
		reset = 0;
	}


	if (u < 0)
		u = 0;
	else if (u > 1)
		u = 1;

	if (v < 0)
		v = 0;
	else if (v > 1)
		v = 1;

	float pointx = u * (xs - 1);
	float pointy = v * (ys - 1);
	float s = pointx - floor(pointx);
	float t = pointy - floor(pointy);

	int posA = floor(pointx) + xs * floor(pointy);
	int posB = ceil(pointx) + xs * floor(pointy);
	int posC = ceil(pointx) + xs * ceil(pointy);
	int posD = floor(pointx) + xs * ceil(pointy);

	for (int i = 0; i < 3; i++)
	{
		color[i] = s * t * images[posC][i] + (1 - s) * t * images[posD][i] + s * (1 - t) * images[posB][i] + (1 - s) * (1 - t) * images[posA][i];
	}*/
	int flag = 0;
	float step = 0.2;
	for (int i = 0; i < 5;i++)
	{
		if (u < step)
		{
			flag = i;
			break;
		}
			
		step += 0.2;
	}
	int flag1 = 0;
	step = 0.2;
	for (int i = 0; i < 5; i++)
	{
		if (v < step)
		{
			flag1 = i;
			break;
		}
		step += 0.2;
	}
	if ((flag1 % 2 == 0 && flag % 2 == 0) || (flag1 % 2 == 1 && flag % 2 == 1))
	{
		color[0] = 1;
		color[1] = 0;
		color[2] = 0;
	}
	else
	{
		color[0] = 0;
		color[1] = 0;
		color[2] = 1;
	}



	return GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

