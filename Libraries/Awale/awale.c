#include "awale.h"



Plateau* initGame(){
    Plateau* p = (Plateau*)malloc(sizeof(Plateau));
    for(int i = 0; i < NB_CASES; ++i) p->cases[i] = 4;
    //p->cases[0] = 1; p->cases[2] = 10; p->cases[3] = 2; p->cases[5] = 1;

    p->grainesJ1 = p->grainesJ2 = 0;


    //SENS HORAIRE A DEFINIR par joueurs////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    p->sensJeu = HORAIRE;


    srand( time( NULL ) ); // on choisi un joueur au hasard
    p->joueurCourant = rand() % 2 + 1;
    return p;
}


void endGame(Plateau* p){
    free(p);
}


void changePlayer(Plateau* p){
    switch(p->joueurCourant){
    case JOUEUR1:
        p->joueurCourant = JOUEUR2;
        break;
    case JOUEUR2:
        p->joueurCourant = JOUEUR1;
        break;
    }
}


Bool play(Plateau* p, NumCase num_case){
    BitField_1o casesJouables = isOpponentFamished(p) ? playableFamine(p) : 63;
    if(cantPlay(p, num_case, casesJouables)) return false;
    NumCase caseArret = semerGraines(p, num_case);
    BitField_1o casesConquises = trouverCasesConquises(p, caseArret);
    if (casesConquises != 63) // casesConquises == 11111111 soit toutes les cases énemies sont conquises
        recolterConquetes(p, casesConquises);
    return true;
}


Bool cantPlay(Plateau* p, NumCase num_case, BitField_1o casesJouables){
	if (p->joueurCourant == JOUEUR1 && (num_case < 6 || num_case > 11))
        return true;
    if (p->joueurCourant == JOUEUR2 && num_case > 5)
        return true;
    if (p->cases[num_case] == 0)
        return true;
    NumCase offset = p->joueurCourant == JOUEUR2 ? 0 : 6;
    if (casesJouables & (1 << (num_case - offset)))
        return false;
    return true;
}


void gererDepassementPlateau(NumCase* num_case){
    if (*num_case == 12) *num_case = 0; 
    else if (*num_case > 12) *num_case = 11; // unsigned int => (0 - 1) == (MAX_UINT)
}


NumCase semerGraines(Plateau* p, NumCase num_case){
    NumCase originalNum_case = num_case;
  	
    Graines nbSeeds = p->cases[num_case];
    p->cases[num_case] = 0;
    do {
        num_case += p->sensJeu;
        gererDepassementPlateau(&num_case);
        if (num_case != originalNum_case) {
            ++p->cases[num_case];
            --nbSeeds;
        }
    } while (nbSeeds);
    return num_case;
}


BitField_1o trouverCasesConquises(Plateau* p, NumCase num_case){
    BitField_1o caseConquise = 0;
    NumCase offset = p->joueurCourant == JOUEUR2 ? 6 : 0;
    // Tant que dans le camp enemi et qu'il y a 2 ou 3 graines on a conquis
    while ((p->cases[num_case] == 2 || p->cases[num_case] == 3) && ((p->joueurCourant == JOUEUR1 && num_case < 6) || (p->joueurCourant == JOUEUR2 && num_case > 5))){
        caseConquise |= (1 << (num_case - offset));
        num_case -= p->sensJeu;
        gererDepassementPlateau(&num_case);
    }
    return caseConquise;
}  


void recolterConquetes(Plateau* p, BitField_1o casesConquises){
    NumCase offset = p->joueurCourant == JOUEUR2 ? 6 : 0;
    for(NumCase i = 0; i < 6 && casesConquises; ++i, casesConquises >>= 1){
	if (casesConquises & 1) {
            if (p->joueurCourant == JOUEUR1)  
                 p->grainesJ1 += p->cases[i + offset];
            else //JOUEUR2
                 p->grainesJ2 += p->cases[i + offset]; 
            p->cases[i + offset] = 0;
	}
    }
}


Bool hasWon(Plateau* p){
    if(p->joueurCourant == JOUEUR1  && p->grainesJ1 > 24) return true;
    if(p->joueurCourant == JOUEUR2  && p->grainesJ2 > 24) return true;
    return false;
}


Bool isDraw(Plateau* p){
    if (p->grainesJ1 == 24 && p->grainesJ2 == 24)
        return true;
    return false;
}


