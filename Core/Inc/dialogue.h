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

#endif
