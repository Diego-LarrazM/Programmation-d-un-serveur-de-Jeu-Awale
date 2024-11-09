#ifndef AWALE_H
#define AWALE_H


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>


#define NB_CASES 12
#define false 0
#define true 1


typedef unsigned char Graines;
typedef unsigned char NumCase;
typedef unsigned char Bool;
typedef unsigned char BitField_1o; /* Bitfield d'un octet où les bits de 1 à 6 correspondent aux cases 1-6(Joueur1) ou 7-12 (Joueur2 respectivement)
JOUEUR1(case)  :  6  5  4  3  2  1
JOUEUR2(case)  : 12 11 10  9  8  7
BITFIELD(bit)  : b6 b5 b4 b3 b2 b1
*/

typedef enum {JOUEUR1 = 1, JOUEUR2 = 2} Joueur;
typedef enum {AHORAIRE = -1, HORAIRE = 1} Sens;


typedef struct plateau{
    Graines cases[NB_CASES]; // plateau avec ranges[i] le nombre de graines dans la case i+1. Le joueur 1 a les cases 7-12 et le joueur 2 1-6
    Graines grainesJ1; // graines du joueur 1
    Graines grainesJ2; // graines du joueur 2
    Joueur JoueurCourant;
    Sens sensJeu;
} Plateau;

Plateau* init();
void end(Plateau* p);

BitField_1o playableFamine(Plateau* p);

Bool play(Plateau* p, NumCase num_case);
Bool cantPlay(Plateau* p, NumCase num_case);
void gererDepassementPlateau(NumCase* num_case);
NumCase semerGraines(Plateau* p, NumCase num_case);
BitField_1o trouverCasesConquises(Plateau* p, NumCase num_case);
void recolterConquetes(Plateau* p, BitField_1o casesConquises);
void changePlayer(Plateau* p);

void collectAllPoints(Plateau* p);
Bool isOpponentFamished(Plateau* p);
Bool hasWon(Plateau* p);
Bool isDraw(Plateau* p);

void printBoard(Plateau* p, char* buffer);


#endif /* guard */