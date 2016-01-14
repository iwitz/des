#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

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

int CYCLE[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

/* notes :
 *  -on utilisera des uint64_t et des uint32_t selon le nombre de bits du bloc
 *  -uint64_t -> décimal : %lu
 *  -utin32_t -> hexa : %lx
 */

 /*
  * Partie sur les fichiers
  *
  */

uint64_t* tableauEntree;
int tailleTableauEntree = 0;
// obtenir le num de fichier passé en paramètre
FILE* ouvrirFichier(char * fichier)
{
	return fopen(fichier, "rb+");
}

void lireFichier(FILE* fichier)
{
	char c;

	 // pointeur au début du fichier
	rewind(fichier);

	//parcours du fichier afin de compter le nombre de caractères du fichier
	while( ( c =  fgetc(fichier)) != EOF)
		tailleTableauEntree++;

	if (feof(fichier))
	{
	  // fin du fichier
	  printf("Fichier lu\n");
	}
	else
	{
	  // une erreur est survenue dans la lecture
	  printf("Erreur dans le fichier\n");
	}

	// création dynamique du tableau du résultat
	unsigned char* res = malloc(tailleTableauEntree + 2);

	// placement du pointeur au début
	rewind(fichier);

	// association du tableau aux caractères du fichier
	int i = 0;
	while( ( c =  fgetc(fichier)) != EOF )
	{
		res[i] = c;
		i++;
	}

	// blocs de 8
	int nbBlocs =(tailleTableauEntree/8);
	if (tailleTableauEntree % 2 == 1) { nbBlocs++; }
	tableauEntree = malloc(sizeof(uint64_t) * nbBlocs);

	int j;
	int index;
	// parcours des blocs pour les concaténer
	for(i = 0; i < nbBlocs; i++)
	{
		for(j = 0; j < 8; j++)
		{
			index = j + (i * 8);
			tableauEntree[i] = (tableauEntree[i] << 8) + res[index];
		}
	}

	fclose(fichier);
}

void ecrireFichier (uint64_t* mots, int nbMots)
{
	int sortie = open("sortie.txt", O_WRONLY|O_CREAT,0640);

		int i;
		for (i = 0; i < nbMots; i++)
			write(sortie, &mots[i], sizeof(uint64_t));
		close(sortie);
}

/*
 * DES
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
   decoupage(mot, moities, 2, size);
   //inversion des moitiés
   uint64_t res = ( moities[0] << (size/2) ) + (moities[1] );
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

 uint64_t fD( uint64_t i )
 {
   return 0x123456789ABCDEFLU;
 }

 /*
  * permute les valeurs de mot dans l'ordre de la matrice ordre
  */
 void permute( uint64_t * mot, int * ordre, int taille_matrice, int taille_mot)
 {
 	uint64_t res = 0;
 	uint64_t bit = 0;
 	int position;
 	int i;
 	for (i = 0; i < taille_matrice; i++)
 	{
 		position = taille_mot - ordre[i];
 		bit = ((*mot >> position) & 0x01) << (taille_matrice - 1 - i);
 		res = res ^ bit;
 	}
 	*mot = res;
 }

 /*
  * shift cyclique (les "decalage" premiers bits sont placés au début du mot de taille "tailleMot")
  */
 void decalageCirculaire( uint64_t * mot, int decalage, int tailleMot )
 {
 	//masque de decalage
 	uint64_t masque = 0;
 	int i;
 	for (i = 0; i < decalage; i++)
 		masque = (masque << 1) + 1;

 	/* on applique le masque pour récupérer la partie à shift
 	 * on décale vers la gauche
 	 * on additionne au mot shifté vers la droite
 	 */
 	 uint64_t temp;
 	 uint64_t res = *mot;
 	 for (i = 0; i < decalage; i++)
 	 {
 		  temp = getBit(res, tailleMot - 1);
 		  res = (res << (1)) + temp;
 		  setBit(&res, tailleMot, 0);
 	 }
 	*mot = res;
 }

  /* génération des clés
   * on permute l'entrée avec PC1 pour avoir une clé de 56 bits
   * (pour le premier tour de boucle, on utilise la clé sortant de PC1, sinon la sortie du tour de boucle précédent
   * on applique le shift cyclique sur chaque moitié en fonction de l'index de la boucle
   * on concatène et on la stocke pour le prochain tour de bloc
   * on permute la concaténation avec PC2 pour avoir une clé de 48 bits
   * on l'ajoute au tableau des clés générées
   */
 void generation_cles( uint64_t key, uint64_t * keys_tab )
  {
 	 uint64_t cle_courante = key; //la clé utilisée pour le tour de boucle
 	 permute(&cle_courante, PC1, 56, 64);

 	 int i;

 	 for(i = 1; i <= 16; i++)
 	 {
 		uint64_t temp = cle_courante;

 		//sépare en deux blocs de 28
 		uint64_t * moities = malloc( sizeof(uint64_t) * 2 ); //tableau pour les deux blocs
 		decoupage(&temp, moities, 2, 56);

 		// si le tour est à 1, 2, 9 ou 16, le shift est de 1, sinon 2
 		int decalage = CYCLE[i - 1];

 		// on applique le shift au deux blocs
 		uint64_t bloc_a = moities[1];
 		decalageCirculaire(&bloc_a, decalage, 28);
 		uint64_t bloc_b = moities[0];
 		decalageCirculaire(&bloc_b, decalage, 28);

 		// on concatène les deux blocs
 		temp = (bloc_a << 28) + bloc_b;

 		// on stocke le résultat pour le prochain tour de boucle
 		cle_courante = temp;

 		// on applique PC2 sur la clé
 		permute(&temp, PC2, 48, 56);

 		// on libère la tableau moities
 		free(moities);

 		// on l'ajoute au tableau de clés générées
 		keys_tab[i-1] = temp;
 	 }
  }

  /*
   * substitution : mot de 48 bits substitué en code de 32 bits à l'aide des SBoxes
   * on découpe le mot de 48 en 8 sous-blocs de longueur 6
   * on créé un code à partir de la matrice[i]
   * l'index de la ligne sera "b1b6"
   * l'index de la colonne sera "b2b3b4b5"
   * on lit le résultat dans la SBox correspondante
   */
   void substitution( uint64_t * mot )
   {
  	 uint64_t * blocs = malloc(sizeof(uint64_t) * 8);
  	 decoupage(mot, blocs, 8, 48);

  	 uint64_t res = 0;
  	 int i;
    uint64_t courant;
  	 for (i = 0; i < 8; i++)
  	 {
  		 courant = blocs[7 - i];
  		 uint64_t ligne = 2*getBit(courant, 5) + getBit(courant, 0);
  		 uint64_t colonne = 8*getBit(courant, 4) + 4*getBit(courant, 3) + 2*getBit(courant, 2) + getBit(courant, 1);
  		 res = (res << 4) + SBOX[i][ligne][colonne];
  	 }
  	 free(blocs);
  	 *mot = res;
   }

   /* expansion
    * transforme un bloc de 32 bits en bloc de 48 bits
    */
   void expansion(uint64_t * mot)
   {
     permute(mot, EXP, 48, 32);
   }



   /** des complet
   *  avec c la direction de chiffrement ( c pour chiffrer, d pour déchiffrer)
   */
   void des( uint64_t * cles, uint64_t * mot, char direction)
   {
     ///permutation initiale
     permute(mot, PI, 64, 64);

     ///découpage en 2
     uint64_t * moities = malloc( sizeof(uint64_t) * 2 );
     decoupage(mot, moities, 2, 64);
     uint64_t droite =  moities[0];
     uint64_t gauche = moities[1];
     free(moities);

     ///rondes
     int i, k;
     uint64_t temp, cle;

     for( i = 0; i < 16; i++ )
     {
       ///stockage de la valeur de dI
       temp = droite ;

       ///expansion
       expansion(&droite);

       ///mélange
       if(direction=='c')
       {///si on chiffre
         cle = cles[ i ];
       }
       else
       {///si on déchiffre
         cle = cles[ 15 - i ];
       }
       droite = droite ^ cle;

       ///la clé donnée est sur plus de 48 bits : on doit donc couper le résultat du mélange pour qu'il soit sur 48 bits
       for(k=48; k < 64; k++)
       {
          setBit(&droite, k, 0);
       }

       ///substitution : ok
       substitution(&droite);

       ///permutation P
       permute(&droite, P, 32, 32);

       //mélange des blocs : OK
       droite = droite ^ gauche;

       ///inversion pour la prochaine itération
       gauche = temp;/// gI+1 = dI

       ///recollage des blocs
       *mot = ( gauche << 32 ) ^ droite;
     }
     //échange
     moities = malloc( sizeof(uint64_t) * 2 );
     decoupage(mot, moities, 2, 64);
     droite =  moities[0];
     gauche = moities[1];
     *mot = ( droite << 32 ) ^ gauche;
     free(moities);
     ///permutation finale
     permute(mot, PI_INV, 64, 64);
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
 /* question a :
  *  fonction de chiffrement/déchiffrement d'un bloc de 64 bits avec 16 tours sans permutation
  *  initiale et finaleavec f(i) = 37...23 *i
  */
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

 /*  question b:
  *  fonction de chiffrement et déchiffrement comme A mais aec les permutations
  *  initiale et finale
  */
 void questionB(uint64_t mot)
 {
   printf("\nChiffrement question b : \n");
   printf("Paramètre : %lx \n", mot);

   permute(&mot, PI, 64, 64);
   chiffrementA(&mot);
   permute(&mot, PI_INV, 64, 64);
   printf("Fin : %lx \n", mot);

   printf("\nDéchiffrement question b : \n");
   printf("Paramètre : %lx \n", mot);
   permute(&mot, PI, 64, 64);
   dechiffrementA(&mot);
   permute(&mot, PI_INV, 64, 64);
   printf("Fin : %lx \n", mot);
 }

 /* expansion :
  *  permute un bloc de 32 bits pour en faire un bloc de 48 bits
  */
 void questionC(uint64_t * mot)
 {
   printf("\nQuestion c\nParamètre : %lx\n", *mot);
   expansion(mot);
   printf("Résultat : %lx\n", *mot);
 }

 /* question D
  *  fonction de substitution appliquant les 8 fonctions de sélection à un bloc de longueur 48 découpé en 8 sous blocs de longueur 6
  *  et renvoyant un bloc de longueur 32
  */
 void questionD(uint64_t * mot)
 {
   printf("\nQuestion d\nParamètre : %lx\n", *mot);
   substitution(mot);
   printf("Résultat : %lx\n", *mot);
 }

/*  question e
 *  fonction f appliquant expansion, mélange avec clé constante, k = 0x123456789abcdef
 *  la substitution et la permutation finale
 */
void questionE(uint64_t * mot)
{
  ///génération du tableau contenant la clé
  uint64_t * cles = malloc( sizeof(uint64_t) * 16 );
  int i;
  for( i = 0; i < 16; i++)
  {///remplissage du tableau de clés avec la clé fixe
    cles[i] = 0x123456789abcdefUL;
  }
  printf("\nQuestion e \n");
  printf("Paramètre : %lx\n", *mot);
  des( cles, mot, 'c' );
  printf("Bloc chiffré : %lx\n", *mot);
  des( cles, mot, 'd' );
  printf("Bloc déchiffré : %lx\n", *mot);
  free(cles);
}

  /** question f:
   *  fonction de génération des 16 clésà partir de la clé unique K
   */
  void questionF(uint64_t cle)
  {
    printf("\nQuestion f \n");
    uint64_t * cles = malloc(sizeof(uint64_t) * 16);
    generation_cles( cle, cles );

    int k;
    for (k = 0; k < 16; k ++)
    {
       printf("clé %d : %lx\n", k, cles[k]);
    }

    free(cles);
  }

  /** question g :
   *  fonction de chiffrement et de déchiffrement complète appliquant des sur un bloc
   */
  void questionG( uint64_t cle, uint64_t * mot)
  {
    printf("\nQuestion g \n");
    ///diversification de la clé
    uint64_t * cles = malloc(sizeof(uint64_t) * 16);
    generation_cles(cle, cles);

    printf("Paramètre : %lx\n", *mot);
    des(cles, mot, 'c');
    printf("Chiffrement : %lx\n", *mot);
    des(cles, mot, 'd');
    printf("Déchiffrement : %lx\n", *mot);

    free(cles);
  }

/*
 * fonction qui va inverser chaque mot de 64 bits d'un tableau de nbMots de la manière suivante :
 * (b0b1...b30b31)(b32b33...b62b63) = (b30b31...b0b1)(b62b63...b32b33)
 */
void inversionMot(uint64_t* mots, int nbMots)
{
	uint64_t* temp = malloc(sizeof(uint64_t) * 2);

	uint64_t mask = 0xFF;
	uint64_t buff_a = 0;
	uint64_t bloc_a = 0;
	uint64_t buff_b = 0;
	uint64_t bloc_b = 0;
	uint64_t res = 0;
	int i;
	int j;

	for(j = 0; j < nbMots; j++)
	{
		decoupage(&mots[j], temp, 2, 64);
		bloc_a = 0;
		bloc_b = 0;
		for(i = 0; i < 32; i = i + 8)
		{

			buff_a = (temp[0] & (mask << i)) >> i;
			bloc_a += (buff_a << (24 - i));

			buff_b = (temp[1] & (mask << i)) >> i;
			bloc_b += (buff_b << (24-i));

			res = (bloc_a << 32) + bloc_b;
		}
		mots[j] = res;
	}
	free(temp);
}


 int main(int argc, char * argv[])
 {
   char direction = 'z';
   int c;
   ///lecture des options
   while ( (c = getopt(argc, argv, "cd"))  != -1)
   {
     if( c == 'c' )
     {
       direction = 'c';
     }
     if( c == 'd' )
     {
       direction = 'd';
     }
   }
   printf("\n%c\n", direction);

   if( direction == 'z')
   {///lancer la démo
     uint64_t bloc = 0x0123456789ABCDEFUL;
     uint64_t cle = 0x0123456789ABCDEFUL;

     ///Question a : chiffrement sans Pi et PI_INV
     bloc = 0x0123456789ABCDEFUL;
     questionA(bloc);

     ///Question b : chiffrement et déchiffrement avec PI et PI_INV
     bloc = 0x0123456789ABCDEFUL;
     questionB(bloc);

     ///Question c : expansion
     bloc = 0xefffffff;
     questionC(&bloc);
     // valeur attendue : 0xf5ffffffffff

     ///Question d : substitution
     bloc = 0b001100110011010101101010000111111000000000111111;
     questionD(&bloc);
     // valeur attendue : 0xB65BC14B

     ///Question e : chiffrement et déchiffrement avec fonction f complète
     bloc = 0x0123456789ABCDEFUL;
     questionE(&bloc);

     ///Question f : génération des clés
     bloc = 0x0123456789ABCDEFUL;
     questionF(cle);

     ///Question G : chiffrement et déchiffrement complet
     bloc = 0x0123456789ABCDEFUL;
     questionG(cle, &bloc);
   }
   else
   {///chiffrer ou déchiffrer le document demandé
     if(argc != 3)
     {///si le nombre d'arguments est incorrect
       printf("%d Avec -c ou -d il faut entrer le nom du fichier dans lequel lire ou écrire.", argc);
       exit(1);
     }
     else
     {
       FILE* fichier;

       if ((fichier = ouvrirFichier(argv[2])) < 0) { exit(-2); }

       /// lecture du fichier
       lireFichier(fichier);

       /// génération des clés
       uint64_t cle = 0x0123456789ABCDEFUL;
       uint64_t * cles = malloc(sizeof(uint64_t) * 16);
       generation_cles(cle, cles);

       int nbBlocs = tailleTableauEntree/8 + 1;
       uint64_t * resultat = malloc(sizeof(uint64_t) * (nbBlocs));
       int i;
       uint64_t mot;

       if(direction == 'd')
      {///déchiffrement

for(i = 0; i < tailleTableauEntree; i++)
	printf("%lx\n", tableauEntree[i]);
	
        // concaténation de deux blocs deux à deux, dans le tableau resultat
        for(i = 0; i < nbBlocs; i++)
        {
          mot = tableauEntree[i];
          des(cles, &mot, 'd'); // DES
          resultat[i] = mot;
        }
		

        // inversion du tableau
  
        inversionMot(resultat, nbBlocs);

		for(i = 0; i < nbBlocs; i++)
		{	printf("%lx\n", resultat[i]); }
        /// écriture dans le fichier
        ecrireFichier(resultat, nbBlocs);


      }
      else
      { //chiffrement
        // concaténation de deux blocs deux à deux, dans le tableau resultat
        for(i = 0; i < nbBlocs; i++)
        {
          mot = tableauEntree[i];
          des(cles, &mot, 'c');
          resultat[i] = mot;
        }

        for(i = 0; i < nbBlocs; i++)
        {  printf("%lx\n", resultat[i]); }

        // inversion du tableau
        inversionMot(resultat, nbBlocs);

        /// écriture dans le fichier
        ecrireFichier(resultat, nbBlocs);
      }
      free(tableauEntree);
      free(cles);
	   }
   }
   return 1;
 }
