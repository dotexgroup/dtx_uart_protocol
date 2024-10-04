
/*
 * uart_protocol.c
 *
 *  Created on: Sep 24, 2024
 *      Author: Dotex:.
 */
#include "uart_protocol.h"
/*----------------------------------------------*/
UART_Protocol_HandleTypeDef uartProtocol;
/*----------------------------------------------*/
void UART_Protocol_Init(UART_Protocol_HandleTypeDef *protocol, UART_HandleTypeDef *huart) {
    protocol->huart = huart;
    protocol->rxState = WAIT_FOR_START;
    protocol->dataLength = 0;
    protocol->receivedBytes = 0;
    protocol->checksum = 0;
    protocol->receivedChecksum = 0;
    protocol->isDataReady = 0;

    HAL_UART_Receive_IT(protocol->huart, &protocol->rxByte, 1);
}
/*----------------------------------------------*/
uint8_t UART_Protocol_Transmit(UART_Protocol_HandleTypeDef *protocol, uint8_t *data, uint8_t length) {
    uint8_t frame[MAX_DATA_LENGTH + 4];
    frame[0] = START_BYTE;
    frame[1] = length;

    for (uint8_t i = 0; i < length; i++) {
        frame[2 + i] = data[i];
    }
    frame[2 + length] = UART_Protocol_CalculateChecksum(data, length);
    frame[3 + length] = END_BYTE;
    HAL_UART_Transmit(protocol->huart, frame, length + 4, 100);
#if EN_ACK
    // wait for (ACK/NACK)
    uint8_t response;
    HAL_UART_Receive(protocol->huart, &response, 1, 100);
    // check ACK or NACK
    if (response == ACK_BYTE) {
        /* send successful*/
    	return 0 ;
    } else {
    	/* send not successful
    	 * try again or call  UART_Protocol_Transmit again or ...*/
    	HAL_UART_Receive(protocol->huart, &response, 1, 100);
    	return 1 ;
    }
#else
    return 0 ;
#endif
}
/*----------------------------------------------*/
void UART_Protocol_Receive_IT(UART_Protocol_HandleTypeDef *protocol) {
    switch (protocol->rxState) {
        case WAIT_FOR_START:
            if (protocol->rxByte == START_BYTE) {
                protocol->rxState = RECEIVE_LENGTH;
                protocol->checksum = 0;
            }
            break;

        case RECEIVE_LENGTH:
            protocol->dataLength = protocol->rxByte;
            protocol->receivedBytes = 0;
            protocol->rxState = RECEIVE_DATA;
            break;

        case RECEIVE_DATA:
            protocol->rxBuffer[protocol->receivedBytes++] = protocol->rxByte;
            protocol->checksum ^= protocol->rxByte;
            if (protocol->receivedBytes >= protocol->dataLength) {
                protocol->rxState = RECEIVE_CHECKSUM;
            }
            break;

        case RECEIVE_CHECKSUM:
            protocol->receivedChecksum = protocol->rxByte;
            protocol->rxState = WAIT_FOR_END;
            break;

        case WAIT_FOR_END:
            if (protocol->rxByte == END_BYTE) {
                if (protocol->checksum == protocol->receivedChecksum) {
                    protocol->isDataReady = 1;
                    uint8_t ack = ACK_BYTE;
                    HAL_UART_Transmit(protocol->huart, &ack, 1, HAL_MAX_DELAY);
                } else {
                    protocol->isDataReady = 0;
                    uint8_t nack = NACK_BYTE;
                    HAL_UART_Transmit(protocol->huart, &nack, 1, HAL_MAX_DELAY);
                }
            }
            protocol->rxState = WAIT_FOR_START;
            break;
    }

    HAL_UART_Receive_IT(protocol->huart, &protocol->rxByte, 1);
}
/*----------------------------------------------*/
uint8_t UART_Protocol_CalculateChecksum(uint8_t *data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}
/*----------------------------------------------*/
