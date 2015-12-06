// Distributed two-dimensional Discrete FFT transform
// Blake Bergquist
// ECE4122 Project 1


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>

#include "Complex.h"
#include "InputImage.h"

using namespace std;

void Transform1D(Complex* h, int N, Complex* H);
void Transpose(Complex* arr, int N);
void Transform2D(Complex* h, int N);
void Distribute(Complex* arr, int N, int source);

int numtasks, rank, rc;

void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  // 1) Use the InputImage object to read in the Tower.txt file and
  //    find the width/height of the input image.
  InputImage image(inputFN);  // Create the helper object for reading the image
  int N = image.GetWidth();
  // 2) Use MPI to find how many CPUs in total, and which one
  //    this process is
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  // 3) Obtain a pointer to the Complex 1d array of input data
  Complex* h = image.GetImageData();
  Transform2D(h, N);
  if (rank == 0)
  {
    image.SaveImageData("MyAfter1d.txt",h,N,N);
    Transpose(h,N);
  }
  Distribute(h, N, 0);
  Transform2D(h, N);
  if (rank == 0)
  {
    Transpose(h,N);
    image.SaveImageData("MyAfter2d.txt",h,N,N);
  }
}

void Distribute(Complex* arr, int N, int source)
{
  // A method to distribute an array from the cpu designated by source to
  // the rest of the cpus. It is assumed that the Array arr is a 1D
  // representation of a square 2D array of height and width N.
  if (rank == source)
  {
    for (int i = 0; i < numtasks; i++)
    {
      if (i != source)
      {
        rc = MPI_Send(arr, N*N*sizeof(Complex), MPI_CHAR, i, 0,
                      MPI_COMM_WORLD);
        if (rc != MPI_SUCCESS)
        {
          MPI_Finalize();
          exit(1);
        }
      }
    }
  }
  else
  {
    MPI_Status status;
    rc = MPI_Recv(arr, N*N*sizeof(Complex), MPI_CHAR, source, 0,
                  MPI_COMM_WORLD, &status);
    if (rc != MPI_SUCCESS)
    {
      MPI_Finalize();
      exit(1);
    }
  }
}

void Transform2D(Complex* h, int N)
{
  Complex* orig_h = h;
  // 4) Allocate an array of Complex object of sufficient size to
  //    hold the 2d DFT results (size is width * height)
  Complex arr_H[N*N/numtasks];
  Complex* H = arr_H;
  // 5) Do thindividual 1D transforms on the rows assigned to your CPU
  h += rank*N*N/numtasks;
  for (int i = 0; i < N/numtasks; i++)
  {
    Transform1D(h,N,H);
    h += N;
    H += N;
  }
  H = arr_H;
  // 6) Send the resultant transformed values to the appropriate
  //    other processors for the next phase.
  if (rank != 0)
  {
    rc = MPI_Send(H, sizeof(arr_H), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    if (rc != MPI_SUCCESS)
    {
      MPI_Finalize();
      exit(1);
    }
  }
  //   6a) To send and receive columns, you might need a separate
  //       Complex array of the correct size.
  // 7) Receive messages from other processes to collect your columns
  if (rank == 0)
  {
    h = orig_h;
    for(int i = 0; i < numtasks; i++)
    {
      if (i != 0)
      {
        MPI_Status status;
        rc = MPI_Recv(H, sizeof(arr_H), MPI_CHAR, i, 0, MPI_COMM_WORLD,
                      &status);
        if (rc != MPI_SUCCESS)
        {
          MPI_Finalize();
          exit(1);
        }
      }
      for(int j = 0; j < N*N/numtasks; j++)
      {
        *h = H[j];
        h++;
      }
    }
  }
  // 8) When all columns received, do the 1D transforms on the columns
  // 9) Send final answers to CPU 0 (unless you are CPU 0)
  //   9a) If you are CPU 0, collect all values from other processors
  //       and print out with SaveImageData().
}

void Transform1D(Complex* h, int N, Complex* H)
{
  // Implement a simple 1-d DFT using the double summation equation
  // given in the assignment handout.  h is the time-domain input
  // data, w is the width (N), and H is the output array.
  for(int n=0; n<N; n++)
  {
    for(int k=0; k<N; k++)
    {
      Complex W(cos(2*M_PI*n*k/N), -sin(2*M_PI*n*k/N));
      H[n] = H[n] + W*h[k];
    }
  }
}

void Transpose(Complex* arr, int N)
{
  // It is assumed that the given array is a 1D representation of
  // a square, 2d array. arr is the array that is to be transposed
  // and N is the width/height of the square array
  for(int row = 0; row < N; row++)
  {
    for(int col = 0; col < N; col++)
    {
      if(row < col)
      {
        Complex swap = arr[row*N + col];
	arr[row*N + col] = arr[col*N + row];
        arr[col*N + row] = swap;
      }
    }
  }
}

int main(int argc, char** argv)
{
  // Original Code
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  // MPI initialization here
  int rc = MPI_Init(&argc,&argv);
  if (rc != MPI_SUCCESS)
  {
    printf("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }
  Transform2D(fn.c_str()); // Perform the transform.
  // Finalize MPI here
  MPI_Finalize();
}  
