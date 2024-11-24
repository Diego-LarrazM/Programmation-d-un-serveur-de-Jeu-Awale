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
/*
Function that initiate an Awale game struct

Return:
Plateau*: the Awale game struct initiated
*/

void endGame(Plateau* p);
/*
Function that delete the Awale game struct put in argument

Parameter:
Plateau* p: the Awale game struct to delete
*/

BitField_1o playableFamine(Plateau* p);
/*
Function that determine what are the houses of the current player that are playable while puting at least one seed in the opponent houses

Parameter:
Plateau* p: the Awale game struct

Return:
BitField_1o: bit field of the possible houses
*/

BitField_1o playable(Plateau* p);
/*
Function that determine the houses that are not equal to 0

Parameter:
Plateau* p: the Awale game struct

Return:
BitField_1o: bit field of the houses that are not equal to 0
*/

Bool play(Plateau* p, NumCase num_case);
/*
Function that plays the turn with the houses number indicated
(while not play if the entered house number is incorrect)

Parameters:
Plateau* p: the Awale game struct
NumCase* num_case: the house number to play

Return:
Bool: if the move was possible
*/

Bool cantPlay(Plateau* p, NumCase num_case, BitField_1o casesJouables);
/*
Function that determine if the current player can play the house number indicated in argument

Parameters:
Plateau* p: the Awale game struct
NumCase* num_case: the house number to test
BitField_1o casesJouables: the bit field of houses that can be played

Return:
Bool: if the move is NOT possible
*/

void gererDepassementPlateau(NumCase* num_case);
/*
Function that changes the value of house number put in argument if it goes out of [[0, 11]]
This function is mainly used with ++ -- to force the value to be kept inside [[0, 11]]

Parameters:
NumCase* num_case: the house number to change if needed
*/

NumCase semerGraines(Plateau* p, NumCase num_case);
/*
Function that puts seeds to the houses it needs to

Parameters:
Plateau* p: the Awale game struct
NumCase num_case: the house number to take seeds from

Return:
NumBase: the house number of the last seed put
*/

BitField_1o trouverCasesConquises(Plateau* p, NumCase num_case);
/*
Function that determine what houses of the opponent's current player would be taken if the last seed was put in the house number put in argument

Parameters:
Plateau* p: the Awale game struct
NumCase num_case: the house number where the last seed was placed

Return:
BitField_1o: bit field of all houses to capture seeds from
*/

void recolterConquetes(Plateau* p, BitField_1o casesConquises);
/*
Function that capture the seeds in the houses indicated by the bit field entered in argument

Parameters:
Plateau* p: the Awale game struct
NumCase num_case: the house number where the last seed was placed

Return:
BitField_1o: bit field of all houses to capture seeds from
*/

void changePlayer(Plateau* p);
/*
Function that change the current player

Parameter:
Plateau* p: the Awale game struct
*/

void collectAllPoints(Plateau* p);
/*
Function that collect all seeds 
*/

Bool isOpponentFamished(Plateau* p);
/*
Function that check if the current player's opponent can't play next turn if no seed are sent to his side

Parameter:
Plateau* p: the Awale game struct

Return:
Bool: if the opponent can't play next turn if no seed are sent to his side
*/

Bool hasWon(Plateau* p);
/*
Function that check if the current player has won

Parameter:
Plateau* p: the Awale game struct

Return:
Bool: if the current player has won
*/

Bool isDraw(Plateau* p);
/*
Function that check if the current player has won

Parameter:
Plateau* p: the Awale game struct

Return:
Bool: if the current player has won
*/

void printBoard(Plateau* p, Joueur joueur, char* buffer);
/*
Function that put the current board state in the buffer put in argument

Parameters:
Plateau* p: the Awale game struct
Joueur joueur: the player's perspective to print the boaed to
char* buffer: the buffer to put the board state in
*/

#endif /* guard */