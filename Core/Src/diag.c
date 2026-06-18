#include "diag.h"
#include "moteur.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "gestion_vitesse.h"
#include "dialogue.h"

static int16_t Vdiag = 0;


//pour verif alim ou fonctionnement + réception commande
void Diag_moteur(void){
	Vdiag =  Get_Vmax();
	Moteur_DefinirGauche(Vdiag);
	Moteur_DefinirDroite(Vdiag);
	Moteur_Traitement();

	Bluetooth_Send_DMA("*V%d*",Vdiag);
}

