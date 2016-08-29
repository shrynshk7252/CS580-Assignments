/*   CS580 HW1 display functions to be completed   */

#include   "stdafx.h"  
#include	"Gz.h"
#include	"disp.h"


int GzNewFrameBuffer(char** framebuffer, int width, int height)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- pass back pointer 
 */
	if (width <= 0 || height <= 0 || width > MAXXRES || height > MAXYRES) 
		return GZ_FAILURE;

	*framebuffer = new char[3 * width * height];
	
	if (framebuffer == NULL)
		return GZ_FAILURE;

	return GZ_SUCCESS;
}

int GzNewDisplay(GzDisplay	**display, int xRes, int yRes)
{
/* HW1.2 create a display:
  -- allocate memory for indicated resolution
  -- pass back pointer to GzDisplay object in display
*/
	
	if (xRes <= 0 || yRes <= 0 || xRes > MAXXRES || yRes > MAXYRES) 
		return GZ_FAILURE;

	*display = new GzDisplay();

	(*display)->xres = xRes;
	(*display)->yres = yRes;
	(*display)->fbuf = new GzPixel[xRes*yRes];

	return GZ_SUCCESS;
}


int GzFreeDisplay(GzDisplay	*display)
{
/* HW1.3 clean up, free memory */
	
	delete display->fbuf;
	display->fbuf = NULL;
	delete display;

	return GZ_SUCCESS;
}


int GzGetDisplayParams(GzDisplay *display, int *xRes, int *yRes)
{
/* HW1.4 pass back values for a display */

	if (display == NULL)
		return GZ_FAILURE;

	xRes = (int*)display->xres;
	yRes = (int*)display->yres;
	
	return GZ_SUCCESS;
}


int GzInitDisplay(GzDisplay	*display)
{
/* HW1.5 set everything to some default values - start a new frame */

	if (display == NULL)
		return GZ_FAILURE;

	int ArraySize = display->xres * display->yres;

	for (int i = 0; i < ArraySize; i++)
	{
		display->fbuf[i].red = 2500;
		display->fbuf[i].green = 2700;
		display->fbuf[i].blue = 4000;
		display->fbuf[i].alpha = 1;
		display->fbuf[i].z = MAXINT;
	}

	return GZ_SUCCESS;
}


int GzPutDisplay(GzDisplay *display, int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.6 write pixel values into the display */
	 
	if (display == NULL)
		return GZ_FAILURE;

	if (i<0 || j<0 || i >= display->xres || j >= display->yres)
		return GZ_FAILURE;

	if (r < 0)
		r = 0;
	else if (r > 4095)
		r = 4095;

	if (g < 0)
		g = 0;
	else if (g > 4095)
		g = 4095;

	if (b < 0)
		b = 0;
	else if (b > 4095)
		b = 4095;


	display->fbuf[ARRAY(i, j)].red = r;
	display->fbuf[ARRAY(i, j)].green = g;
	display->fbuf[ARRAY(i, j)].blue = b;
	display->fbuf[ARRAY(i, j)].alpha = a;
	display->fbuf[ARRAY(i, j)].z = z;

	return GZ_SUCCESS;
}


int GzGetDisplay(GzDisplay *display, int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.7 pass back a pixel value to the display */

	if (display == NULL)
		return GZ_FAILURE;

	if (i<0 || j<0 || i >= display->xres || j >= display->yres)
		return GZ_FAILURE;

	*r = display->fbuf[ARRAY(i, j)].red;
	*g = display->fbuf[ARRAY(i, j)].green;
	*b = display->fbuf[ARRAY(i, j)].blue;
	*a = display->fbuf[ARRAY(i, j)].alpha;
	*z = display->fbuf[ARRAY(i, j)].z;

	return GZ_SUCCESS;
}


int GzFlushDisplay2File(FILE* outfile, GzDisplay *display)
{
/* HW1.8 write pixels to ppm file -- "P6 %d %d 255\r" */
	
	if (display == NULL)
		return GZ_FAILURE;

	if (outfile == NULL)
		return GZ_FAILURE;

	int ArraySize = display->xres * display->yres;

	fprintf(outfile, "P6 %d %d 255\n", display->xres, display->yres); 

	for (int i = 0; i < ArraySize; i++) 
	{
		GzPixel Pixel = display->fbuf[i];

		unsigned char r = Pixel.red >> 4;
		unsigned char g = Pixel.green >> 4;
		unsigned char b = Pixel.blue >> 4;

		fprintf(outfile, "%c%c%c", r, g, b);
	}

	return GZ_SUCCESS;
}

int GzFlushDisplay2FrameBuffer(char* framebuffer, GzDisplay *display)
{
/* HW1.9 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/

	if (display == NULL)
		return GZ_FAILURE;

	if (framebuffer == NULL)
		return GZ_FAILURE;
	
	int ArraySize = display->xres * display->yres;

	for (int i = 0; i < ArraySize; i++) 
	{
		GzPixel Pixel = display->fbuf[i];
		
		unsigned char r = Pixel.red >> 4;
		unsigned char g = Pixel.green >> 4;
		unsigned char b = Pixel.blue >> 4;

		framebuffer[i*3] = b;
		framebuffer[i*3 + 1] = g;
		framebuffer[i*3 + 2] = r;
	}

	return GZ_SUCCESS;
}