#ifndef SYSREME_H
#define SYSREME_H



void BattementCoeur(void); // Fonction de clignotement pour verif si plantage 0.5Hz
void DWT_Init(void);
uint32_t get_time_us(void);
void Tempt_de_Boucle(void);


#endif
