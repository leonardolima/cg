#ifndef _MYGL_H_
#define _MYGL_H_

#include "definitions.h"

//*****************************************************************************
// Defina aqui as suas funções gráficas
//*****************************************************************************

class Color
{
 public:
	unsigned char r, g, b, a;
};


class Pixel
{
 public:
	unsigned int x, y;
	Color c;
};

void putPixel(Pixel p)
{
	if (p.x >= 0 && p.y >= 0 && p.x <= IMAGE_WIDTH && p.y <= IMAGE_HEIGHT)
	{
		FBptr[p.x * 4 + p.y * 4 * IMAGE_WIDTH] = p.c.r;
		FBptr[p.x * 4 + p.y * 4 * IMAGE_WIDTH + 1] = p.c.g;
		FBptr[p.x * 4 + p.y * 4 * IMAGE_WIDTH + 2] = p.c.b;
		FBptr[p.x * 4 + p.y * 4 * IMAGE_WIDTH + 3] = p.c.a;
	}
}

Pixel interpolate(Pixel p1, Pixel p2, Pixel p)
{
	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
	
	if (abs(dy) <= dx)
	{
		p.c.r = (unsigned char) (p1.c.r + (p.x - p1.x) * (p2.c.r - p1.c.r)/dx);
		p.c.g = (unsigned char) (p1.c.g + (p.x - p1.x) * (p2.c.g - p1.c.g)/dx);
		p.c.b = (unsigned char) (p1.c.b + (p.x - p1.x) * (p2.c.b - p1.c.b)/dx);
	} else {
		p.c.r = (unsigned char) (p1.c.r + (p.x - p1.x) * (p2.c.r - p1.c.r)/dy);
		p.c.g = (unsigned char) (p1.c.g + (p.x - p1.x) * (p2.c.g - p1.c.g)/dy);
		p.c.b = (unsigned char) (p1.c.b + (p.x - p1.x) * (p2.c.b - p1.c.b)/dy);
	}
	return p;
}

void drawLine(Pixel p1, Pixel p2)
{

	if (p1.x > p2.x) // Reflections
	{
		std::swap(p1.x, p2.x);
		std::swap(p1.y, p2.y);
	}
	
	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
	int d = 0, incr_x = 1, incr_y = 1;

	if (dx < 0) { incr_x = -1; dx = abs(dx); }
	if (dy < 0) { incr_y = -1; dy = abs(dy); }

	Pixel temp_p;
	temp_p.x = p1.x;
	temp_p.y = p1.y;
	temp_p.c = p1.c;

	if (dy <= dx) // Octants 1 and 8
	{
		do {
			//temp_p = interpolate(p1, p2, temp_p);
			putPixel(temp_p);
			temp_p.x += incr_x;
			d += 2 * dy;
			if (d >= dx) { temp_p.y += incr_y; d -= 2 * dx; }
		} while (temp_p.x < p2.x);

	} else {      // Octants 2 and 7
		do {
			//temp_p = interpolate(p1, p2, temp_p);
			putPixel(temp_p);
			temp_p.y += incr_y;
			d += 2 * dx;
			if (d >= dy) { temp_p.x += incr_x; d -= 2 * dy; }
		} while (temp_p.x < p2.x);
	}
}

void drawTriangle(Pixel p1, Pixel p2, Pixel p3)
{
	drawLine(p1, p2);
	drawLine(p1, p3);
	drawLine(p2, p3);
}

#endif // _MYGL_H_

