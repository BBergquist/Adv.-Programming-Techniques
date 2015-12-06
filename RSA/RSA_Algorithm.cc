// RSA Assignment for ECE4122/6122 Fall 2015

#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "RSA_Algorithm.h"

using namespace std;
mpz_class GenerateRandomKeyPair(size_t sz);
gmp_randclass rng1(gmp_randinit_default);
mpz_class Encrypt(mpz_class M);
mpz_class Decrypt(mpz_class C);
mpz_class n;
mpz_class d;
mpz_class e;

// Implement the RSA_Algorithm methods here

// Constructor
RSA_Algorithm::RSA_Algorithm()
  : rng(gmp_randinit_default)
{
  // get a random seed for the random number generator
  int dr = open("/dev/random", O_RDONLY);
  if (dr < 0)
    {
      cout << "Can't open /dev/random, exiting" << endl;
      exit(0);
    }
  unsigned long drValue;
  read(dr, (char*)&drValue, sizeof(drValue));
  //cout << "drValue " << drValue << endl;
  rng.seed(drValue);
// No need to init n, d, or e.
}

// Fill in the remainder of the RSA_Algorithm methods

// Generates a random prime number of size sz bits
mpz_class GeneratePrimeNumber(size_t sz)
{
  int check;
  mpz_class bits(sz);
  mpz_class returned;
  do
  {
    returned = rng1.get_z_bits(bits);
    check = mpz_probab_prime_p(returned.get_mpz_t(), 100);
  }
  while (check == 0);
  return returned;
}

// Generate key
void RSA_Algorithm::GenerateRandomKeyPair(size_t sz)
{
  mpz_class p;
  mpz_class q;
  p = GeneratePrimeNumber(sz);
  q = GeneratePrimeNumber(sz);
  n = p * q;
  mpz_class phi = (p - 1) * (q - 1);
  mpz_class gcd;
  mpz_class one(1);
  mpz_class bits(sz * 2);
  do
  {
    d = rng1.get_z_bits(bits);
    mpz_gcd(gcd.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
  }
  while (d >= phi || gcd != one);
  int result = mpz_invert(e.get_mpz_t(), d.get_mpz_t(), phi.get_mpz_t());
}

// Encrypt plantext message M with key pair n/e
// By convention, we will make the encryption key e the public key
// and the decryption key d the private key.
mpz_class RSA_Algorithm::Encrypt(mpz_class M)
{
  // C = M^e mod n
  mpz_class C;
  mpz_powm(C.get_mpz_t(), M.get_mpz_t(), e.get_mpz_t(), n.get_mpz_t());
  return C;
}

// Decrypt ciphertext message C with key pair n/d
mpz_class RSA_Algorithm::Decrypt(mpz_class C)
{
  // M = C^d mod n
  mpz_class M;
  mpz_powm(M.get_mpz_t(), C.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
  return M;
}

void RSA_Algorithm::PrintND()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << endl;
}

void RSA_Algorithm::PrintNE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " e " << e << endl;
}

void RSA_Algorithm::PrintNDE()
{ // Do not change this, right format for the grading script
  cout << "n " << n << " d " << d << " e " << e << endl;
}

void RSA_Algorithm::PrintM(mpz_class M)
{ // Do not change this, right format for the grading script
  cout << "M " << M << endl;
}

void RSA_Algorithm::PrintC(mpz_class C)
{ // Do not change this, right format for the grading script
  cout << "C " << C << endl;
}




