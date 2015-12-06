/* 
 * File:   MBSet.cu
 * 
 * Created on June 24, 2012
 * 
 * Purpose:  This program displays Mandelbrot set using the GPU via CUDA and
 * OpenGL immediate mode.
 * 
 */

#include <iostream>
#include <stack>
#include <cuda_runtime_api.h>
#include <stdio.h>
#include "Complex.cu"

#include <GL/freeglut.h>

// Size of window in pixels, both width and height
#define WINDOW_DIM            512

using namespace std;

// Initial screen coordinates, both host and device.
Complex minC(-2.0, -1.2);
Complex maxC(1.0, 1.8);
Complex* dev_minC;
Complex* dev_maxC;
char* pixelBuf = new char[WINDOW_DIM * WINDOW_DIM * 3];
const int maxIt = 2000; // Maximum Iterations

// Define the RGB Class
class RGB
{
public:
  RGB()
    : r(0), g(0), b(0) {}
  RGB(double r0, double g0, double b0)
    : r(r0), g(g0), b(b0) {}
public:
  double r;
  double g;
  double b;
};

RGB* colors = 0; // Array of color values

__global__ void devDrawPix(float* realRange, float* imagRange, Complex* min, int* pixel)
{
  int threadId = threadIdx.x + blockIdx.x * blockDim.x;
  int x = threadId % WINDOW_DIM;
  int y = threadId / WINDOW_DIM;

  Complex cur(min->r + (x * *realRange)/WINDOW_DIM,
              min->i + (y * *imagRange)/WINDOW_DIM);
  Complex z(cur);
  for (pixel[threadId] = 0; pixel[threadId] < 2000; pixel[threadId]++)
  {
    z = z*z + cur;
    if (z.magnitude2() > 4.0)
    {
      break;
    }
  }
}

void display(void)
{
  float realRange, imagRange; // Host copies
  int* iters; // Host copy
  float* d_realRange,* d_imagRange; // Device copies
  int* d_iters; // Devide copy
  int size = WINDOW_DIM*WINDOW_DIM*sizeof(int);
  // Allocate memory for host
  realRange = maxC.r - minC.r;
  imagRange = maxC.i - minC.i;
  iters = (int*) malloc(size);
  // Allocate memory for device
  cudaMalloc((void **)&d_realRange, sizeof(float));
  cudaMalloc((void **)&d_imagRange, sizeof(float));
  cudaMalloc((void **)&d_iters, size);
  cudaMalloc((void **)&dev_minC, sizeof(Complex));
  // Copy inputs to device
  cudaMemcpy(d_realRange, &realRange, sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(d_imagRange, &imagRange, sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(dev_minC, &minC, sizeof(Complex), cudaMemcpyHostToDevice);

  // Calculate pixels...
  devDrawPix<<<WINDOW_DIM*WINDOW_DIM/32,32>>>(d_realRange, d_imagRange, dev_minC, d_iters);

  // Copy results
  cudaMemcpy(iters, d_iters, size, cudaMemcpyDeviceToHost);

  // Populate pixelBuf based off of iterations
  for (int i = 0; i < WINDOW_DIM*WINDOW_DIM; i++)
  {
    pixelBuf[i*3] = (char) (colors[iters[i]].r * 255);
    pixelBuf[i*3 + 1] = (char) (colors[iters[i]].g * 255);
    pixelBuf[i*3 + 2] = (char) (colors[iters[i]].b * 255);
  }

  // Cleanup
  cudaFree(d_realRange); cudaFree(d_imagRange); cudaFree(d_iters); cudaFree(dev_minC);
  free(iters);
  
  glDrawPixels(WINDOW_DIM,WINDOW_DIM,GL_RGB,GL_UNSIGNED_BYTE,pixelBuf);
  glutSwapBuffers();
  glutPostRedisplay();
}

void init()
{
  glClearColor(0,0,0,0);
  glShadeModel(GL_FLAT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}

//void reshape(int w, int h)
//{
//  glViewport(0,0,(GLsizei)w,(GLsizei)h);
//  glMatrixMode(GL_PROJECTION);
//  glLoadIdentity();
//  glOrtho(0.0,(GLdouble)w,(GLdouble)0.0,h,(GLdouble)w*2,(GLdouble)-w*2);
//  glMatrixMode(GL_MODELVIEW);
//  glLoadIdentity();
//}

void InitializeColors()
{
  colors = new RGB[maxIt + 1];
  for (int i = 0; i < maxIt; ++i)
    {
      if (i < 5)
        { // Try this.. just white for small it counts
          colors[i] = RGB(1, 1, 1);
        }
      else
        {
          srand48(333333333333333333 * drand48());
          colors[i] = RGB(drand48(), drand48(), drand48());
        }
    }
  colors[maxIt] = RGB(); // black
}

int main(int argc, char** argv)
{
  // Initialize OPENGL here
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOW_DIM, WINDOW_DIM);
  glutInitWindowPosition(50, 50);
  glutCreateWindow("Mandelbrot Set");
  init();
  // Set up necessary host and device buffers
  // set up the opengl callbacks for display, mouse and keyboard
  glutDisplayFunc(display);
//  glutReshapeFunc(reshape);
  // Calculate the interation counts
  // Grad students, pick the colors for the 0 .. 1999 iteration count pixels
  InitializeColors();
  glutMainLoop(); // THis will callback the display, keyboard and mouse
  return 0;
  
}
