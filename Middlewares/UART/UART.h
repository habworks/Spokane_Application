/*
 * UART.h
 *
 *  Created on: Apr 10, 2020
 *      Author: devink
 */


#ifndef UART_UART_H_
#define UART_UART_H_

#include "main.h"
#include <Queue/Queue.h>
#include <stdbool.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/// Dafault size in bytes for the received buffer.
#define UART_RX_BUFF_SIZE		512

/// Data structure to hold UART transmit request
///
/// This data structure will be generated when UART_Transmit() is called and will be placed in UART.TX_Queue.
typedef struct
{
	/// Pointer to the data to be transmitted.
	uint8_t * Data;
	/// Size of the data that is pointed to by Data.
	uint16_t Data_Size;
}UART_TX_Node;


/// Core data structure for the uart peripheral
typedef struct
{
	/// Holds the pointer to the STM32 uart handle that was passed in Init_DMA_UART()
	UART_HandleTypeDef * UART_Handle;
	/// Flag for future use if the driver ever uses anything other than DMA
	bool Use_DMA;
	/// Flag if the UART is in an enable state or not
	bool UART_Is_Enabled;

	/// Buffer passed to the DMA to receive data.
	uint8_t RX_Buffer[UART_RX_BUFF_SIZE];
	/// Holds the position of the laster character read using UART_Receive()
	uint32_t RX_Buff_Tail_Ptr;

	/// Queue of packets that need to be transmitted.
	Queue TX_Queue;
	/// Pointer to the packet that is currently being transmitted.
	UART_TX_Node * TX_Buffer;
	/// Flag if the uart is currently transmitting or idle.
	volatile bool Currently_Transmitting;

	// Function Pointer for a SUDO UART
	int32_t (*SUDO_Transmit)(uint8_t * Data, uint16_t Data_Size);
	int32_t (*SUDO_Receive)(uint8_t * Data, uint16_t * Data_Size);
	uint32_t RX_Buff_Head_Ptr;

	/// Holds the return value of Start_Task()
	uint32_t Task_ID;
}UART;


/// Initialization function into UART.h
///
/// @param UART_Handle Takes an STM HAL UART pointer.
/// @returns Returns a pointer to a UART structure used by all modifying functions
UART * Init_DMA_UART(UART_HandleTypeDef * UART_Handle);

/// Initialization function into UART.h. This feature is intended to help convert device drivers from using UART to USB CDC.
/// The function pointers are used to point to a different bus read and write so that the higher level application is abstracted
/// out.
///
/// @param Function pointer to a transmit function
/// @param Function pointer to a receive function
/// @returns Returns a pointer to a UART structure used by all modifying functions
UART * Init_SUDO_UART(int32_t (* Transmit_Func_PTR)(uint8_t *, uint16_t), int32_t (* Receive_Func_PTR)(uint8_t *, uint16_t *));

/// Enable a UART that has previously been disabled
///
/// @param UART_Handle Takes a UART handle the is generated by calling Ini_DMA_UART() or Init_SUDO_UART()
void Enable_UART(UART * Bus);

/// Disable a UART that has previously been initialized
///
/// @param UART_Handle Takes a UART handle the is generated by calling Ini_DMA_UART() or Init_SUDO_UART()
void Disable_UART(UART * Bus);

/// Transmits data over a UART bus
///
/// @param uart pointer returned from Init_DMA_UART()
/// @param Data Pointer to the data that is requested to be transmitted
/// @param Data_Size Size of the data that is pointed to in Data
/// @returns The count of the amount of data that was transmitted.
int32_t UART_Transmit(UART * uart, uint8_t * Data, uint16_t Data_Size);

/// Returns received data over a UART bus
///
/// @param uart pointer returned from Init_DMA_UART()
/// @param Data Pointer to where the received data should be copied.
/// @param Data_Size Pointer to a variable that will result in how much data was placed in Data pointer.
/// @returns Returns 0 on completion.
/// @warning The function can return data sizes up to UART_RX_BUFF_SIZE, take care that the buffer pointed to by Data should be at least of size UART_RX_BUFF_SIZE.
/// @see UART_RX_BUFF_SIZE
int32_t UART_Receive(UART * uart, uint8_t * Data, uint16_t * Data_Size);

/// Changes the UART baud rate
///
/// @param uart Pointer returned from Init_DMA_UART()
/// @param New_Baudrate The desired new baud rate for the UART port.
/// @note It is best to call UART_Flush_TX() to flush out the transmit queue before calling this function.
void Modify_UART_Baudrate(UART * uart, uint32_t New_Baudrate);

/// Flushes the transmit buffer.
///
/// @param uart Pointer returned from Init_DMA_UART()
/// @return This function will return when the transmit buffer is empty
/// @note This function is only necessary if you need to force all the data out of the transmit queue. One example would be to flush out the transmit queue before calling Modify_UART_Baudrate()
void UART_Flush_TX(UART * uart);

#ifdef __cplusplus
}
#endif

#endif /* UART_UART_H_ */