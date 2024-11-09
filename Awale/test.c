#include "awale.h"
#include <stdio.h>

void bitfieldToString(BitField_1o cases, char* buffer){
  NumCase i = 1;
    while(cases){
      buffer[i] = 48 + i;
      ++i;
      cases >> 1;
      
    }
    buffer[i] = '\0';
}

int main(){
  char* joueur[2] = {"toto1", "pico2"};
  Plateau* plateauJeu = init();
  Bool playing = true;
  while(playing){
    BitField_1o casesJouables = playableFamine(plateauJeu);
    char* casesJouablesStr;
    bitfieldToString(casesJouables, casesJouablesStr);
    NumCase caseAJouer;

    printf("Il est votre tour de jouer %s!\n",joueur[(int)(plateauJeu->JoueurCourant) - 1]);
    do {
      printf("Choissisez une case parmis: %s\n", casesJouablesStr);
      scanf("%d", &caseAJouer);
    } while (!play(plateauJeu, caseAJouer));

    playing = !(hasWon(plateauJeu) || isDraw(plateauJeu));
    changePlayer(plateauJeu);
  }
  end(plateauJeu);
  return 0;
}