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
	p3.c.r = 0;
	p3.c.g = 255;
	p3.c.b = 0;
	p3.c.a = 0;

	p4.x = 350;
	p4.y = 40;
	p4.c.r = 0;
	p4.c.g = 255;
	p4.c.b = 0;
	p4.c.a = 0;

	p5.x = 487;
	p5.y = 150;
	p5.c.r = 0;
	p5.c.g = 255;
	p5.c.b = 0;
	p5.c.a = 0;

	p6.x = 487;
	p6.y = 350;
	p6.c.r = 0;
	p6.c.g = 255;
	p6.c.b = 0;
	p6.c.a = 0;

	p7.x = 350;
	p7.y = 487;
	p7.c.r = 0;
	p7.c.g = 255;
	p7.c.b = 0;
	p7.c.a = 0;

	p8.x = 40;
	p8.y = 350;
	p8.c.r = 0;
	p8.c.g = 255;
	p8.c.b = 0;
	p8.c.a = 0;

	p9.x = 150;
	p9.y = 487;
	p9.c.r = 0;
	p9.c.g = 255;
	p9.c.b = 0;
	p9.c.a = 0;

	drawLine(p1, p2);
	// drawLine(p1, p3);
	// drawLine(p1, p4);
	// drawLine(p1, p5);
	// drawLine(p1, p6);
	// drawLine(p1, p7);
	// drawLine(p1, p8);
	// drawLine(p1, p9);

	// for (int i = 0; i < 10; i++)
	// {
	// 	drawLine(p1, p2);
	// 	p2.x += 25;
	// 	p2.y -= 25;
	// }
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

