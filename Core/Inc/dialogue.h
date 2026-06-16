#ifndef DIALOGUE_H
#define DIALOGUE_H

#include <stdint.h>

// ================= CONFIG =================

// Optionnel : tu peux les laisser ici ou dans le .c
#define DIALOGUE_FIFO_SIZE 512
#define DIALOGUE_LINE_SIZE 128


// ================= API =================

// Initialisation (UART DMA + reset FIFO)
void Dialogue_Init(void);

// Traitement (à appeler dans le while(1))
void Dialogue_Process(void);

// Injection de données (USB / UART / autre)
void Dialogue_PushBytes(uint8_t *data, uint32_t len);

// Callback IDLE UART (appelé depuis IRQ)
void Dialogue_UART_IDLE_Callback(void);

// ================= CALLBACK UTILISATEUR =================

// À redéfinir dans ton code applicatif
void Dialogue_OnCommand(char *cmd);

// Structure pour stocker le résultat du décodage d'une commande
typedef struct {
    char cmd_type;      // La lettre de la commande (ex: 'V' pour vitesse)
    int32_t value;      // La valeur associée (positive ou négative)
    uint8_t is_valid;   // 1 si la commande est valide, 0 sinon
} Command_t;

/**
 * @brief Analyse la chaîne reçue et extrait la commande et sa valeur.
 * @param cmd_str Pointeur vers la chaîne de caractères brute reçue.
 * @return Command_t Structure contenant les données décodées.
 */
Command_t Communication_Parse(const char *cmd_str);

/**
 * @brief Traite la commande reçue en fonction de son type (Logique métier).
 * @param cmd Pointeur vers la structure de la commande décodée.
 */
void Communication_Execute(const Command_t *cmd);

void Bluetooth_Send_DMA(const char *format, ...);

#endif
