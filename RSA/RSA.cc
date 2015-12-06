// ECE4122/6122 RSA Encryption/Decryption assignment
// Fall Semester 2015
// Name: Blake Bergquist
// Date: 10/2/15

#include <iostream>
#include "RSA_Algorithm.h"

using namespace std;
gmp_randclass rng(gmp_randinit_default);
mpz_class M, C, D;
int sz = 32;

int main()
{
  // Instantiate the one and only RSA_Algorithm object
  RSA_Algorithm RSA;
  
  int failCount = 0, totalCount = 0;
  // Loop from sz = 32 to 1024 inclusive
  for (int sz = 32; sz <= 1024; sz *= 2)
  {
    // Loop to generate 10 different key sets
    for (int i = 0; i < 10; i++)
    {
      RSA.GenerateRandomKeyPair(sz);
      RSA.PrintNDE();
      // Loop to generate 10 random messages per key set and encrypt/decrypt
      for (int j = 0; j < 10; j++)
      {
        do
        {
          M = rng.get_z_bits(sz*2);
        }
        while (M >= RSA.n);
        RSA.PrintM(M);
        C = RSA.Encrypt(M);
        RSA.PrintC(C);
        D = RSA.Decrypt(C);
        if (M != D) failCount++;
        totalCount++;
      }
    }
  }
//  cout << "Number of failed messages: " << failCount << endl;
//  cout << "Total number of messages: " << totalCount << endl;
  // for each size choose 10 different key pairs
  // For each key pair choose 10 differnt plaintext 
  // messages making sure it is smaller than n.
  // If not smaller then n then choose another
  // For eacm message encrypt it using the public key (n,e).
  // After encryption, decrypt the ciphertext using the private
  // key (n,d) and verify it matches the original message.

  // your code here
}
  
