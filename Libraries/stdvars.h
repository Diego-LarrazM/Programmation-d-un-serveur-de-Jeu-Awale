#ifndef BOOL_H
#define BOOL_H

#define false 0
#define true 1

typedef unsigned char Bool;
typedef unsigned int NumCase;
typedef unsigned char BitField_1o; /* Bitfield d'un octet où les bits de 1 à 6 correspondent aux cases 7-12(Joueur1) ou 1-6 (Joueur2 respectivement)
JOUEUR2(case)  :  6  5  4  3  2  1
JOUEUR1(case)  : 12 11 10  9  8  7
BITFIELD(bit)  : b6 b5 b4 b3 b2 b1
*/

#endif /* guard */