#include "dialogue.h"
#include "moteur.h"


static int16_t Vmax = 50;


//Normalisation de la vitesse max reçu par pc ou bt
void Set_Vmax(int16_t vitesse)
{
    if (vitesse > 100)  vitesse = 100;
    if (vitesse < -100) vitesse = -100;
    Vmax = vitesse;
}

//Recupére la valeur Vmax reçu par PC ou BT
int16_t Get_Vmax(void)
{
    return Vmax;
}
