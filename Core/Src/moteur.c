#include "moteur.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

extern TIM_HandleTypeDef htim1;

/* ===== CONFIG ===== */
#define PWM_MAX        99     // Résolution du PWM (0 à 99)
#define RAMP_STEP      1      // Variation de vitesse max par cycle (5ms)
#define RAMP_PERIOD_MS 1      // Appel de Moteur_Traitement toutes les 1 ms

/* ===== STRUCT ===== */
typedef struct
{
    int16_t consigne;   // Vitesse demandée (-99 à +99)
    int16_t actuel;     // Vitesse appliquée filtrée par la rampe
    EtatMoteur_t etat;  // État ou mode de fonctionnement actuel
} Moteur_t;

/* ===== VARIABLES ===== */
static Moteur_t moteurG = {0, 0, MOTEUR_ROUE_LIBRE};
static Moteur_t moteurD = {0, 0, MOTEUR_ROUE_LIBRE};

/* ===== UTILS ===== */
//Valeur absolue de la vitessse
static uint16_t valeurAbsolue(int16_t v)
{
    return (v < 0) ? -v : v;
}

/* ===== LOW LEVEL ===== */
static void AppliquerMoteurGauche(int16_t v, EtatMoteur_t etat)
{
    /* ===== 1. MODES FORCÉS ===== */
    if (etat == MOTEUR_FREIN)
    {
        HAL_GPIO_WritePin(AIN1_MotG_GPIO_Port, AIN1_MotG_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(AIN2_MotG_GPIO_Port, AIN2_MotG_Pin, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        return;
    }

    if (etat == MOTEUR_ROUE_LIBRE)
    {
        HAL_GPIO_WritePin(AIN1_MotG_GPIO_Port, AIN1_MotG_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(AIN2_MotG_GPIO_Port, AIN2_MotG_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        return;
    }

    /* ===== 2. MODE NORMAL ===== */
    if (v == 0)
    {
        HAL_GPIO_WritePin(AIN1_MotG_GPIO_Port, AIN1_MotG_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(AIN2_MotG_GPIO_Port, AIN2_MotG_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
        return;
    }

    if (v > 0)
    {
        HAL_GPIO_WritePin(AIN1_MotG_GPIO_Port, AIN1_MotG_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(AIN2_MotG_GPIO_Port, AIN2_MotG_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(AIN1_MotG_GPIO_Port, AIN1_MotG_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(AIN2_MotG_GPIO_Port, AIN2_MotG_Pin, GPIO_PIN_SET);
    }

    // ✨ Correction ici : on passe la valeur absolue au Timer
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, valeurAbsolue(v));
}

static void AppliquerMoteurDroite(int16_t v, EtatMoteur_t etat)
{
    /* ===== 1. MODES FORCÉS ===== */
    if (etat == MOTEUR_FREIN)
    {
        HAL_GPIO_WritePin(BIN1_MotD_GPIO_Port, BIN1_MotD_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BIN2_MotD_GPIO_Port, BIN2_MotD_Pin, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
        return;
    }

    if (etat == MOTEUR_ROUE_LIBRE)
    {
        HAL_GPIO_WritePin(BIN1_MotD_GPIO_Port, BIN1_MotD_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BIN2_MotD_GPIO_Port, BIN2_MotD_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
        return;
    }

    /* ===== 2. MODE NORMAL ===== */
    if (v == 0)
    {
        HAL_GPIO_WritePin(BIN1_MotD_GPIO_Port, BIN1_MotD_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BIN2_MotD_GPIO_Port, BIN2_MotD_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
        return;
    }

    if (v > 0)
    {
        HAL_GPIO_WritePin(BIN1_MotD_GPIO_Port, BIN1_MotD_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(BIN2_MotD_GPIO_Port, BIN2_MotD_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(BIN1_MotD_GPIO_Port, BIN1_MotD_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BIN2_MotD_GPIO_Port, BIN2_MotD_Pin, GPIO_PIN_SET);
    }

    // ✨ Correction ici aussi
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, valeurAbsolue(v));
}
/* ===== INIT ===== */

void Moteur_Initialiser(void)
{
    /* ÉTAT INTERNE SÉCURISÉ */
    moteurG.consigne = 0;
    moteurG.actuel   = 0;
    moteurG.etat     = MOTEUR_ROUE_LIBRE;

    moteurD.consigne = 0;
    moteurD.actuel   = 0;
    moteurD.etat     = MOTEUR_ROUE_LIBRE;

    /* ARRÊT HARDWARE IMMÉDIAT (Sécurité GPIO) */
    HAL_GPIO_WritePin(AIN1_MotG_GPIO_Port, AIN1_MotG_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIN2_MotG_GPIO_Port, AIN2_MotG_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BIN1_MotD_GPIO_Port, BIN1_MotD_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BIN2_MotD_GPIO_Port, BIN2_MotD_Pin, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);

    /* DÉMARRAGE DES PWM STANDARD */
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

    Moteur_Arreter(MOTEUR_ROUE_LIBRE);
}

/* ===== API ===== */

void Moteur_DefinirGauche(int8_t v)
{
    if (v > PWM_MAX) v = PWM_MAX;
    if (v < -PWM_MAX) v = -PWM_MAX;
    moteurG.consigne = v;
    /* On ne repasse en mode normal QUE si on demande au moteur de bouger.
       Si v == 0, on ne touche pas à l'état (on préserve un éventuel FREIN). */
        if (v != 0)
        {
            moteurG.etat = MOTEUR_NORMAL;
        }
}

void Moteur_DefinirDroite(int8_t v)
{
    if (v > PWM_MAX) v = PWM_MAX;
    if (v < -PWM_MAX) v = -PWM_MAX;
    moteurD.consigne = v;
    /* On ne repasse en mode normal QUE si on demande au moteur de bouger.
       Si v == 0, on ne touche pas à l'état (on préserve un éventuel FREIN). */
           if (v != 0)
           {
               moteurD.etat = MOTEUR_NORMAL;
           }
}
/* ===== MODES FORCÉS ===== */

void Moteur_Gauche_Mode(EtatMoteur_t mode)
{
    moteurG.etat = mode;
}

void Moteur_Droit_Mode(EtatMoteur_t mode)
{
    moteurD.etat = mode;
}

void Moteur_Arreter(EtatMoteur_t mode)
{
    moteurG.consigne = 0;
    moteurD.consigne = 0;
    moteurG.actuel   = 0; // Reset de la rampe pour un arrêt immédiat
    moteurD.actuel   = 0;
    moteurG.etat     = mode;
    moteurD.etat     = mode;
}

/* ===== RAMP (ANTI-COURANT) ===== */

static void Rampe(Moteur_t *m)
{
    // Ne calcule la rampe que si on est en mode de marche normal
    if (m->etat != MOTEUR_NORMAL) return;

    if (m->actuel < m->consigne)
    {
        m->actuel += RAMP_STEP;
        if (m->actuel > m->consigne) m->actuel = m->consigne;
    }
    else if (m->actuel > m->consigne)
    {
        m->actuel -= RAMP_STEP;
        if (m->actuel < m->consigne) m->actuel = m->consigne;
    }
}

/* ===== PROCESS ===== */

void Moteur_Traitement(void)
{
    Rampe(&moteurG);
    Rampe(&moteurD);

    AppliquerMoteurGauche(moteurG.actuel, moteurG.etat);
    AppliquerMoteurDroite(moteurD.actuel, moteurD.etat);
}
