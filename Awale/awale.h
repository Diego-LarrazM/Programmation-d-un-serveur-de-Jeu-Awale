#ifndef AWALE_H
#define AWALE_H

#define NB_CASES 12
#define false 0
#define true 1

typedef unsigned short int Graines;
typedef unsigned short int Bool;
typedef enum {JOUEUR1 = 1, JOUEUR2 = 2} Joueur;

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
} Plateau;

Plateau* init();
void end(Plateau* p);
Bool play(Plateau* p, unsigned short int num_case);
casesPermises famine(Plateau* p, Joueur j);
void printBoard(Plateau* p, Joueur j, char* buffer);
Bool isWin(Plateau* p, Joueur j);
Bool isDraw(Plateau* p);

#endif /* guard */