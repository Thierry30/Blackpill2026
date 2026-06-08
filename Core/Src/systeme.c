#include "main.h"
#include "systeme.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>



//==== Fonction de clignotement pour verifier que le programme n'est pas planté 0.5Hz=============
void BattementCoeur(void) {
  static uint32_t last_tick = 0; 				// Stocke le dernier tick
  uint32_t current_tick = HAL_GetTick(); 		// Temps courant en ms

  if ((current_tick - last_tick) >= 500) { 		// Vérifie si 500 ms sont écoulées
	  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);  	// Inverse l'état de PA5

	  last_tick = current_tick;				 	// Met à jour le dernier tick
  }
}


//===========================Pour compteur en microsecone================================
uint32_t cycles_per_us;


//Pour compteur en microsecon
void DWT_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    cycles_per_us = SystemCoreClock / 1000000;
}
//Pour compteur en microsecone
uint32_t get_time_us(void) {
    return DWT->CYCCNT / cycles_per_us;
}


