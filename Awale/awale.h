#ifndef AWALE_H
#define AWALE_H

#define NB_CASES 12
#define false 0
#define true 1

typedef unsigned char Graines;
typedef unsigned char Bool;
typedef unsigned short int BitField; //Bitfield pour indiquer si une case est validée (1) ou pas (0)
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
BitField play(Plateau* p, unsigned char num_case);
Bool canPlay(Plateau* p, unsigned char num_case);
void gererDepassement(unsigned char* num_case);
BitField playableFamine(Plateau* p);
void printBoard(Plateau* p, char* buffer);
Bool hasWon(Plateau* p);
Bool isDraw(Plateau* p);
void collectAllPoints(Plateau* p);
Bool isOpponentFamished(Plateau* p);

#endif /* guard */