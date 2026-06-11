
#include "compteur.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

//Variable volatile pour stocker le nombre de dépassements (65536 ticks par dépassement) */
static volatile uint32_t overflow_droite = 0;


// Démarre les timers (TIM3 est démarré AVEC interruption)
void Compteur_Start(void) {
    HAL_TIM_Base_Start(&htim2);        // TIM2 (32-bit) pas besoin d'interruption
    HAL_TIM_Base_Start_IT(&htim3);     // TIM3 (16-bit) AVEC interruption de débordement
}

void Compteur_Stop(void) {
    HAL_TIM_Base_Stop(&htim2);
    HAL_TIM_Base_Stop_IT(&htim3);
}


//Compteur Gauche direct (Déjà 32 bits en matériel)
uint32_t Compteur_GetGauche(void) {
    return __HAL_TIM_GET_COUNTER(&htim2);
}


//Compteur Droite Virtuel 32 bits
uint32_t Compteur_GetDroite(void)
{
    uint32_t ov1, ov2, ticks;

    do {
        // 1) On lit l’état des overflows (partie haute du compteur 32 bits)
        ov1 = overflow_droite;

        // 2) On lit le compteur matériel (partie basse 16 bits)
        ticks = __HAL_TIM_GET_COUNTER(&htim3);

        // 3) On relit les overflows pour vérifier qu’il n’a pas changé
        ov2 = overflow_droite;

         /* Si ov1 != ov2 :
          -> un overflow est survenu pendant la lecture
          -> la valeur est incohérente
          -> on recommence
         */
    } while (ov1 != ov2);

// On décale le nombre d'overflows de 16 bits vers la gauche et on ajoute le compteur matériel, total = (ov1 * 65536) + ticks
    return (ov1 << 16) | ticks;
}
void Compteur_ResetGauche(void) {
    __HAL_TIM_SET_COUNTER(&htim2, 0);
}


//Réinitialise le compteur matériel ET logiciel du côté droit
void Compteur_ResetDroite(void) {
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    overflow_droite = 0; // On remet le compteur d'overflow à zéro
}


//Fonction de rappel (Callback) appelée automatiquement par la HAL lors d'un overflow
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM3) {
        overflow_droite++; // +1 virtuel (équivaut à +65536 ticks au global)
    }
}
