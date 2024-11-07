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

void printBoard(Plateau* p, JOUEUR j, char* buffer){
    switch(j){
        case JOUEUR2:
            sprintf(buffer,
                "╔══╦══╦══╦══╦══╦══╗\n"
                "║%02d║%02d║%02d║%02d║%02d║%02d║\n"
                "╠══╬══╬══╬══╬══╬══╣\n"
                "║%02d║%02d║%02d║%02d║%02d║%02d║\n"
                "╚══╩══╩══╩══╩══╩══╝\n",
                p->cases[0], p->cases[1], p->cases[2], p->cases[3], p->cases[4], p->cases[5],
                p->cases[11], p->cases[10], p->cases[9], p->cases[8], p->cases[7], p->cases[6]
            );
            break;
        case JOUEUR1:
            sprintf(buffer,
                "╔══╦══╦══╦══╦══╦══╗\n"
                "║%02d║%02d║%02d║%02d║%02d║%02d║\n"
                "╠══╬══╬══╬══╬══╬══╣\n"
                "║%02d║%02d║%02d║%02d║%02d║%02d║\n"
                "╚══╩══╩══╩══╩══╩══╝\n",
                p->cases[6], p->cases[7], p->cases[8], p->cases[9], p->cases[10], p->cases[11],
                p->cases[5], p->cases[4], p->cases[3], p->cases[2], p->cases[1], p->cases[0]
            );
            break;
        default:
            perror("joueur non défini");
            exit(errno);
    }
}

bool isWin(Plateau* p, JOUEUR j){
    switch(j){
        case JOUEUR1:
            if (p->grainesJ1 > 24)
                return true;
            return false;
        case JOUEUR2:
            if (p->grainesJ2 > 24)
                return true;
            return false;
        default:
            perror("joueur non défini");
            exit(errno);
    }
}

bool isDraw(Plateau* p){
    if (p->grainesJ1 == 24 && p->grainesJ2 == 24)
        return true;
    return false;
}