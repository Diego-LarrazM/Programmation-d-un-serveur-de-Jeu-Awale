#include "awale.h"
#include <stdlib>
#include <errno.h>


Plateau* init(){
    Plateau* p = (Plateau*)malloc(sizeof(Plateau));
    for(int i = 0; i < NB_CASES; p->cases[i++] = 4);
    p->grainesJ1 = p->grainesJ2 = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    p->sensJeu = HORAIRE;


    srand( time( NULL ) ); // on choisi un joueur au hasard
    p->JoueurCourant = rand() % 2 + 1;
    return p;
}

void end(Plateau* p){
    free(p);
}

bool play(Plateau* p, unsigned short int num_case){
    if (p->JoueurCourant == JOUEUR1 && (num_case < 6 || num_case > 11))
        return false;
    if (p->JoueurCourant == JOUEUR2 && num_case > 5)
        return false;
    if (p->cases[num_case] == 0)
        return false;
    unsigned short int originalNum_case = num_case;
    int nbSeeds = p->cases[num_case];
    p->cases[num_case] = 0;
    do {
        num_case += p->sensJeu;
        if (num_case == 12)
            num_case = 0;
        else if (num_case > 12)
            num_case = 11;
        if (num_case != originalNum_case) {
            ++p->cases[num_case];
            --nbSeeds;
        }
    }while (nbSeeds);
    return true;
}

void printBoard(Plateau* p, char* buffer){
    switch(p->JoueurCourant){
        case JOUEUR1:
            if (p->sensJeu == HORAIRE)
                sprintf(buffer,
                    "  ╔══╦══╦══╦══╦══╦══╗\n"
                    "> ║%02d║%02d║%02d║%02d║%02d║%02d║ ┐\n"
                    "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
                    "└ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
                    "  ╚══╩══╩══╩══╩══╩══╝\n",
                    p->cases[0], p->cases[1], p->cases[2], p->cases[3], p->cases[4], p->cases[5],
                    p->cases[11], p->cases[10], p->cases[9], p->cases[8], p->cases[7], p->cases[6]
                );
            else
                sprintf(buffer,
                    "  ╔══╦══╦══╦══╦══╦══╗\n"
                    "┌ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
                    "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
                    " >║%02d║%02d║%02d║%02d║%02d║%02d║ ┘\n"
                    "  ╚══╩══╩══╩══╩══╩══╝\n",
                    p->cases[0], p->cases[1], p->cases[2], p->cases[3], p->cases[4], p->cases[5],
                    p->cases[11], p->cases[10], p->cases[9], p->cases[8], p->cases[7], p->cases[6]
                );
            break;
        case JOUEUR2:
            if (p->sensJeu == HORAIRE)
                sprintf(buffer,
                    "  ╔══╦══╦══╦══╦══╦══╗\n"
                    "> ║%02d║%02d║%02d║%02d║%02d║%02d║ ┐\n"
                    "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
                    "└ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
                    "  ╚══╩══╩══╩══╩══╩══╝\n",
                p->cases[6], p->cases[7], p->cases[8], p->cases[9], p->cases[10], p->cases[11],
                p->cases[5], p->cases[4], p->cases[3], p->cases[2], p->cases[1], p->cases[0]
                );
            else
                sprintf(buffer,
                    "  ╔══╦══╦══╦══╦══╦══╗\n"
                    "┌ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
                    "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
                    " >║%02d║%02d║%02d║%02d║%02d║%02d║ ┘\n"
                    "  ╚══╩══╩══╩══╩══╩══╝\n",
                p->cases[6], p->cases[7], p->cases[8], p->cases[9], p->cases[10], p->cases[11],
                p->cases[5], p->cases[4], p->cases[3], p->cases[2], p->cases[1], p->cases[0]
                );
            break;
        default:
            perror("joueur non défini");
            exit(errno);
    }
}

bool hasWon(Plateau* p){
    switch(p->JoueurCourant){
        case JOUEUR1:
            if (p->grainesJ1 > 24)
                return true;
            return false;
        case JOUEUR2:
            if (p->grainesJ2 > 24)
                return true;
            return false;
    }
}

bool isDraw(Plateau* p){
    if (p->grainesJ1 == 24 && p->grainesJ2 == 24)
        return true;
    return false;
}

casesPermises playableFamine(Plateau* p){
    casesPermises casesAutorise = {0, 0, 0, 0, 0, 0};
    int startInd;
    switch(p->JoueurCourant){
        case JOUEUR1:
            if (p->sensJeu == HORAIRE)
                startInd = 6;
            else
                startInd = 11;
            break;
        case JOUEUR2:
            if (p->sensJeu == HORAIRE)
                startInd = 0;
            else
                startInd = 5;
            break;
    }
    if (p->cases[startInd] >= 6) {
        if (p->sensJeu == HORAIRE)
            casesAutorise->case6 = 1;
        else
            casesAutorise->case1 = 1;
    }
    if (p->cases[startInd + p->sensJeu] >= 5) {
        if (p->sensJeu == HORAIRE)
            casesAutorise->case5 = 1;
        else
            casesAutorise->case2 = 1;
    }
    if (p->cases[startInd + 2 * p->sensJeu] >= 4) {
        if (p->sensJeu == HORAIRE)
            casesAutorise->case4 = 1;
        else
            casesAutorise->case3 = 1;
    }
    if (p->cases[startInd + 3 * p->sensJeu] >= 3) {
        if (p->sensJeu == HORAIRE)
            casesAutorise->case3 = 1;
        else
            casesAutorise->case4 = 1;
    }
    if (p->cases[startInd + 4 * p->sensJeu] >= 2) {
        if (p->sensJeu == HORAIRE)
            casesAutorise->case2 = 1;
        else
            casesAutorise->case5 = 1;
    }
    if (p->cases[startInd + 5 * p->sensJeu] >= 1) {
        if (p->sensJeu == HORAIRE)
            casesAutorise->case1 = 1;
        else
            casesAutorise->case6 = 1;
    }
    return casesAutorise;
}

void collectAllPoints(Plateau* p){
    switch(p->JoueurCourant){
        case JOUEUR1:
            for (int i = 6 ; i < 12; ++i){
                p->grainesJ1 += p->cases[i];
                p->cases[i] = 0;
            }
            break;
        case JOUEUR2:
            for (int i = 0 ; i < 6; ++i){
                p->grainesJ2 += p->cases[i];
                p->cases[i] = 0;
            }
            break;
    }
}

Bool isOpponentFamished(Plateau* p){
    switch(p->JoueurCourant){
        case JOUEUR1:
            for (int i = 0 ; i < 6; ++i){
                if (p->cases[i] != 0)
                    return false;
            }
            break;
        case JOUEUR2:
            for (int i = 6 ; i < 12; ++i){
                if (p->cases[i] != 0)
                    return false;
            }
            break;
    }
    return true;
}