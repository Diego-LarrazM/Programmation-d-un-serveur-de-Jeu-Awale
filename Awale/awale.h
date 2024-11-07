#ifndef AWALE_H
#define AWALE_H

#define NB_CASES 12
#define false 0
#define true 1

typedef unsigned short int Graines;
typedef unsigned short int Bool;
typedef enum {JOUEUR1 = 1, JOUEUR2 = 2} Joueur;
typedef enum {AHORAIRE = -1, HORAIRE = 1} Sens;

typedef struct bitField{
    Bool case1 : 1; // store 1 bit
    Bool case2 : 1;
    Bool case3 : 1;
    Bool case4 : 1;
    Bool case5 : 1;
    Bool case6 : 1;
} casesPermises;

typedef struct plateau{
    Graines cases[NB_CASES]; // plateau avec ranges[i] le nombre de graines dans la case i+1. Le joueur 1 a les cases 1-6 et le joueur 2 7-12
    Graines grainesJ1; // graines du joueur 1
    Graines grainesJ2; // graines du joueur 2
    Joueur JoueurCourant;
    Sens sensJeu;
} Plateau;

Plateau* init();
void end(Plateau* p);
Bool play(Plateau* p, unsigned short int num_case);
casesPermises playableFamine(Plateau* p);
void printBoard(Plateau* p, char* buffer);
Bool hasWon(Plateau* p);
Bool isDraw(Plateau* p);
void collectAllPoints(Plateau* p);
Bool isOpponentFamished(Plateau* p);

#endif /* guard */