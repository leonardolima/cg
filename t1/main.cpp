#include "main.h"

//-----------------------------------------------------------------------------
void MyGlDraw(void)
{
	//*************************************************************************
	// Chame aqui as funções do mygl.h
	//*************************************************************************
       
	Pixel p1, p2, p3, p4, p5, p6, p7, p8, p9;

	p1.x = 250;
	p1.y = 250;
	p1.c.r = 255;
	p1.c.g = 0;
	p1.c.b = 0;
	p1.c.a = 0;

	p2.x = 40;
	p2.y = 150;
	p2.c.r = 0;
	p2.c.g = 255;
	p2.c.b = 0;
	p2.c.a = 0;

	p3.x = 80;
	p3.y = 40;
	p3.c.r = 255;
	p3.c.g = 0;
	p3.c.b = 0;
	p3.c.a = 0;
	
	// drawLine(p1, p2);

	drawTriangle(p1, p2, p3);
}

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	// Inicializações.
	InitOpenGL(&argc, argv);
	InitCallBacks();
	InitDataStructures();

	// Ajusta a função que chama as funções do mygl.h
	DrawFunc = MyGlDraw;	

	// Framebuffer scan loop.
	glutMainLoop();

	return 0;
}

