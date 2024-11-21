#ifndef AWALE_H
#define AWALE_H


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include "../stdvars.h"


#define NB_CASES 12

typedef unsigned char Graines;
typedef enum {OBSERVATEUR = 0, JOUEUR1 = 1, JOUEUR2 = 2} Joueur;
typedef enum {AHORAIRE = -1, HORAIRE = 1} Sens;

typedef struct plateau{
    Graines cases[NB_CASES]; // plateau avec ranges[i] le nombre de graines dans la case i+1. Le joueur 1 a les cases 7-12 et le joueur 2 1-6
    Graines grainesJ1; // graines du joueur 1
    Graines grainesJ2; // graines du joueur 2
    Joueur joueurCourant;
    Sens sensJeu;
} Plateau;

Plateau* initGame();
void endGame(Plateau* p);

BitField_1o playableFamine(Plateau* p);
BitField_1o playable(Plateau* p);

Bool play(Plateau* p, NumCase num_case);
Bool cantPlay(Plateau* p, NumCase num_case, BitField_1o casesJouables);
void gererDepassementPlateau(NumCase* num_case);
NumCase semerGraines(Plateau* p, NumCase num_case);
BitField_1o trouverCasesConquises(Plateau* p, NumCase num_case);
void recolterConquetes(Plateau* p, BitField_1o casesConquises);
void changePlayer(Plateau* p);

void collectAllPoints(Plateau* p);
Bool isOpponentFamished(Plateau* p);
Bool hasWon(Plateau* p);
Bool isDraw(Plateau* p);

void printBoard(Plateau* p, Joueur joueur, char* buffer);


#endif /* guard */