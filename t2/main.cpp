#define GLM_FORCE_RADIANS

// OpenGL/Glut
#include <GL/gl.h>
#include <GL/glut.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>
// Standard libs
#include <iostream>
#include <stdio.h>
// Header files
#include "objLoader.h"
#include "mygl.h"

#define IMAGE_WIDTH 512
#define IMAGE_HEIGHT 512

// Ponteiro para o objeto que carregará o modelo 3D (formato OBJ).
objLoader *objData;

unsigned int ViewPortWidth  = 512;
unsigned int ViewPortHeight = 512;

//-----------------------------------------------------------------------------
void display_without_pipeline(void)
{
	glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 4.0f);
	glm::vec3 camera_lookat = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

	glViewport(0, 0, ViewPortWidth, ViewPortHeight);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
  	glLoadIdentity();
	gluPerspective(55.0f, 				   // angulo de abertura da camera
		       (float)ViewPortWidth/ViewPortWidth, // aspecto da imagem
		       1.0f, 				   // near plane
		       50.0f);				   // far plane

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camera_pos[0], camera_pos[1], camera_pos[2], // posição da câmera
		  camera_lookat[0], camera_lookat[1], camera_lookat[2], // ponto para o qual a camera está olhando
		  camera_up[0], camera_up[1], camera_up[2]);   // vetor "up"

	///////////////////////////////////////////////////////////////////////////
	// Desenha os eixos do sistema de coordenadas do universo
	// Vermelho: eixo X
	// Verde: eixo Y
	// Azul: eixo Z
	///////////////////////////////////////////////////////////////////////////

	glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);	// eixo X
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(2.0f, 0.0f, 0.0f);

			glColor3f(0.0f, 1.0f, 0.0f);	// eixo Y
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 2.0f, 0.0f);

			glColor3f(0.0f, 0.0f, 1.0f);	// eixo Z
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 2.0f);
	glEnd();

	///////////////////////////////////////////////////////////////////////////
	// Desenha o modelo carregado utilizando linhas brancas.
	// Cada volta do loop desenha um triangulo, formado por tres linhas. 
	// Cada linha eh formada por 2 pontos (inicial e final).
	///////////////////////////////////////////////////////////////////////////

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_LINES);
		for(int i=0; i<objData->faceCount; i++)
		{
			obj_face *o = objData->faceList[i];

			glVertex3f(objData->vertexList[o->vertex_index[0]]->e[0], // primeira linha
				   objData->vertexList[o->vertex_index[0]]->e[1],
				   objData->vertexList[o->vertex_index[0]]->e[2]);
			glVertex3f(objData->vertexList[o->vertex_index[1]]->e[0],
				   objData->vertexList[o->vertex_index[1]]->e[1],
				   objData->vertexList[o->vertex_index[1]]->e[2]);

			glVertex3f(objData->vertexList[o->vertex_index[1]]->e[0], // segunda linha
				   objData->vertexList[o->vertex_index[1]]->e[1],
				   objData->vertexList[o->vertex_index[1]]->e[2]);
			glVertex3f(objData->vertexList[o->vertex_index[2]]->e[0],
				   objData->vertexList[o->vertex_index[2]]->e[1],
				   objData->vertexList[o->vertex_index[2]]->e[2]);

			glVertex3f(objData->vertexList[o->vertex_index[2]]->e[0], // terceira linha
				   objData->vertexList[o->vertex_index[2]]->e[1],
				   objData->vertexList[o->vertex_index[2]]->e[2]);
			glVertex3f(objData->vertexList[o->vertex_index[0]]->e[0],
				   objData->vertexList[o->vertex_index[0]]->e[1],
				   objData->vertexList[o->vertex_index[0]]->e[2]);
		}
	glEnd();

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

//-----------------------------------------------------------------------------
// Pipeline implementado manualmente
GLuint tex;

void (*DrawFunc)(glm::vec4 v1, glm::vec4 v2);

