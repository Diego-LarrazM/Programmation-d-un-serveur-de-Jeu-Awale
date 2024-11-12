#include "awale.h"
#include <stdio.h>

void bitfieldToString(Joueur JCourant, BitField_1o cases, char* buffer){
  NumCase i = JCourant == JOUEUR2 ? 1 : 7;
  NumCase j = 0;
    while(cases){
      if(cases & 1){
        if(i >= 10){
          buffer[j++] = 48 + i/10;
          buffer[j++] = 48 + i%10;
          buffer[j++] = ' ';
        }
        else{
          buffer[j++] = 48 + i%10;
          buffer[j++] = ' ';
        }
      }  
      ++i;
      cases >>= 1;
    }
    buffer[j] = '\0';
}

int main(){
  char* joueur[2] = {"toto1", "pico2"};
  char casesJouablesStr[20];
  char boardStr[300];
  NumCase caseAJouer;
  BitField_1o casesJouables;
  Plateau* plateauJeu = init();
  while(!(hasWon(plateauJeu) || isDraw(plateauJeu))){
    casesJouables = isOpponentFamished(plateauJeu) ? playableFamine(plateauJeu) : 63;
    bitfieldToString(plateauJeu->JoueurCourant, casesJouables, casesJouablesStr);
    printBoard(plateauJeu, boardStr);
    printf("Il est votre tour de jouer %s!\n",joueur[(int)(plateauJeu->JoueurCourant) - 1]);
    do {
      printf("%s\n",boardStr);
      printf("Choissisez une case parmis: %s\n", casesJouablesStr);  
      scanf("%d", &caseAJouer);
      caseAJouer -= 1;
    } while (!play(plateauJeu, caseAJouer));

    changePlayer(plateauJeu);
  }
  printBoard(plateauJeu, boardStr);
  return 0;
  }