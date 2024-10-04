/*
 * uart_protocol.h
 *
 *  Created on: Sep 24, 2024
 *      Author: Dotex:.
 */
#ifndef UART_PROTOCOL_H
#define UART_PROTOCOL_H
/*----------------------------------------------*/
#include "main.h"
/*----------------------------------------------*/

#define EN_ACK		(1)
#define START_BYTE  0x7E
#define END_BYTE    0x7F
#define ACK_BYTE  	0xA5
#define NACK_BYTE 	0x5A

#define MAX_DATA_LENGTH 20
/*----------------------------------------------*/
typedef enum {
    WAIT_FOR_START,
    RECEIVE_LENGTH,
    RECEIVE_DATA,
    RECEIVE_CHECKSUM,
    WAIT_FOR_END
} RxState;
typedef struct {
    UART_HandleTypeDef *huart;  // UART handle
    RxState rxState;            // current state
    uint8_t rxBuffer[MAX_DATA_LENGTH];
    uint8_t rxByte;
    uint8_t dataLength;
    uint8_t receivedBytes;
    uint8_t checksum;
    uint8_t receivedChecksum;
    uint8_t isDataReady;
} UART_Protocol_HandleTypeDef;
extern UART_Protocol_HandleTypeDef uartProtocol;
/*----------------------------------------------*/
void UART_Protocol_Init(UART_Protocol_HandleTypeDef *protocol, UART_HandleTypeDef *huart);
void UART_Protocol_Receive_IT(UART_Protocol_HandleTypeDef *protocol);
uint8_t UART_Protocol_Transmit(UART_Protocol_HandleTypeDef *protocol, uint8_t *data, uint8_t length);
uint8_t UART_Protocol_CalculateChecksum(uint8_t *data, uint8_t length);
/*----------------------------------------------*/
#endif
