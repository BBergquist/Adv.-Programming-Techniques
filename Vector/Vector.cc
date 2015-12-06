// Implementation of the templated Vector class
// ECE4122/8122
// Blake Bergquist

#include <iostream> // debugging
#include "Vector.h"

// Your implementation here
// Fill in all the necessary functions below
using namespace std;

// Default constructor
template <typename T>
Vector<T>::Vector()
{
  count = 0;
  reserved = 0;
  elements = (T*)malloc(0);
}

// Copy constructor
template <typename T>
Vector<T>::Vector(const Vector& rhs)
{
  count = rhs.count;
  reserved = rhs.reserved;
  elements = (T*)malloc(count * sizeof(T));
  for(int i = 0; i < count; i++)
  {
    new (&elements[i]) T(rhs.elements[i]);
  }
}

// Assignment operator
template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& rhs)
{
  count = rhs.count;
  reserved = rhs.reserved;
  elements = rhs.elements;
  return *this;
}

#ifdef GRAD_STUDENT
// Other constructors
template <typename T>
Vector<T>::Vector(size_t nReserved)
{ // Initialize with reserved memory
}

template <typename T>
Vector<T>::Vector(size_t n, const T& t)
{ // Initialize with "n" copies of "t"
}
#endif

// Destructor
template <typename T>
Vector<T>::~Vector()
{
  for(int i = 0; i < count; i++)
  {
    elements[i].~T();
  }
  free(elements);
}

// Add and access front and back
template <typename T>
void Vector<T>::Push_Back(const T& rhs)
{
  if (count < reserved)
  {
    new (&elements[count++]) T(rhs);
  }
  else
  {
    T* newElements = (T*)malloc(++reserved * sizeof(T));
    for (int i = 0; i < count; i++)
    {
      new (&newElements[i]) T(elements[i]);
      elements[i].~T();
    }
    free(elements);
    elements = newElements;
    new (&elements[count++]) T(rhs);
  }
}

template <typename T>
void Vector<T>::Push_Front(const T& rhs)
{
  if (count < reserved)
  {
    for(int i = count; i > 1; i++)
    {
      new (&elements[i]) T(elements[i - 1]);
      elements[i - 1].~T();
    }
    new (&elements[0]) T(rhs);
    count++;
  }
  else
  {
    T* newElements = (T*)malloc(++reserved * sizeof(T));
    for(int i = 0; i < count; i++)
    {
      new (&newElements[i + 1]) T(elements[i]);
      elements[i].~T();
    }
    free(elements);
    elements = newElements;
    new (&elements[0]) T(rhs);
    count++;
  }
}

template <typename T>
void Vector<T>::Pop_Back()
{ // Remove last element
  elements[--count].~T();
}

template <typename T>
void Vector<T>::Pop_Front()
{ // Remove first element
  elements[0].~T();
  for(int i = 0; i < count - 1; i++)
  {
    new (&elements[i]) T(elements[i + 1]);
    elements[i + 1].~T();
  }
  count--;
}

// Element Access
template <typename T>
T& Vector<T>::Front() const
{
  return elements[0];
}

// Element Access
template <typename T>
T& Vector<T>::Back() const
{
  return elements[count - 1];
}

template <typename T>
T& Vector<T>::operator[](size_t i)
{
  return elements[i];
}

template <typename T>
size_t Vector<T>::Size() const
{
  return count;
}

template <typename T>
bool Vector<T>::Empty() const
{
  return count == 0;
}

// Implement clear
template <typename T>
void Vector<T>::Clear()
{
  for(int i = 0; i < count; i++)
  {
    elements[i].~T();
  }
  count = 0;
}

// Iterator access functions
template <typename T>
VectorIterator<T> Vector<T>::Begin() const
{
  return VectorIterator<T>(elements);
}

template <typename T>
VectorIterator<T> Vector<T>::End() const
{
  return VectorIterator<T>(elements + count);
}

#ifdef GRAD_STUDENT
// Erase and insert
template <typename T>
void Vector<T>::Erase(const VectorIterator<T>& it)
{
}

template <typename T>
void Vector<T>::Insert(const T& rhs, const VectorIterator<T>& it)
{
}
#endif

// Implement the iterators

// Constructors
template <typename T>
VectorIterator<T>::VectorIterator()
{
  current = NULL;
}

template <typename T>
VectorIterator<T>::VectorIterator(T* c)
{
  current = c;
}

// Copy constructor
template <typename T>
VectorIterator<T>::VectorIterator(const VectorIterator<T>& rhs)
{
  current = rhs.current;
}

// Iterator defeferencing operator
template <typename T>
T& VectorIterator<T>::operator*() const
{
  return *current;
}

// Prefix increment
template <typename T>
VectorIterator<T>  VectorIterator<T>::operator++()
{
  current++;
  VectorIterator<T> result(current);
  return result;
}

// Postfix increment
template <typename T>
VectorIterator<T> VectorIterator<T>::operator++(int)
{
  VectorIterator<T> result(current);
  current++;
  return result;
}

// Comparison operators
template <typename T>
bool VectorIterator<T>::operator !=(const VectorIterator<T>& rhs) const
{
  return current != rhs.current;
}

template <typename T>
bool VectorIterator<T>::operator ==(const VectorIterator<T>& rhs) const
{
  return current == rhs.current;
}




