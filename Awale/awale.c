#include "awale.h"
#include <stdlib>
#include <errno.h>


Plateau* init(){
    Plateau* p = (Plateau*)malloc(sizeof(Plateau));
    for(int i = 0; i < NB_CASES; p->cases[i++] = 4);
    p->grainesJ1 = p->grainesJ2 = 0;


//SENS HORAIRE A DEFINIR par joueurs////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    p->sensJeu = HORAIRE;


    srand( time( NULL ) ); // on choisi un joueur au hasard
    p->JoueurCourant = rand() % 2 + 1;
    return p;
}

void end(Plateau* p){
    free(p);
}

void changePlayer(Plateau* p){
    switch(p->JoueurCourant){
        case JOUEUR1:
            p->JoueurCourant = JOUEUR2;
            break;
        case JOUEUR2:
            p->JoueurCourant = JOUEUR1;
            break;
    }
}

Bool canPlay(Plateau* p, unsigned char num_case){
	  if (p->JoueurCourant == JOUEUR1 && (num_case < 6 || num_case > 11))
        return false;
    if (p->JoueurCourant == JOUEUR2 && num_case > 5)
        return false;
    if (p->cases[num_case] == 0)
        return false;
    return true;
}

void gererDepassement(unsigned char* num_case){
    if (num_case == 12) *num_case = 0; 
    else if (num_case > 12) *num_case = 11; // unsigned char => (0 - 1) == (255)
}

BitField play(Plateau* p, unsigned char num_case){
    unsigned char originalNum_case = num_case;
  	
    int nbSeeds = p->cases[num_case];
    p->cases[num_case] = 0;
    do {
        num_case += p->sensJeu;
        gererDepassement(&num_case);
        if (num_case != originalNum_case) {
            ++p->cases[num_case]
            --nbSeeds;
        }
    }while (nbSeeds);
    return gererConquetes(p,num_case);
}

            
BitField gererConquetes(Plateau* p, unsigned char num_case){
    BitField caseConquise = 0;
    while (p->cases[num_case] == 2 || p->cases[num_case] == 3 && ((p->JoueurCourant == JOUEUR1 && num_case > 5) || (p->JoueurCourant == JOUEUR2 && num_case < 6))){
        caseConquise |= (1 << num_case);

        if (p->JoueurCourant == JOUEUR1)  
            p->grainesJ1 += p->cases[num_case];
        else //JOUEUR2
            p->grainesJ2 += p->cases[num_case]; 
        p->cases[num_case] = 0;
        
        num_case -= p->sensJeu;
        gererDepassement(&num_case);
    }
    return caseConquise;
}


void printBoard(Plateau* p, char* buffer){
<<<<<<< HEAD
    switch((int)p->JoueurCourant * (int)p->sensJeu){
        case (int)JOUEUR1* (int)HORAIRE:
            sprintf(buffer,
                "  ╔══╦══╦══╦══╦══╦══╗\n"
                "> ║%02d║%02d║%02d║%02d║%02d║%02d║ ┐\n"
                "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
                "└ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
                "  ╚══╩══╩══╩══╩══╩══╝\n",
                p->cases[0], p->cases[1], p->cases[2], p->cases[3], p->cases[4], p->cases[5],
                p->cases[11], p->cases[10], p->cases[9], p->cases[8], p->cases[7], p->cases[6]
            );
            break;
        case (int)JOUEUR1* (int)AHORAIRE:
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
        case (int)JOUEUR2 * (int)HORAIRE:
            sprintf(buffer,
                "  ╔══╦══╦══╦══╦══╦══╗\n"
                "> ║%02d║%02d║%02d║%02d║%02d║%02d║ ┐\n"
                "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
                "└ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
                "  ╚══╩══╩══╩══╩══╩══╝\n",
            p->cases[6], p->cases[7], p->cases[8], p->cases[9], p->cases[10], p->cases[11],
            p->cases[5], p->cases[4], p->cases[3], p->cases[2], p->cases[1], p->cases[0]
            );
            break;
        case (int)JOUEUR2 * (int)AHORAIRE:
            sprintf(buffer,
                "  ╔══╦══╦══╦══╦══╦══╗\n"
                "┌ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
                "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
                " >║%02d║%02d║%02d║%02d║%02d║%02d║ ┘\n"
                "  ╚══╩══╩══╩══╩══╩══╝\n",
            p->cases[6], p->cases[7], p->cases[8], p->cases[9], p->cases[10], p->cases[11],
            p->cases[5], p->cases[4], p->cases[3], p->cases[2], p->cases[1], p->cases[0]
            );
=======
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
                    "> ║%02d║%02d║%02d║%02d║%02d║%02d║ ┘\n"
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
                    "> ║%02d║%02d║%02d║%02d║%02d║%02d║ ┘\n"
                    "  ╚══╩══╩══╩══╩══╩══╝\n",
                p->cases[6], p->cases[7], p->cases[8], p->cases[9], p->cases[10], p->cases[11],
                p->cases[5], p->cases[4], p->cases[3], p->cases[2], p->cases[1], p->cases[0]
                );
>>>>>>> bb03a841eaa01ed463b134b063a0d332b4d03a99
            break;
        default:
            perror("joueur non défini");
            exit(errno);
    }
}

