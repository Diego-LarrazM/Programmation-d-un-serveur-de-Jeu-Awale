#ifndef AWALE_H
#define AWALE_H

#define JOUEUR1 1 
#define JOUEUR2 2
#define NB_CASES 12
#define false 0
#define true 1

typedef unsigned short int JOUEUR
typedef unsigned short int GRAINES
typedef unsigned char bool


typedef struct plateau{
    GRAINES cases[NB_CASES]; // plateau avec ranges[i] le nombre de graines dans la case i+1. Le joueur 1 a les cases 1-6 et le joueur 2 7-12
    GRAINES grainesJ1; // graines du joueur 1
    GRAINES grainesJ2; // graines du joueur 2
    JOUEUR JoueurCourant;
} Plateau;

Plateau* init();
void end(Plateau* p);
bool play(Plateau* p, unsigned short int num_case);
void print(Plateau* p, JOUEUR j);

#endif /* guard */