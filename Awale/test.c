#include "awale.h"
#include <stdio.h>

void bitfieldToString(BitField_1o cases, char* buffer){
  NumCase i = 1;
  NumCase j = 0;
    while(cases){
      buffer[j] = 48 + i;
      buffer[j + 1] = ' ';
      ++i;
      j += 2;
      cases >>= 1;
      
    }
    buffer[j] = '\0';
}

int main(){
  char* joueur[2] = {"toto1", "pico2"};
  Plateau* plateauJeu = init();
  Bool playing = true;
  while(playing){
    BitField_1o casesJouables = isOpponentFamished(plateauJeu) ? playableFamine(plateauJeu) : 63;
    char casesJouablesStr[13];
    bitfieldToString(casesJouables, casesJouablesStr);
    NumCase caseAJouer;

    printf("Il est votre tour de jouer %s!\n",joueur[(int)(plateauJeu->JoueurCourant) - 1]);
    do {
      printf("Choissisez une case parmis: %s\n", casesJouablesStr);
      scanf("%d", &caseAJouer);
      caseAJouer -= 1;
    } while (!play(plateauJeu, caseAJouer));

    playing = !(hasWon(plateauJeu) || isDraw(plateauJeu));
    changePlayer(plateauJeu);
  }
  end(plateauJeu);
  return 0;
}