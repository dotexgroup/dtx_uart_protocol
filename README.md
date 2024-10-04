# UART Communication Library

This UART communication library provides a reliable way to exchange data between two STM32 microcontrollers using UART. The library utilizes a custom protocol that includes:
- **Start byte**: Marks the beginning of the frame.
- **Stop byte**: Marks the end of the frame.
- **Checksum**: Ensures data integrity.
- **Variable-length data**: Supports sending frames of different sizes.
- **ACK/NACK**: Mechanism to confirm successful reception or request retransmission in case of errors.

## Key Features
1. **Start/Stop Bytes**: Each frame starts with a special byte and ends with another, clearly marking the frame's boundaries.
2. **Checksum**: Verifies the integrity of transmitted data.
3. **Variable-length Data**: Supports different data sizes in the frames.
4. **ACK/NACK**: After receiving a frame, the receiver sends back an ACK (acknowledgment) or a NACK (negative acknowledgment).

## Functions
1. **`UART_Protocol_Transmit_ByteByByte`**: Sends a data frame byte by byte.
2. **`UART_Protocol_Receive_IT`**: Receives data asynchronously via UART interrupts.
3. **Checksum Calculation**: Used to validate the data integrity.

## How to Use the Library

### Initialization
Before using the library, initialize the UART handle (e.g., `huart2`) in your main program.

### Sending Data
You can send data using the `UART_Protocol_Transmit_ByteByByte` function:
```c
uint8_t data[] = {0x01, 0x02, 0x03};  // Example data to send
UART_Protocol_Transmit_ByteByByte(&uartProtocolHandle, data, sizeof(data));
```  
### Receiving Data
To receive data, use the `HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)` function inside the UART interrupt handler. The received data is handled asynchronously.

Example:
```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart2 ){
		UART_Protocol_Receive_IT(&uartProtocol);
		HAL_UART_Receive_IT(uartProtocol.huart, &uartProtocol.rxByte, 1);
	}
}
```


### Handling Received Data
After receiving the data, check if it's ready using the `isDataReady`  flag:
```c
if (uartProtocolHandle.isDataReady) {
    // Process the received data
    uartProtocolHandle.isDataReady = 0;  // Reset the flag after processing
}
```

