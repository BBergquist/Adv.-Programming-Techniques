// Threaded two-dimensional Discrete FFT transform
// Blake Bergquist
// ECE4122 Project 2


#include <iostream>
#include <string>
#include <math.h>
#include <stdlib.h>

#include "Complex.h"
#include "InputImage.h"

// You will likely need global variables indicating how
// many threads there are, and a Complex* that points to the
// 2d image being transformed.
unsigned int N;
unsigned int nThreads;
Complex* imageArr;
Complex* W;
pthread_mutex_t exitMutex;
pthread_cond_t exitCond;
int startCount;
pthread_mutex_t startCountMutex;
pthread_barrier_t barrier;

void ComputeWValues(Complex* W, int length);
void Transpose(Complex* arr, int N, int startRow);
void MyBarrier_Init(pthread_barrier_t* barr, unsigned numThreads);
void MyBarrier(pthread_barrier_t* barr);

using namespace std;

// Function to reverse bits in an unsigned integer
// This assumes there is a global variable N that is the
// number of points in the 1D transform.
unsigned ReverseBits(unsigned v)
{ //  Provided to students
  unsigned n = N; // Size of array (which is even 2 power k value)
  unsigned r = 0; // Return value
   
  for (--n; n > 0; n >>= 1)
    {
      r <<= 1;        // Shift return value
      r |= (v & 0x1); // Merge in next bit
      v >>= 1;        // Shift reversal value
    }
  return r;
}

// GRAD Students implement the following 2 functions.
// Undergrads can use the built-in barriers in pthreads.

// Call MyBarrier_Inunit once in main
void MyBarrier_Init(pthread_barrier_t* barr, unsigned numThreads)
{
  pthread_barrier_init(barr, NULL, numThreads);
}

// Each thread calls MyBarrier after completing the row-wise DFT
void MyBarrier(pthread_barrier_t* barr) // Again likely need parameters
{
  pthread_barrier_wait(barr);
}

void Transpose(Complex* arr, int width, int startRow)
{
  // It is assumed that the given array is a 1D representation of
  // a square, 2d array. "arr" is the array that is to be transposed
  // and N is the width/height of the square array
  for (int row = startRow; row - startRow < width / (int) nThreads; row++)
  {
    for (int col = 0; col < width; col++)
    {
      if (row < col)
      {
        Complex swap = arr[row*width + col];
        arr[row * width + col] = arr[col * width + row];
        arr[col * width + row] = swap;
      }
    }
  }
}
                    
void Transform1D(Complex* h, int N)
{
  // Implement the efficient Danielson-Lanczos DFT here.
  // "h" is an input/output parameter
  // "N" is the size of the array (assume even power of 2)

  // Reorder the array (Step 1)
  for (int i = 0; i < N; i++)
  {
    int i2 = ReverseBits(i);
    if (i2 < i)
    {
      Complex swap = h[i2];
      h[i2] = h[i];
      h[i] = swap;
    }
  }
  // Do the transform (Step 3)
  for (int i = 2; i <= N; i *= 2)
  {
    for (int j = 0; j < N; j += i)
    {
      for (int k = j; k < j + i/2; k++)
      {
        Complex even = h[k];
        Complex odd = h[k + i/2];
        h[k] = even + W[(k * N / i) % N] * odd;
        h[k + i/2] = even - W[(k * N / i) % N] * odd;
      }
    }
  }
}

void ComputeWValues(Complex* W, int length)
{
  // Method used to precompute W values
  // "W" is the output parameter
  // "length" is the number of values to be calculated. Assumed to
  // be power of 2.

  for (int i = 0; i < length/2; i++)
  {
    Complex next(cos(2*M_PI*i/length), -sin(2*M_PI*i/length));
    Complex mag = next.Mag();
    if (mag.real < pow(10, -7))
    {
      next.real = 0;
      next.imag = 0;
    }
    W[i] = next;
    W[i + length/2] = W[i + length/2] - next;
  }
}

void* Transform2DThread(void* v)
{ // This is the thread startign point.  "v" is the thread number
  unsigned long id = (unsigned long) v;
  // 1d DFT variables
  unsigned int rowsPerThread = N/nThreads;
  unsigned int startingRow = id * rowsPerThread;
  // Calculate 1d DFT for assigned rows
  Complex* thisRow = imageArr + startingRow * N;
  for (unsigned int i = 0; i < rowsPerThread; i++)
  {
    Transform1D(thisRow, N);
    thisRow += N;
  }
  // Barrier to wait for threads to complete 1D transform
  MyBarrier(&barrier);
  Transpose(imageArr, N, startingRow);
  // Barrier to wait for threads to complete transpose
  MyBarrier(&barrier);
  // Calculate 1d DFT for assigned cols
  thisRow = imageArr + startingRow * N;
  for (unsigned int i = 0; i < rowsPerThread; i++)
  {
    Transform1D(thisRow, N);
    thisRow += N;
  }
  // Barrier to wait for threads to complete 1D transform
  MyBarrier(&barrier);
  Transpose(imageArr, N, startingRow);
  // wait for all to complete
  // Calculate 1d DFT for assigned columns
  // Decrement active count and signal main if all complete
  pthread_mutex_lock(&startCountMutex);
  startCount--;
  pthread_mutex_unlock(&startCountMutex);
  if (startCount == 0)
  { // Last to exit, notify main
    pthread_mutex_lock(&exitMutex);
    pthread_cond_signal(&exitCond);
    pthread_mutex_unlock(&exitMutex);
  }
  return 0;
}

void Transform2D(const char* inputFN) 
{ // Do the 2D transform here.
  InputImage image(inputFN);  // Create the helper object for reading the image
  N = image.GetWidth();
  // Create the global pointer to the image array data
  imageArr = image.GetImageData();
  // Precompute the W values (Step 2)
  Complex W_init[N];
  ComputeWValues(W_init, N);
  W = W_init;
  // pthread initialization
  pthread_mutex_init(&exitMutex, 0);
  pthread_mutex_init(&startCountMutex, 0);
  pthread_cond_init(&exitCond, 0);
  pthread_mutex_init(&exitMutex, 0);
  MyBarrier_Init(&barrier, nThreads);
  startCount = nThreads;
  // Create threads
  for (unsigned int threadNum = 0; threadNum < nThreads; threadNum++)
  {
    pthread_t pt;
    pthread_create(&pt, 0, Transform2DThread, (void*) threadNum);
  }
  // Wait for all threads complete
  pthread_cond_wait(&exitCond, &exitMutex);
  // Write the transformed data
  image.SaveImageData("Tower-DFT2d.txt", imageArr, N, N);
}

int main(int argc, char** argv)
{
  string fn("Tower.txt"); // default file name
  if (argc > 1) fn = string(argv[1]);  // if name specified on cmd line
  nThreads = 16;
  if (argc > 2) nThreads = atoi(argv[2]);
  Transform2D(fn.c_str()); // Perform the transform.
}  
  

  
