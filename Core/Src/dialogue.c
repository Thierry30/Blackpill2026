#include "dialogue.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"

// ================= CONFIG =================


#define RX_SIZE 128


// ================= DMA UART =================

//static uint8_t rx_dma[RX_SIZE];

static uint8_t rx_dma[RX_SIZE] __attribute__((aligned(32)));
static volatile uint16_t old_pos;

// ================= FIFO =================

typedef struct {
    uint8_t buffer[DIALOGUE_FIFO_SIZE];
    volatile uint16_t head;  // Supporte 0-255
    volatile uint16_t tail;
} fifo_t;
static fifo_t rx_fifo;

// ================= LINE BUFFER =================

static char line_buffer[DIALOGUE_LINE_SIZE];
static uint16_t line_index = 0;

// ================= FIFO =================

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

    // Enregistrer callback d'erreur - VERSION CORRIGÉE

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

    while (fifo_pop(&rx_fifo, &c))

    {
        if (c == '\n' || c == '\r'|| c=='#')
        {
            if (line_index > 0)
            {
                line_buffer[line_index] = '\0';
                Dialogue_OnCommand(line_buffer);
                line_index = 0;
            }
        }
        else
        {
            if (line_index < DIALOGUE_LINE_SIZE - 1)
                line_buffer[line_index++] = c;
            else
                line_index = 0;
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
//======================erreur dma==================

// ================= CALLBACK USER =================

__attribute__((weak)) void Dialogue_OnCommand(char *cmd)
{
    //(void)cmd; // évite warning
	printf("chaine USB/BT: %s\r\n", cmd);
}