glm::vec4 pipeline(glm::vec3 camera_pos, 
		   glm::vec3 camera_lookat, 
		   glm::vec3 camera_up, 
		   glm::vec4 v_obj)
{
	///////////////////////////////////////////////////////////////////////////
	// Matriz model (Identidade)
	///////////////////////////////////////////////////////////////////////////
	float array_Model[4][4] = { 
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	glm::mat4 Model;
	memcpy(glm::value_ptr(Model), array_Model, sizeof(array_Model));
	Model = glm::transpose(Model);

	///////////////////////////////////////////////////////////////////////////
	// Calculo do sistema ortonormal gerado a partir dos parametros da camera
	///////////////////////////////////////////////////////////////////////////
	glm::vec3 z_camera = (camera_pos - camera_lookat) / 
			     glm::length(camera_pos - camera_lookat);
	glm::vec3 x_camera = glm::cross(camera_up, z_camera) / 
		             glm::length(glm::cross(camera_up, z_camera));
        glm::vec3 y_camera = glm::cross(z_camera, x_camera);

	///////////////////////////////////////////////////////////////////////////
	// Construção da matriz view
	///////////////////////////////////////////////////////////////////////////
        float array_Bt[4][4] = { 
		{x_camera[0], x_camera[1], x_camera[2], 0.0f},
		{y_camera[0], y_camera[1], y_camera[2], 0.0f},
		{z_camera[0], z_camera[0], z_camera[2], 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	glm::mat4 Bt;
	memcpy(glm::value_ptr(Bt), array_Bt, sizeof(array_Bt));
	Bt = glm::transpose(Bt);

	glm::vec3 translation = glm::vec3(-camera_pos[0], -camera_pos[1], -camera_pos[2]);
	glm::mat4 T = glm::translate(translation);
	glm::mat4 View = Bt * T;

	///////////////////////////////////////////////////////////////////////////
	// Construção da matriz ModelView
	///////////////////////////////////////////////////////////////////////////
	glm::mat4 ModelView = View * Model;

	///////////////////////////////////////////////////////////////////////////
	// Construção da matriz de projeção, com d = 1
	///////////////////////////////////////////////////////////////////////////
	float d = 1.0f;
	float array_Projection[4][4] = { 
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, d},
		{0.0f, 0.0f, -1/d, 0.0f}
	};
	glm::mat4 Projection;
	memcpy(glm::value_ptr(Projection), array_Projection, sizeof(array_Projection));
	Projection = glm::transpose(Projection);

	///////////////////////////////////////////////////////////////////////////
	// Construção da matriz ModelViewProjection
	///////////////////////////////////////////////////////////////////////////
	glm::mat4 ModelViewProjection = Projection * ModelView;

	///////////////////////////////////////////////////////////////////////////
	// Aplicação da matriz ModelViewProjection sobre os vértices no E.O.
	///////////////////////////////////////////////////////////////////////////
	glm::vec4 v_clip = ModelViewProjection * v_obj;

	///////////////////////////////////////////////////////////////////////////
	// Homogeneização (divisão por W)
	// Leva os vértices do espaço de recorte para o canônico
	///////////////////////////////////////////////////////////////////////////
	glm::vec4 v_canonic = v_clip / v_clip[3];

	///////////////////////////////////////////////////////////////////////////
	// Conversão de coordenadas do espaço canônico para o espaço de tela
	///////////////////////////////////////////////////////////////////////////
	float array_S1[4][4] = { 
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, -1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	glm::mat4 S1;
	memcpy(glm::value_ptr(S1), array_S1, sizeof(array_S1));
	S1 = glm::transpose(S1);

	float array_T2[4][4] = { 
		{1.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	glm::mat4 T2;
	memcpy(glm::value_ptr(T2), array_T2, sizeof(array_T2));
	T2 = glm::transpose(T2);

	float array_S2[4][4] = { 
		{round((ViewPortWidth-1)/2), 0.0f, 0.0f, 0.0f},
		{0.0f, round((ViewPortHeight-1)/2), 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	glm::mat4 S2;
	memcpy(glm::value_ptr(S2), array_S2, sizeof(array_S2));
	S2 = glm::transpose(S2);

	glm::mat4 ViewPort = S2 * T2 * S1;
	return ViewPort * v_canonic;
	//glm::vec4 v_screen = glm::round(ViewPort * v_canonic);

	//return v_screen;
}

void MyGlDraw(glm::vec4 v1, glm::vec4 v2)
{      
	Pixel p1, p2;

	p1.x = v1[0];
	p1.y = v1[1];
	p1.c.r = 255;
	p1.c.g = 255;
	p1.c.b = 255;
	p1.c.a = 0;

	p2.x = v2[0];
	p2.y = v2[1];
	p2.c.r = 255;
	p2.c.g = 255;
	p2.c.b = 255;
	p2.c.a = 0;
	
	drawLine(p1, p2);
}

void display_with_pipeline(void)
{
	glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 2.0f);
	glm::vec3 camera_lookat = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

	///////////////////////////////////////////////////////////////////////////
	// Desenha o modelo carregado utilizando linhas brancas.
	// Cada volta do loop desenha um triangulo, formado por tres linhas. 
	// Cada linha eh formada por 2 pontos (inicial e final).
	///////////////////////////////////////////////////////////////////////////
	glViewport(0, 0, ViewPortWidth, ViewPortHeight);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
  	glLoadIdentity();
	gluPerspective(55.0f, 				   // angulo de abertura da camera
		       (float)ViewPortWidth/ViewPortWidth, // aspecto da imagem
		       1.0f, 				   // near plane
		       50.0f);				   // far plane

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camera_pos[0], camera_pos[1], camera_pos[2], // posição da câmera
		  camera_lookat[0], camera_lookat[1], camera_lookat[2], // ponto para o qual a camera está olhando
		  camera_up[0], camera_up[1], camera_up[2]);   // vetor "up"

	glm::vec4 v1, v2;

	for(int i=0; i<objData->faceCount; i++) {
 	        obj_face *o = objData->faceList[i];

	        v1 = glm::vec4(objData->vertexList[o->vertex_index[0]]->e[0],
		               objData->vertexList[o->vertex_index[0]]->e[1],
		               objData->vertexList[o->vertex_index[0]]->e[2],
		               1.0f);
	        v2 = glm::vec4(objData->vertexList[o->vertex_index[1]]->e[0],
		               objData->vertexList[o->vertex_index[1]]->e[1],
		               objData->vertexList[o->vertex_index[1]]->e[2],
		               1.0f);
		v1 = pipeline(camera_pos, camera_lookat, camera_up, v1);
		v2 = pipeline(camera_pos, camera_lookat, camera_up, v2);
		DrawFunc(v1, v2);

	        v1 = glm::vec4(objData->vertexList[o->vertex_index[1]]->e[0],
		               objData->vertexList[o->vertex_index[1]]->e[1],
		               objData->vertexList[o->vertex_index[1]]->e[2],
		               1.0f);
	        v2 = glm::vec4(objData->vertexList[o->vertex_index[2]]->e[0],
		               objData->vertexList[o->vertex_index[2]]->e[1],
		               objData->vertexList[o->vertex_index[2]]->e[2],
		               1.0f);
		v1 = pipeline(camera_pos, camera_lookat, camera_up, v1);
		v2 = pipeline(camera_pos, camera_lookat, camera_up, v2);
		DrawFunc(v1, v2);

	        v1 = glm::vec4(objData->vertexList[o->vertex_index[2]]->e[0],
		               objData->vertexList[o->vertex_index[2]]->e[1],
		               objData->vertexList[o->vertex_index[2]]->e[2],
		               1.0f);
	        v2 = glm::vec4(objData->vertexList[o->vertex_index[0]]->e[0],
		               objData->vertexList[o->vertex_index[0]]->e[1],
		               objData->vertexList[o->vertex_index[0]]->e[2],
		               1.0f);
		v1 = pipeline(camera_pos, camera_lookat, camera_up, v1);
		v2 = pipeline(camera_pos, camera_lookat, camera_up, v2);
		DrawFunc(v1, v2);
        }

	// Copia o framebuffer para a textura.
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, FBptr);

	glEnable(GL_TEXTURE_2D);

	// Desenha o quadrilátero com a textura mapeada
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f( 1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f( 1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);	
		glVertex3f( 1.0f, 1.0f, 0.0f);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	glFlush();
	glutSwapBuffers();
	glutPostRedisplay();
}

void exitprog(void)
{
	// Libera a memória referente ao framebuffer.
	if (!FBptr)
		delete [] FBptr;

	std::clog << "Exiting...\n";
}

void InitOpenGL(int *argc, char **argv)
{
	glutInit(argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(IMAGE_WIDTH, IMAGE_HEIGHT);
	glutInitWindowPosition(100,100);
	glutCreateWindow("My OpenGL");

	// Ajusta a projeção ortográfica.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void InitCallBacks(void)
{
	atexit( exitprog );
	glutDisplayFunc(display_with_pipeline);
}

void InitDataStructures(void)
{
	// Aloca o framebuffer e inicializa suas posições com 0.
	FBptr = new unsigned char[IMAGE_WIDTH * IMAGE_HEIGHT * 5];
	
	for (unsigned int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT ; i++)
	{
		FBptr[i*4]   = 0;
		FBptr[i*4+1] = 0;
		FBptr[i*4+2] = 0;
		FBptr[i*4+3] = 255;
	}

	// Cria uma textura 2D, RGBA (8 bits por componente).
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
// Funcao que imprime as coordenadas de um vertice. 
// Pode ser utilizada para fazer debug de código.
void printVector(obj_vector *v)
{
	printf("%.2f,", v->e[0]);
	printf("%.2f,", v->e[1]);
	printf("%.2f  ", v->e[2]);
}

//-----------------------------------------------------------------------------
// Esta funcao apenas imprime informacoes referentes ao modelo que foi carregado, 
// tais como numero de vertices, normais, fontes de luz, etc.
void PrintModelInfo(objLoader* ptr)
{
	printf("Number of vertices: %i\n", ptr->vertexCount);
	printf("Number of vertex normals: %i\n", ptr->normalCount);
	printf("Number of texture coordinates: %i\n", ptr->textureCount);
	printf("\n");
	
	printf("Number of faces: %i\n", ptr->faceCount);
	for(int i=0; i<ptr->faceCount; i++)
	{
		obj_face *o = ptr->faceList[i];
		printf(" face ");
		for(int j=0; j<3; j++)
		{
			printVector(ptr->vertexList[ o->vertex_index[j] ]);
		}
		printf("\n");
	}

	printf("\n");
	
	printf("Number of spheres: %i\n", ptr->sphereCount);
	for(int i=0; i<ptr->sphereCount; i++)
	{
		obj_sphere *o = ptr->sphereList[i];
		printf(" sphere ");
		printVector(ptr->vertexList[ o->pos_index ]);
		printVector(ptr->normalList[ o->up_normal_index ]);
		printVector(ptr->normalList[ o->equator_normal_index ]);
		printf("\n");
	}

	printf("\n");
	
	printf("Number of planes: %i\n", ptr->planeCount);
	for(int i=0; i<ptr->planeCount; i++)
	{
		obj_plane *o = ptr->planeList[i];
		printf(" plane ");
		printVector(ptr->vertexList[ o->pos_index ]);
		printVector(ptr->normalList[ o->normal_index]);
		printVector(ptr->normalList[ o->rotation_normal_index]);
		printf("\n");
	}

	printf("\n");
	
	printf("Number of point lights: %i\n", ptr->lightPointCount);
	for(int i=0; i<ptr->lightPointCount; i++)
	{
		obj_light_point *o = ptr->lightPointList[i];
		printf(" plight ");
		printVector(ptr->vertexList[ o->pos_index ]);
		printf("\n");
	}

	printf("\n");
	
	printf("Number of disc lights: %i\n", ptr->lightDiscCount);
	for(int i=0; i<ptr->lightDiscCount; i++)
	{
		obj_light_disc *o = ptr->lightDiscList[i];
		printf(" dlight ");
		printVector(ptr->vertexList[ o->pos_index ]);
		printVector(ptr->normalList[ o->normal_index ]);
		printf("\n");
	}

	printf("\n");
	
	printf("Number of quad lights: %i\n", ptr->lightQuadCount);
	for(int i=0; i<ptr->lightQuadCount; i++)
	{
		obj_light_quad *o = ptr->lightQuadList[i];
		printf(" qlight ");
		printVector(ptr->vertexList[ o->vertex_index[0] ]);
		printVector(ptr->vertexList[ o->vertex_index[1] ]);
		printVector(ptr->vertexList[ o->vertex_index[2] ]);
		printVector(ptr->vertexList[ o->vertex_index[3] ]);
		printf("\n");
	}

	printf("\n");
	
	if(ptr->camera != NULL)
	{
		printf("Found a camera\n");
		printf(" position: ");
		printVector(ptr->vertexList[ ptr->camera->camera_pos_index ]);
		printf("\n looking at: ");
		printVector(ptr->vertexList[ ptr->camera->camera_look_point_index ]);
		printf("\n up normal: ");
		printVector(ptr->normalList[ ptr->camera->camera_up_norm_index ]);
		printf("\n");
	}

	printf("\n");

	printf("Number of materials: %i\n", ptr->materialCount);
	for(int i=0; i<ptr->materialCount; i++)
	{
		obj_material *mtl = ptr->materialList[i];
		printf(" name: %s", mtl->name);
		printf(" amb: %.2f ", mtl->amb[0]);
		printf("%.2f ", mtl->amb[1]);
		printf("%.2f\n", mtl->amb[2]);

		printf(" diff: %.2f ", mtl->diff[0]);
		printf("%.2f ", mtl->diff[1]);
		printf("%.2f\n", mtl->diff[2]);

		printf(" spec: %.2f ", mtl->spec[0]);
		printf("%.2f ", mtl->spec[1]);
		printf("%.2f\n", mtl->spec[2]);
		
		printf(" reflect: %.2f\n", mtl->reflect);
		printf(" trans: %.2f\n", mtl->trans);
		printf(" glossy: %i\n", mtl->glossy);
		printf(" shiny: %i\n", mtl->shiny);
		printf(" refact: %.2f\n", mtl->refract_index);

		printf(" texture: %s\n", mtl->texture_filename);
		printf("\n");
	}

	printf("\n");
	
	//vertex, normal, and texture test
	if(ptr->textureCount > 2 && ptr->normalCount > 2 && ptr->faceCount > 2)
	{
		printf("Detailed face data:\n");

		for(int i=0; i<3; i++)
		{
			obj_face *o = ptr->faceList[i];
			printf(" face ");
			for(int j=0; j<3; j++)
			{
				printf("%i/", o->vertex_index[j] );
				printf("%i/", o->texture_index[j] );
				printf("%i ", o->normal_index[j] );
			}
			printf("\n");
		}
	}
}

//-----------------------------------------------------------------------------
// Libera a memoria do objeto responsavel por guardar dados do modelo.
void FreeMemFunc(void)
{
	std::clog << "Exiting...\n";

	if (!objData)
		delete objData;
}

//-----------------------------------------------------------------------------
void run_without_pipeline(int *argc, char **argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(ViewPortWidth, ViewPortHeight);
	glutInitWindowPosition(100,100);
	glutCreateWindow("OBJ Loader");

	glutDisplayFunc(display_without_pipeline);

	atexit(FreeMemFunc);

	glutMainLoop();
}

void run_with_pipeline(int *argc, char **argv)
{
	// Inicializações.
	InitOpenGL(argc, argv);
	InitCallBacks();
	InitDataStructures();

	// Ajusta a função que chama as funções do mygl.h
	DrawFunc = MyGlDraw;	

	// Framebuffer scan loop.
	glutMainLoop();
}


// Programa principal
int main(int argc, char **argv)
{
	objData = new objLoader();		// cria o objeto que carrega o modelo
	objData->load("monkey_head2.obj");	// a carga do modelo é indicada atraves do nome do arquivo. 
						// Neste caso, deve ser sempre do tipo OBJ.

	// Habilite esta função se você deseja imprimir na tela dados do modelo
	// gerados durante a sua carga.
	//PrintModelInfo(objData);

	// Executa o programa sem chamar o pipeline implementado manualmente
	//run_without_pipeline(&argc, argv);
	
	// Executa o programa chamando o pipeline
	run_with_pipeline(&argc, argv);

	return 0;
}