Bool hasWon(Plateau* p){
    if(p->JoueurCourant == JOUEUR1  && p->grainesJ1 > 24) return true;
    if(p->JoueurCourant == JOUEUR2  && p->grainesJ2 > 24) return true;
    return false;
}

Bool isDraw(Plateau* p){
    if (p->grainesJ1 == 24 && p->grainesJ2 == 24)
        return true;
    return false;
}

<<<<<<< HEAD
BitField playableFamine(Plateau* p){
    BitField casesAutorise = 0; // les bits de 1 à 6 indiquent si les cases 1-6(Joueur1) ou 7-12 (Joueur2) sont jouables (égal à 1).
    int ajout = 1;
    int offset = p->JoueurCourant == JOUEUR2 ? 6 : 0;;

    for(int i = offset; i < 6 + offset; i++) {
        /*
        Cas Horaire  : p->cases[i] >= 6 - (i - offset)
        Cas Ahoraire : p->cases[i] >= 1 + (i - offset)
        */
        if(p->cases[i] >= (((int)(p->sensJeu) == HORAIRE ? 6:1) - (int)(p->sensJeu) * (i - offset))){
            casesAutorise |= ajout; 
            ajout <<= 1;
        }
=======
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
            casesAutorise.case6 = 1;
        else
            casesAutorise.case1 = 1;
    }
    if (p->cases[startInd + p->sensJeu] >= 5) {
        if (p->sensJeu == HORAIRE)
            casesAutorise.case5 = 1;
        else
            casesAutorise.case2 = 1;
    }
    if (p->cases[startInd + 2 * p->sensJeu] >= 4) {
        if (p->sensJeu == HORAIRE)
            casesAutorise.case4 = 1;
        else
            casesAutorise.case3 = 1;
    }
    if (p->cases[startInd + 3 * p->sensJeu] >= 3) {
        if (p->sensJeu == HORAIRE)
            casesAutorise.case3 = 1;
        else
            casesAutorise.case4 = 1;
    }
    if (p->cases[startInd + 4 * p->sensJeu] >= 2) {
        if (p->sensJeu == HORAIRE)
            casesAutorise.case2 = 1;
        else
            casesAutorise.case5 = 1;
    }
    if (p->cases[startInd + 5 * p->sensJeu] >= 1) {
        if (p->sensJeu == HORAIRE)
            casesAutorise.case1 = 1;
        else
            casesAutorise.case6 = 1;
>>>>>>> bb03a841eaa01ed463b134b063a0d332b4d03a99
    }
    return casesAutorise;
}

void collectAllPoints(Plateau* p){
    if(p->JoueurCourant == JOUEUR1)
        for (int i = 6 ; i < 12; ++i){
            p->grainesJ1 += p->cases[i];
            p->cases[i] = 0;
        }
    else{ //JOUEUR2
        for (int i = 0 ; i < 6; ++i){
            p->grainesJ2 += p->cases[i];
            p->cases[i] = 0;
        }
    }
}

Bool isOpponentFamished(Plateau* p){
    int i;
    int offset = p->JoueurCourant == JOUEUR2 ? 6 : 0;
    for (i = offset ; i < 6 + offset; ++i)
        if(p->cases[i] != 0) return false;
    return true;
}