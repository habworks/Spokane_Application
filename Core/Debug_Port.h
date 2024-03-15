/*
 * Debug_Port.h
 *
 *  Created on: Jan 15, 2024
 *      Author: Victoria
 */

#ifndef DEBUG_PORT_H_
#define DEBUG_PORT_H_
#//include "UART/UART.h"

//void InitDebugPort(void);
void Debug_Task(void);
void Init_Some(void * Task_Data);
//uint8_t RDR_Data[UART_RX_BUFF_SIZE];

//------------------------GPS-----------------------------
#define STATIC_STORAGE			__attribute__((section(".static_storage")))
extern uint8_t print_radar_status;


#define STATUS_LED_TOGGLE()         HAL_GPIO_TogglePin(nLED_GPIO_Port, nLED_Pin)


#endif /* DEBUG_PORT_H_ */
