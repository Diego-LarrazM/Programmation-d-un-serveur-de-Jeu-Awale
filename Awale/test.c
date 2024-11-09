#include "awale.h"
#include <stdio.h>

char* bitfieldToString(BitField_1o cases){
  NumCase i = 1;
  char strCases[6];
    while(cases){
      strCases[i] = 48 + i;
      ++i;
      cases >> 1;
      
    }
    strCases[i] = '\0';
    return strCases;
}

int main(){
  char* joueur[2] = {"toto1", "pico2"};
  Plateau* plateauJeu = init();
  Bool playing = true;
  while(playing){
    BitField_1o casesJouables = playableFamine(plateauJeu);
    char* casesJouablesStr = bitfieldToString(casesJouables);
    NumCase caseAJouer;

    printf("Il est votre tour de jouer %s!\n",joueur[(int)(plateauJeu->JoueurCourant) - 1]);
    do {
      printf("Choissisez une case parmis: %s\n", casesJouablesStr);
      scanf("%d", &caseAJouer);
    } while (!play(plateauJeu, caseAJouer));

    playing = not(hasWon(plateauJeu) || isDraw(plateauJeu));
    changePlayer(plateauJeu);
  }
  end(plateauJeu);
  
}