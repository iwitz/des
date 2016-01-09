#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

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

int PC1[56] = { 57,49,41,33,25,17,9 ,1 ,58,50,42,34,26,18,
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

int EXP[48] = {  32, 1, 2, 3, 4, 5,
                  4, 5, 6, 7, 8, 9,
                  8, 9,10,11,12,13,
                 12,13,14,15,16,17,
                 16,17,18,19,20,21,
                 20,21,22,23,24,25,
                 24,25,26,27,28,29,
                 28,29,30,31,32, 1};


/* notes :
 *  -on utilisera des uint64_t et des uint32_t selon le nombre de bits du bloc
 *  -uint64_t -> décimal : %lu
 *  -utin32_t -> hexa : %lx
 */

/* découpage
 * découpe un bloc de droite à gauche et met les résultats dans un tableau de blocs
 */
 void decoupage(uint64_t * mot, uint64_t * res, int nbParties, int taille)
 {
   int i;
   int taillebloc = taille / nbParties;
   uint64_t masque = 0;
   uint64_t masqueCourant;

   //création du masque sur le nombre de bits nécessaires à la séparation d'un bloc
   for(i = 0; i < taillebloc ; i++)
   {
     masque = (masque << 1 ) +  1;
   }
   //application du masque aux différentes parties du bloc
   for(i = 0; i < nbParties; i++)
   {
     int decalage = i*taillebloc;

     masqueCourant = masque << ( decalage ) ;

     uint64_t blocCourant = ( *mot & masqueCourant ) >> decalage;

     res[i] = blocCourant;
   }

 }
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
 void inversion(uint64_t * mot, int size)
 {
  //découpage en deux parties
  uint64_t * moities = malloc( sizeof(uint64_t) * 2 );
  decoupage(mot, moities, 2, 64);
  //inversion des moitiés
  uint64_t res = ( moities[0] << 32 ) + (moities[1] );
  free(moities);
	*mot = res;
}

/*
 *  Fonction f de la question A
 */
uint64_t fA( uint64_t i )
{
  return 37453123 * i;
}

/*
 * permute les valeurs de mot dans l'ordre de la matrice ordre
 */
void permute( uint64_t * mot, int * ordre, int taille )
{
  uint64_t res;
  int i = 0;
  for (i = 0; i < taille; i++)
  {
     setBit(&res, i, getBit(*mot, ordre[i] - 1));
   }
   *mot = res;
}

*
 * shift cyclique (les "shift_size" premiers bits sont placés au début du mot de taille "total_size")
 */
void circular_shift (uint64_t * mot, int shift_size, int total_size)
{
	//masque de shift_size
	uint64_t mask = 0;
	int i;
	for (i = 0; i < shift_size; i++)
		mask = (mask << 1) + 1;
		
	/* on applique le masque pour récupérer la partie à shift 
	 * on décale vers la gauche
	 * on additionne au mot shifté vers la droite
	 */	
	uint64_t res = ((*mot & mask) << (total_size - shift_size)) + (*mot >> shift_size);
	*mot = res;
}
 
 /* génération des clés 
  * on permute l'entrée avec PC1 pour avoir une clé de 56 bits
  * (pour le premier tour de boucle, on utilise la clé sortant de PC1, sinon la sortie du tour de boucle précédent
  * on inverse la clé sur 28 bits
  * on applique le shift cyclique sur chaque moitié en fonction de l'index de la boucle
  * on concatène et on la stocke pour le prochain tour de bloc
  * on permute la concaténation avec PC2 pour avoir une clé de 48 bits
  * on l'ajoute au tableau des clés générées
  */
  /* clés attendues : 
  * Diversification de la clé 0123456789abcdef
	clé numéro 1 : b02679b49a5
	clé numéro 2 : 69a659256a26
	clé numéro 3 : 45d48ab428d2
	clé numéro 4 : 7289d2a58257
	clé numéro 5 : 3ce80317a6c2
	clé numéro 6 : 23251e3c8545
	clé numéro 7 : 6c04950ae4c6
	clé numéro 8 : 5788386ce581
	clé numéro 9 : c0c9e926b839
	clé numéro 10 : 91e307631d72
	clé numéro 11 : 211f830d893a
	clé numéro 12 : 7130e5455c54
	clé numéro 13 : 91c4d04980fc
	clé numéro 14 : 5443b681dc8d
	clé numéro 15 : b691050a16b5
	clé numéro 16 : ca3d03b87032
  */
 void key_generation( uint64_t * key, uint64_t * keys_tab )
 {
	 uint64_t key_round = *key; //la clé utilisée pour le tour de boucle
	 permute(&key_round, PC1, 56);
	 int i;
	 for(i = 1; i <= 16; i++)
	 { 
		 //printf("entrée : %lx\n", key_round);
		 //inverse la clé sur 28 bits
		uint64_t key_temp = key_round;
		inversion(&key_temp, 56);
		//printf("inversion : %lx\n", key_temp);
		
		//sépare en deux blocs de 28
		uint64_t * split = malloc( sizeof(uint64_t) * 2 ); //tableau pour les deux blocs
		decoupage(&key_temp, split, 2, 56);
		
		// si le tour est à 1, 2, 9 ou 16, le shift est de 1, sinon 2
		int nb_shift;
		if(i == 1 || i == 2 || i == 9 || i == 16)
			nb_shift = 1;
		else
			nb_shift = 2;
			
		// on applique le shift au deux blocs
		uint64_t bloc_a = split[0];
		//printf("bloc a : %lx\n", bloc_a);
		circular_shift(&bloc_a, nb_shift, 28);
		//printf("shift : %lx\n", bloc_a);
		uint64_t bloc_b = split[1];
		//printf("bloc b : %lx\n", bloc_b);
		circular_shift(&bloc_b, nb_shift, 28);
		//printf("shift : %lx\n", bloc_b);
		
		// on concatène les deux blocs
		key_temp = (bloc_a << 28) + bloc_b;
		//printf("concat : %lx\n", key_temp);
		
		// on stocke le résultat pour le prochain tour de boucle
		key_round = key_temp;
		//printf("round : %lx\n", key_round);
		
		// on applique PC2 sur la clé
		permute(&key_temp, PC2, 48);
		//printf("%d, : %lx\n\n", i, key_temp);
		
		// on libère la tableau split
		free(split);
		
		// on l'ajoute au tableau de clés générées
		keys_tab[i-1] = key_temp;
	 }
 }

/*
 * Chiffrement pour la question A
 */
void chiffrementA( uint64_t  * mot )
{
  uint64_t i;
  uint64_t * moities = malloc( sizeof(uint64_t) * 2 );
  for( i=0 ; i < 16 ; i++)
  {
      //découpage en deux parties
      decoupage(mot, moities, 2, 64);

      //on applique le masque, et on shift les 32 bits en première partie
      uint64_t bloc_l = moities[0] << 32;
      //on shift les 32 bits en deuxième partie, puis on effectue un XOR avec le résultat de la fonction
      uint64_t bloc_r = ( moities[1] ) ^ fA(i);
      //on concatène les deux moitiés
      *mot = bloc_l + bloc_r;
  }
  free(moities);
  inversion(mot, 64);
}

/*
 *  déchiffrement pour la question A
 */
void dechiffrementA ( uint64_t  * mot )
{
  inversion(mot, 64);
  int i;
  uint64_t * moities = malloc( sizeof(uint64_t) * 2 );
  for( i=15 ; i >= 0 ; i--)
  {
      //découpage en deux parties
      decoupage(mot, moities, 2, 64);

      //on applique le masque, et on shift les 32 bits en première partie
      uint64_t bloc_l = ( moities[0] ^ fA((uint64_t)i) ) << 32;
      //on shift les 32 bits en deuxième partie, puis on effectue un XOR avec le résultat de la fonction
      uint64_t bloc_r = ( moities[1] ) ;
      //on concatène les deux moitiés
      *mot = bloc_l + bloc_r;
  }
  free(moities);
}
void questionA(uint64_t mot)
{
  printf("\nChiffrement question a : \n");
  printf("Début : %lx \n", mot);
  chiffrementA(&mot);
  printf("Fin : %lx \n", mot);

  printf("\nDéchiffrement question a : \n");
  printf("Début : %lx \n", mot);
  dechiffrementA(&mot);
  printf("Fin : %lx \n", mot);
}

void questionB(uint64_t mot)
{
  printf("\nChiffrement question B : \n");
  printf("Début : %lx \n", mot);

  permute(&mot, PI, 64);
  chiffrementA(&mot);
  printf("Fin : %lx \n", mot);

  printf("\nDéchiffrement question B : \n");
  printf("Début : %lx \n", mot);
  dechiffrementA(&mot);
  permute(&mot, PI_INV, 64);
  printf("Fin : %lx \n", mot);
}

void questioncCExpansion(uint64_t * mot)
{
  permute(mot, EXP, 48 );
}

 int main(int argc, char *argv[])
 {
   //uint64_t long cle = 0x0123456789ABCDEF;
   uint64_t bloc = 0x0123456789ABCDEFUL;

   // après permutation initiale bloc doit valoir  0x cc00ccfff0aaf0aa : vrai
   permute(&bloc, PI, 64);
   printf("\nPermutation intiale :  %lx\n", bloc);

   ///test de l'inversion : elle fonctionne
   inversion(&bloc, 32);
   printf("\nInversion : %lx\n", bloc);

   uint64_t i = 1;
   printf("\nFonction f question A : %lu\n", fA(i) );

   printf("\nTest de découpage en 4 : \n");
   bloc = 0x0123456789ABCDEFUL;
   int k;
   uint64_t * moitie = malloc( sizeof(uint64_t) * 4 );
   decoupage(&bloc, moitie, 4, 64);
   for( k = 0; k < 4; k++)
   {
     printf("   RESULTAT : %d : %lx\n", k, moitie[k]);
   }
   free(moitie);

   bloc = 0x0123456789ABCDEFUL;
   questionA(bloc);

   bloc = 0x0123456789ABCDEFUL;
   questionB(bloc);

   bloc = 0b11101111111111111111111111111111;
   printf("\nExpansion de %lx\n", bloc);
   questioncCExpansion(&bloc);
   printf("%lx\n", bloc);

	bloc = 0x0123456789ABCDEFUL;
	printf("\nGénération des clés de %lx\n", bloc);
	uint64_t * keys= malloc(sizeof(uint64_t) * 16);
	key_generation(&bloc, keys);
	int x;
	for (x = 0; x < 16; x ++)
		printf("clé %d : %lx\n", x, keys[x]);

 	 return 1;
 }