Bool isOpponentFamished(Plateau* p){
    int i;
    NumCase offset = p->joueurCourant == JOUEUR2 ? 6 : 0;
    for (i = offset ; i < 6 + offset; ++i)
        if(p->cases[i] != 0) return false;
    return true;
}


BitField_1o playableFamine(Plateau* p){
    BitField_1o casesAutorise = 0; // les bits de 1 à 6 indiquent si les cases 1-6(Joueur1) ou 7-12 (Joueur2) sont jouables (égal à 1).
    int ajout = 1;
    NumCase offset = p->joueurCourant == JOUEUR2 ? 0 : 6;

    for(int i = offset; i < 6 + offset; i++) {
        /*
        Cas Horaire  : p->cases[i] >= 6 - (i - offset)
        Cas Ahoraire : p->cases[i] >= 1 + (i - offset)
        */
        if(p->cases[i] >= (((int)(p->sensJeu) == HORAIRE ? 6:1) - (int)(p->sensJeu) * (i - offset)))
            casesAutorise |= ajout; 
        ajout <<= 1;
    }
    return casesAutorise;
}

BitField_1o playable(Plateau* p){
    BitField_1o casesAutorise = 0; // les bits de 1 à 6 indiquent si les cases 1-6(Joueur1) ou 7-12 (Joueur2) sont jouables (égal à 1).
    int ajout = 1;
    NumCase offset = p->joueurCourant == JOUEUR2 ? 0 : 6;

    for(int i = offset; i < 6 + offset; i++) {
        if(p->cases[i] > 0)  casesAutorise |= ajout; // activation
        ajout <<= 1;
    }
    return casesAutorise;
}


void collectAllPoints(Plateau* p){
    if(p->joueurCourant == JOUEUR1)
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


void printBoard(Plateau* p, Joueur joueur, char* buffer){
    switch((int)joueur * (int)p->sensJeu){
    case (int)JOUEUR1* (int)HORAIRE:
        sprintf(buffer,
            "  ╔01╦02╦03╦04╦05╦06╗\n"
            "> ║%02d║%02d║%02d║%02d║%02d║%02d║ ┐\n"
            "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
            "└ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
            "  ╚12╩11╩10╩09╩08╩07╝\n",
            p->cases[0], p->cases[1], p->cases[2], p->cases[3], p->cases[4], p->cases[5],
            p->cases[11], p->cases[10], p->cases[9], p->cases[8], p->cases[7], p->cases[6]
        );
        break;
    case (int)JOUEUR1* (int)AHORAIRE:
        sprintf(buffer,
            "  ╔01╦02╦03╦04╦05╦06╗\n"
            "┌ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
            "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
            " >║%02d║%02d║%02d║%02d║%02d║%02d║ ┘\n"
            "  ╚12╩11╩10╩09╩08╩07╝\n",
            p->cases[0], p->cases[1], p->cases[2], p->cases[3], p->cases[4], p->cases[5],
            p->cases[11], p->cases[10], p->cases[9], p->cases[8], p->cases[7], p->cases[6]
        );
        break;
    case (int)JOUEUR2 * (int)HORAIRE:
        sprintf(buffer,
            "  ╔07╦08╦09╦10╦11╦12╗\n"
            "> ║%02d║%02d║%02d║%02d║%02d║%02d║ ┐\n"
            "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
            "└ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
            "  ╚06╩05╩04╩03╩02╩01╝\n",
        p->cases[6], p->cases[7], p->cases[8], p->cases[9], p->cases[10], p->cases[11],
        p->cases[5], p->cases[4], p->cases[3], p->cases[2], p->cases[1], p->cases[0]
        );
        break;
    case (int)JOUEUR2 * (int)AHORAIRE:
        sprintf(buffer,
            "  ╔07╦08╦09╦10╦11╦12╗\n"
            "┌ ║%02d║%02d║%02d║%02d║%02d║%02d║ <\n"
            "│ ╠══╬══╬══╬══╬══╬══╣ │\n"
            " >║%02d║%02d║%02d║%02d║%02d║%02d║ ┘\n"
            "  ╚06╩05╩04╩03╩02╩01╝\n",
        p->cases[6], p->cases[7], p->cases[8], p->cases[9], p->cases[10], p->cases[11],
        p->cases[5], p->cases[4], p->cases[3], p->cases[2], p->cases[1], p->cases[0]
        );
        break;
    default:
        perror("joueur non défini");
        exit(errno);
    }
}
