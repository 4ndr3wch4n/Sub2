/*******************************************************************
	       Multi-Part Model Construction and Manipulation
********************************************************************/

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "QuadMesh.h"

#define M_PI 3.14159265358979323846

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
VECTOR3D ScreenToWorld(int x, int y);

static GLfloat theta = 0.0;
static GLfloat thetay = 0.0;
static GLfloat spin = 0.0;
static int currentButton;
static unsigned char currentKey;

GLfloat light_position0[] = {0,  20, 22,1.0};
GLfloat light_position1[] = { 6.0,  12.0, 0.0,1.0};
GLfloat light_diffuse[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[]  = {1.0, 1.0, 1.0, 1.0};
GLfloat light_ambient[]   = {0.2, 0.2, 0.2, 1.0};

GLint viewport[4];
GLdouble modelview[16];
GLdouble projection[16];
GLfloat winX, winY, winZ;
GLdouble posX, posY, posZ;
GLint position = 0;
GLint vertice = 0;
GLint prevX = 0;
GLint prevY = 0;
bool check = false;

boolean cameraMode = false;
GLfloat previousX = 0.0;
GLfloat previousY = 0.0;
GLfloat xFrom = 0.0;
GLfloat yFrom = 20.0;
GLfloat zFrom = 22.0;
GLfloat xZoom = 1.0;
GLfloat yZoom = 1.0;
GLfloat zZoom = 1.0;

QuadMesh *groundMesh = NULL;


struct BoundingBox
{
  VECTOR3D min;
  VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 64;

int main(int argc, char **argv)
{
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Scene Modeller");

  initOpenGL(500,500);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotionHandler);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(functionKeys);
  
  glutMainLoop();
  return 0;
}



// Setup openGL */
void initOpenGL(int w, int h)
{
  // Set up viewport, projection, then change to modelview matrix mode - 
  // display function will then set up camera and modeling transforms
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1.0,0.2,80.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // Set up and enable lighting
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
  
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHT1);
  
  // Other OpenGL setup
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.6, 0.6, 0.6, 0.0);  
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  // This one is important - renormalize normal vectors 
  glEnable(GL_NORMALIZE);
  
  //Nice perspective.
  glHint(GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST);
  
  // Set up ground quad mesh
  VECTOR3D origin  = VECTOR3D(-8.0f,0.0f,8.0f);
  VECTOR3D dir1v   = VECTOR3D(1.0f, 0.0f, 0.0f);
  VECTOR3D dir2v   = VECTOR3D(0.0f, 0.0f,-1.0f);
  groundMesh = new QuadMesh(meshSize, 16);
  groundMesh->InitMesh(meshSize, origin, 16, 16, dir1v, dir2v);
  
  VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
  VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
  VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
  float shininess = 0.2;
  groundMesh->SetMaterial(ambient,diffuse,specular,shininess);

  // Set up the bounding box of the scene
  // Currently unused. You could set up bounding boxes for your objects eventually.
  BBox.min.Set(-8.0f, 0.0, -8.0);
  BBox.max.Set( 8.0f, 6.0,  8.0);
}



void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glLoadIdentity();
  
  // Set up the camera
  gluLookAt(xFrom,yFrom,zFrom,0.0,0.0,0.0,0.0,1.0,0.0);

  //Draw a sphere
  glPushMatrix();
  glScalef(xZoom, yZoom, zZoom);
  glPushMatrix();

  glPopMatrix();

  // Draw ground
  groundMesh->DrawMesh(meshSize);
  glPopMatrix();

  glutSwapBuffers();
}


// Called at initialization and whenever user resizes the window */
void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1.0,0.2,40.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}

VECTOR3D pos = VECTOR3D(0,0,0);

// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
  currentButton = button;

  switch(button)
  {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_UP)
	{

	}
	else if (state == GLUT_DOWN) //drag direction
	{
		prevX = x;
		prevY = y;
	}
	break;
  case GLUT_RIGHT_BUTTON:
    if (state == GLUT_DOWN) //create blob on right mouse click
	{
		if (cameraMode == false) //if not camera mode, create blob
		groundMesh->createBlob();
	}
	break;
  case 3: //scroll in
	  if (cameraMode == true)
	  {
		  xZoom += 0.01;
		  yZoom += 0.01;
		  zZoom += 0.01;
	  }
	  break;
  case 4: //scroll out
	  if (cameraMode == true)
	  {
		 if (xZoom >= 0.0) {
		  xZoom -= 0.01;
		  yZoom -= 0.01;
		  zZoom -= 0.01;
		}
	  }
	  break;
  default:
	break;
  }
  glutPostRedisplay();
}

// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
  if (currentButton == GLUT_LEFT_BUTTON) //allows user to drag while holding down left button
  {
	  if (cameraMode == false) { //non-camera mode
		  glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		  glGetDoublev(GL_PROJECTION_MATRIX, projection);
		  glGetIntegerv(GL_VIEWPORT, viewport);
		  winX = (float)xMouse;
		  winY = (float)viewport[3] - (float)yMouse;
		  glReadPixels(xMouse, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
		  gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ); //gets the position of 
		  float pX = posX;
		  float pZ = posZ;
		  groundMesh->moveBlobMouse(pX, pZ); //moves the blob to the mouse position

	  }
	  else //camera mode
	  {
		  theta += 0.1*(xMouse - prevX); //move movement azimuth camera
		  xFrom = 22 * -cos(theta / 360 * M_PI);
		  zFrom = 22 * -sin(theta / 360 * M_PI);
		  thetay += 0.1*(yMouse - prevY); //movement elevation camera
		  if (thetay < 200) thetay = 200;
		  if (thetay > 240) thetay = 240;
		  yFrom = 22 * -sin(thetay / 180 * M_PI);
	  }
  }
  glutPostRedisplay();
}


VECTOR3D ScreenToWorld(int x, int y)
{
	// you will need this if you use the mouse
	return VECTOR3D(0);
}// ScreenToWorld()

/* Handles input from the keyboard, non-arrow keys */
void keyboard(unsigned char key, int x, int y)
{
	if (cameraMode == false) {
		switch (key)
		{
		case 'e':
			groundMesh->createBlob(); //makes a new blob
			break;
		case 'q':
			groundMesh->change(1, 0); //makes blob wider
			break;
		case 'w':
			groundMesh->change(0, 1); //makes blob taller
			break;
		case 'a':
			groundMesh->change(-1, 0); //makes blob thinner
			break;
		case 's':
			groundMesh->change(0, -1); //makes blob shorter/valley
			break;
		}
	}

  glutPostRedisplay();
}

void functionKeys(int key, int x, int y)
{
  VECTOR3D min, max;

  if (key == GLUT_KEY_F1) //toggle camera modes
  {
	  if (cameraMode == false)
		  cameraMode = true;
	  else
		  cameraMode = false;
  }
  if (key == GLUT_KEY_F2) //toggles per vertex/per quad
  {
	  if (check == false) check = true;
	  else check = false;
	  groundMesh->quadNormal(check);
  }
 
  if (cameraMode == false) { //keyboard movements for blob
	  if (key == GLUT_KEY_UP)
	  {
		  groundMesh->moveBlob(0, -1);
	  }
	  else if (key == GLUT_KEY_DOWN)
	  {
		  groundMesh->moveBlob(0, 1);
	  }
	  else if (key == GLUT_KEY_LEFT)
	  {
		  groundMesh->moveBlob(-1, 0);
	  }
	  else if (key == GLUT_KEY_RIGHT)
	  {
		  groundMesh->moveBlob(1, 0);
	  }
	  glutPostRedisplay();
  }
}