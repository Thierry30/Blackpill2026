#ifndef MOTEUR_H
#define MOTEUR_H

#include "main.h"
#include <stdint.h>

/* ===== ETATS MOTEUR ===== */
typedef enum {
    MOTEUR_NORMAL,
    MOTEUR_FREIN,
    MOTEUR_ROUE_LIBRE
} EtatMoteur_t;

/* ===== API ===== */
void Moteur_Initialiser(void);

void Moteur_DefinirGauche(int8_t vitesse);   // -100 à +100
void Moteur_DefinirDroite(int8_t vitesse);

void Moteur_DefinirEtatGauche(EtatMoteur_t etat);
void Moteur_DefinirEtatDroite(EtatMoteur_t etat);

void Moteur_Arreter(EtatMoteur_t mode);

void Moteur_Traitement(void);

#endif
