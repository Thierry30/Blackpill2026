
#ifndef INC_COMPTEUR_H_
#define INC_COMPTEUR_H_

#include "stm32f4xx_hal.h"

void Compteur_Start(void);
void Compteur_Stop(void);

uint32_t Compteur_GetGauche(void);
uint32_t Compteur_GetDroite(void); // Passé en uint32_t !

void Compteur_ResetGauche(void);
void Compteur_ResetDroite(void);

#endif /* INC_COMPTEUR_H_ */
