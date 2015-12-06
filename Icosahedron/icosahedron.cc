// Draw an Icosahedron
// ECE4122/6122 Project 4
// Blake Bergqiust

#include <iostream>
#include <math.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

#define NFACE 20
#define NVERTEX 12

#define X .525731112119133606 
#define Z .850650808352039932

void subdivide(GLfloat* v1, GLfloat* v2, GLfloat* v3, int depth);

static int updateRate = 50;
int winW, winH;
int glbl_argc;
char** glbl_argv;

// These are the 12 vertices for the icosahedron
static GLfloat vdata[NVERTEX][3] = {    
   {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},    
   {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},    
   {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0} 
};

// These are the 20 faces.  Each of the three entries for each 
// vertex gives the 3 vertices that make the face.
static GLint tindices[NFACE][3] = { 
   {0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},    
   {8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},    
   {7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6}, 
   {6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

int testNumber; // Global variable indicating which test number is desired

void drawTriangle(GLfloat* v1, GLfloat* v2, GLfloat* v3)
{
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_TRIANGLES);
  glVertex3fv(v1);
  glVertex3fv(v2);
  glVertex3fv(v3);
  glEnd();
  glColor3f(1.0, 1.0, 1.0);
  glLineWidth(2.0);
  glBegin(GL_LINE_LOOP);
  glVertex3fv(v1);
  glVertex3fv(v2);
  glVertex3fv(v3);
  glEnd();
}

void drawIcosahedron(int depth)
{
  for (int i = 0; i < NFACE; i++)
  {
//    drawTriangle(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]]);
    subdivide(vdata[tindices[i][0]], vdata[tindices[i][1]], vdata[tindices[i][2]], depth);
  }
}

void normalize(GLfloat v[3])
{
  GLfloat length = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
  v[0] /= length;
  v[1] /= length;
  v[2] /= length;
}

void subdivide(GLfloat* v1, GLfloat* v2, GLfloat* v3, int depth)
{
  if (depth > 0)
  {
    GLfloat v12[] = {v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]};
    normalize(v12);
    GLfloat v13[] = {v1[0] + v3[0], v1[1] + v3[1], v1[2] + v3[2]};
    normalize(v13);
    GLfloat v23[] = {v3[0] + v2[0], v3[1] + v2[1], v3[2] + v2[2]};
    normalize(v23);
    subdivide(v1, v12, v13, depth - 1);
    subdivide(v2, v12, v23, depth - 1);
    subdivide(v3, v13, v23, depth - 1);
    subdivide(v12, v13, v23, depth - 1);
  }
  else
  {
    drawTriangle(v1, v2, v3);
  }
}
 
// Test cases.  Fill in your code for each test case
void Test1()
{
  drawIcosahedron(0);
}

void Test2()
{
  static GLfloat rotX = 0.0;
  static GLfloat rotY = 0.0;
  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  rotX += 1.0;
  rotY -= 1.0;
  Test1();
}

void Test3()
{
  drawIcosahedron(1);
}

void Test4()
{
  static GLfloat rotX = 0.0;
  static GLfloat rotY = 0.0;
  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  rotX += 1.0;
  rotY -= 1.0;
  Test3();
}

void Test5(int depth)
{
  drawIcosahedron(depth);
}

void Test6(int depth)
{
  static GLfloat rotX = 0.0;
  static GLfloat rotY = 0.0;
  glRotatef(rotX, 1.0, 0.0, 0.0);
  glRotatef(rotY, 0.0, 1.0, 0.0);
  rotX += 1.0;
  rotY -= 1.0;
  Test5(depth);
}

void display(void)
{
  glEnable(GL_LINE_SMOOTH | GL_DEPTH_TEST);
  // clear all
  glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
  // clear the matrix
  glLoadIdentity();
  // Translate to center
  glTranslatef(winW/2, winH/2, 0);
  glScalef(200.0, 200.0, 200.0);
  // Switch statement for Test
  int depth;
  switch(testNumber)
  {
    case 1 :
      Test1();
      break;
    case 2 :
      Test2();
      break;
    case 3 :
      Test3();
      break;
    case 4 :
      Test4();
      break;
    case 5 :
      if (glbl_argc < 3)
      {
        std::cout << "Missing depth argument" << endl;
        exit(1);
      }
      depth = atoi(glbl_argv[2]);
      Test5(depth);
      break;
    case 6 :
      if (glbl_argc < 3)
      {
        std::cout << "Missing depth argument" << endl;
        exit(1);
      }
      depth = atoi(glbl_argv[2]);
      Test6(depth);
      break;
    default :
      Test1();
  }
  glPopMatrix();
  glutSwapBuffers();
}

void init()
{
  // select clearing (background) color
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  winW = glutGet(GLUT_WINDOW_WIDTH);
  winH = glutGet(GLUT_WINDOW_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, (GLdouble)winW, (GLdouble)0.0, winH, (GLdouble)winW * 2, (GLdouble)-winW * 2);
  //glFrustum(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)1, (GLdouble)40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void reshape(int w, int h)
{
  glViewport(0,0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)w * 2, (GLdouble)-w * 2);
  //glFrustum(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)1, (GLdouble)40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  winH = h;
  winW = w;
}

void timer(int)
{
  glutPostRedisplay();
  glutTimerFunc(1000.0 / 10, timer, 0);
}

int main(int argc, char** argv)
{
  glbl_argc = argc;
  glbl_argv = argv;
  if (argc < 2)
    {
      std::cout << "Usage: icosahedron testnumber" << endl;
      exit(1);
    }
  // Set the global test number
  testNumber = atol(argv[1]);
  // Initialize glut  and create your window here
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Icosahedron");
  init();
  // Set your glut callbacks here
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutTimerFunc(1000.0 / updateRate, timer, 0);
  // Enter the glut main loop here
  glutMainLoop();
  return 0;
}

