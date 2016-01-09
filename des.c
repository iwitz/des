#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ROUNDS 16;

//La permutation initiale et son inverse

int PI[64] = {58,50,42,34,26,18,10,2,
              60,52,44,36,28,20,12,4,
              62,54,46,38,30,22,14,6,
              64,56,48,40,32,24,16,8,
              57,49,41,33,25,17,9,1,
              59,51,43,35,27,19,11,3,
              61,53,45,37,29,21,13,5,
              63,55,47,39,31,23,15,7};

int PI_INV[64] = {  40,8,48,16,56,24,64,32,
                    39,7,47,15,55,23,63,31,
                    38,6,46,14,54,22,62,30,
                    37,5,45,13,53,21,61,29,
                    36,4,44,12,52,20,60,28,
                    35,3,43,11,51,19,59,27,
                    34,2,42,10,50,18,58,26,
                    33,1,41,9,49,17,57,25};

// Les S-Boxes

int SBOX[8][4][16] =
{
    {
        {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7},
        { 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8},
        { 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0},
        {15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13},
    },

    {
        {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10},
        { 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5},
        { 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15},
        {13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9},
    },

    {
        {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
        {13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
        {13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
        { 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12},
    },

    {
        { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
        {13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
        {10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
        { 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14},
    },

    {
        { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9},
        {14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6},
        { 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14},
        {11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3},
    },

    {
        {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11},
        {10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8},
        { 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6},
        { 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13},
    },

    {
        { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1},
        {13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6},
        { 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2},
        { 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12},
    },

    {
        {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7},
        { 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2},
        { 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8},
        { 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11},
    },

};

// La permutation finale avant sortie de f

int P[32]={ 16, 7,20,21,29,12,28,17,
            1 ,15,23,26,5 ,18,31,10,
            2 ,8 ,24,14,32,27, 3, 9,
            19,13,30,6,22,11,4,25};

// choix permut� dans la diversification de la cl�

int PC1[64] = { 57,49,41,33,25,17,9 ,1 ,58,50,42,34,26,18,
                10,2 ,59,51,43,35,27,19,11,3 ,60,52,44,36,
                63,55,47,39,31,23,15,7 ,62,54,46,38,30,22,
                14,6 ,61,53,45,37,29,21,13,5 ,28,20,12,4 };

int PC2[48]={ 14,17,11,24,1 ,5 ,
              3 ,28,15,6 ,21,10,
              23,19,12,4 ,26,8 ,
              16,7 ,27,20,13,2 ,
              41,52,31,37,47,55,
              30,40,51,45,33,48,
              44,49,39,56,34,53,
              46,42,50,36,29,32};

///note : on utilisera des unsigned int pour les nombres sur 32 bits
/// et des unsigned long pour des 64 bits

/*
 *  opérations sur les bits
 */

/*  getBit
 *  renvoie le ième bit en partant de la droite et commençant à 0
 */
  size_t getBit(uint64_t mot, uint64_t position)
  {
   uint64_t temp = mot >> position;
   return temp & 0x1;
  }

/*  setBit
 * modifie le ième bit en partant de la droite et indexé à partir de 0
 */
 void setBit(uint64_t* mot, int position, uint64_t value)
 {
   uint64_t code = *mot;
 	 code ^= (-value ^ code) & (1UL << position);
   *mot = code;
 }
 
 /* inversion
  * inverse les "size" premiers bits avec les "size" suivants
  */
 void inversion(uint64_t long* mot, int size) {
	
	uint64_t mask_a = 0; //masque de gauche
	uint64_t mask_b = 0; //masque de droite
	int i;
	for (i = 0; i < size; i++)
	{
		mask_a = (mask_a << 1) + 1;
		mask_b = (mask_b << 1) + 1;
	}
	mask_a = mask_a << size; //on décale le masque de "size" bits
	
	uint64_t mot_a = (*mot & mask_a) >> size; //on récupère la partie de gauche, et on la place en partie de droite
	uint64_t mot_b = (*mot & mask_b) << size; //on récupère la partie de droite, et on la place en partie de gauche
	
	*mot = mot_a + mot_b; //on concatène les deux parties
}


/*
  * permute les valeurs de mot dans l'ordre de la matrice ordre
  */
 void permute( uint64_t * mot, int * ordre, int taille )
 {
   uint64_t res;
   int i = 0;
   uint64_t bit;
   for (i = 0; i < taille; i++)
   {
      setBit(&res, i, getBit(*mot, ordre[i] - 1));
    }
    printf("\n\n%lx" , *mot);
    printf("\n\n%lx\n" , res);
    *mot = res;
 }

 int main(int argc, char *argv[])
 {
   //uint64_t long cle = 0x0123456789ABCDEF;
   uint64_t bloc = 0x0123456789ABCDEFUL;

   // après permutation initiale bloc doit valoir  0x cc00ccfff0aaf0aa : vrai
   printf(" Permutation intiale :  %lx\n", bloc);
   permute(&bloc, PI, 64);



 	 return 1;
 }
