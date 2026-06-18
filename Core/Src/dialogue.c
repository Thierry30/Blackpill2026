#include "dialogue.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include <stdlib.h> // Requis pour strtol
#include <stdarg.h> // Requis pour gérer les arguments variables (...)
#include "gestion_vitesse.h"

// ================= CONFIG =================

#define RX_SIZE 128

// ================= DMA UART =================

static uint8_t rx_dma[RX_SIZE] __attribute__((aligned(32)));
static volatile uint16_t old_pos;

// ================= FIFO =================

typedef struct {
    uint8_t buffer[DIALOGUE_FIFO_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
} fifo_t;
static fifo_t rx_fifo;

// ================= LINE BUFFER =================

static char line_buffer[DIALOGUE_LINE_SIZE];
static uint16_t line_index = 0;

// ================= FIFO FUNCTIONS =================

static inline void fifo_push(fifo_t *f, uint8_t c)
{
    uint16_t next = (f->head + 1) % DIALOGUE_FIFO_SIZE;

    if(next != f->tail)
    {
        f->buffer[f->head] = c;
        f->head = next;
    }
}

static inline int fifo_pop(fifo_t *f, uint8_t *c)
{
    if(f->head == f->tail)
        return 0;

    *c = f->buffer[f->tail];
    f->tail = (f->tail + 1) % DIALOGUE_FIFO_SIZE;

    return 1;
}

// ================= INIT =================

void Dialogue_Init(void)
{
    rx_fifo.head = 0;
    rx_fifo.tail = 0;
    line_index = 0;
    old_pos = 0;

    HAL_UART_Receive_DMA(&huart2, rx_dma, RX_SIZE);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
}

// ================= PUSH =================

void Dialogue_PushBytes(uint8_t *data, uint32_t len)
{
    for(uint32_t i = 0; i < len; i++)
        fifo_push(&rx_fifo, data[i]);
}

// ================= PROCESS =================

void Dialogue_Process(void)
{
    uint8_t c;
    static uint8_t overflow_mode = 0;

    while (fifo_pop(&rx_fifo, &c))
    {
        // 1. Si on reçoit un caractère de fin (le '*' coupe la trame ici)
        if (c == '\n' || c == '\r' || c == '*' || c == '#')
        {
            if (overflow_mode)
            {
                overflow_mode = 0;
                line_index = 0;
            }
            else if (line_index > 0)
            {
                line_buffer[line_index] = '\0'; // Le '*' est remplacé par '\0'
                Dialogue_OnCommand(line_buffer);
                line_index = 0;
            }
        }
        // 2. Mode débordement
        else if (overflow_mode)
        {
            continue;
        }
        // 3. Remplissage normal
        else
        {
            if (line_index < DIALOGUE_LINE_SIZE - 1)
            {
                line_buffer[line_index++] = c;
            }
            else
            {
                overflow_mode = 1;
                line_index = 0;
            }
        }
    }
}

// ================= IDLE CALLBACK =================

void Dialogue_UART_IDLE_Callback(void)
{
    uint16_t pos = RX_SIZE - __HAL_DMA_GET_COUNTER(huart2.hdmarx);

    if (pos != old_pos)
    {
        if (pos > old_pos)
        {
            Dialogue_PushBytes(&rx_dma[old_pos], pos - old_pos);
        }
        else
        {
            Dialogue_PushBytes(&rx_dma[old_pos], RX_SIZE - old_pos);
            if (pos > 0)
                Dialogue_PushBytes(&rx_dma[0], pos);
        }

        old_pos = pos;
    }
}

// ================= PARSING CORRIGÉ =================

Command_t Communication_Parse(const char *cmd_str)
{
    Command_t result = {0}; // is_valid = 0 par défaut
    int len = strlen(cmd_str);

    // CORRECTION 1 : La longueur minimale sans le '*' est de 3 caractères (ex: "$V5")
    if (len < 3) {
        return result;
    }

    // CORRECTION 2 : On ne vérifie que le début. Le '*' a déjà été retiré par Dialogue_Process
    if (cmd_str[0] != '$') {
        return result;
    }

    // 3. Extraction de la lettre de commande
    result.cmd_type = cmd_str[1];

    // 4. Extraction de la valeur avec strtol
    char *endptr;
    result.value = strtol(&cmd_str[2], &endptr, 10);

    /* CORRECTION 3 : Sécurité strtol
     * Si endptr pointe sur '\0', c'est que toute la chaîne après la lettre
     * était un nombre valide (pas de caractères parasites).
     * On s'assure aussi qu'au moins un chiffre a été lu (endptr différent du début).
     */
    if (endptr != &cmd_str[2] && *endptr == '\0') {
        result.is_valid = 1;
    }

    return result;
}

// ================= EXECUTION =================

void Dialogue_OnCommand(char *cmd)
{
    Command_t msg = Communication_Parse(cmd);

    if (msg.is_valid)
    {
        Communication_Execute(&msg);
    }
    else
    {
        // Affiche la chaîne nettoyée pour le debug
        printf("Erreur: Trame invalide ou mal formee : %s\r\n", cmd);
    }
}

void Communication_Execute(const Command_t *cmd)
{
    printf("Commande recue -> Type: %c, Valeur: %ld\r\n", cmd->cmd_type, cmd->value);

    switch (cmd->cmd_type)
    {
		  case 'V':
			Set_Vmax((int16_t)cmd->value);

			//Uniquement pour debug et mise au point
			printf("Action: Mise a jour de la vitesse a %d%%\r\n", Get_Vmax());
			break;


        case 'A': // Cas de l'Accélération
            printf("Action: Mise a jour de l'acceleration a %ld\r\n", cmd->value);
            break;

        default:
            printf("Commande '%c' inconnue ou non geree.\r\n", cmd->cmd_type);
            break;
    }
}



// Buffer statique dédié à l'envoi DMA pour éviter les corruptions de mémoire
static uint8_t tx_dma_buffer[128];


//Utiliser comme printf, ça part sur huart2 BT
void Bluetooth_Send_DMA(const char *format, ...)
{
	// Si le DMA est occupé à envoyer autre chose, on quitte IMMEDIATEMENT (le message est ignoré)
	    if (huart2.gState == HAL_UART_STATE_BUSY_TX)
	    {
	        return;
	    }
    va_list args;
    va_start(args, format);
    // vsnprintf remplace les %d, %s par leurs vraies valeurs dans le buffer
    int len = vsnprintf((char*)tx_dma_buffer, sizeof(tx_dma_buffer), format, args);
    va_end(args);

    if (len > 0)
    {
        // On lance l'envoi en DMA. Le CPU passe immédiatement à la ligne suivante !
        HAL_UART_Transmit_DMA(&huart2, tx_dma_buffer, len);
    }
}
