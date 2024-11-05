#include "awale.h"
#include <stdlib>
#include <errno.h>


Plateau* init(){
    Plateau* p = (Plateau*)malloc(sizeof(Plateau));
    for(int i = 0; i < NB_CASES; p->cases[i++] = 4);
    p->grainesJ1 = p->grainesJ2 = 0;

    srand( time( NULL ) ); // on choisi un joueur au hasard
    p->JoueurCourant = rand() % 2 + 1;
    return p;
}

void end(Plateau* p){
    free(p);
}

bool play(Plateau* p, unsigned short int num_case){

}

void print(Plateau* p, JOUEUR j, char* buffer){
    switch(j){
        case JOUEUR1:
            buffer = "-------------------------\n|"; // cases 7-12 en haut puis 1-6 en bas
            break;
        case JOUEUR2:
            buffer = "-------------------------\n|"; // cases 1-6 en haut puis 7-12 en bas
            break;
        default:
            perror("joueur non défini");
            exit(errno);
    }
}