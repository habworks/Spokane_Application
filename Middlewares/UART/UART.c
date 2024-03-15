/*
 * UART.c
 *
 *  Created on: Apr 10, 2020
 *      Author: devink
 */

#include "UART.h"
#include "Scheduler/Scheduler.h"
#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// We need to keep track of all UARTs to handle the callbacks
static bool UART_Callbacks_Initialized = false;
static Queue UART_Callback_Handles;
static void UART_Tasks(void * Task_Data);


UART * Init_DMA_UART(UART_HandleTypeDef * UART_Handle)
{
	if(!UART_Callbacks_Initialized)
	{
		Prep_Queue(&UART_Callback_Handles);
		UART_Callbacks_Initialized = true;
	}


	UART * uart = (UART *)malloc(sizeof(UART));

	if(uart)
	{
		uart->UART_Handle = UART_Handle;
		uart->Use_DMA = true;
		uart->RX_Buff_Tail_Ptr = 0;
		Prep_Queue(&uart->TX_Queue);
		uart->TX_Buffer = NULL;
		uart->Currently_Transmitting = false;
		uart->UART_Is_Enabled = true;

		// Insert the UART into the queue so that we can handle callbacks
		Enqueue(&UART_Callback_Handles, (void *)uart);

		uart->Task_ID = Start_Task(UART_Tasks, (void *)uart, 0);
		Set_Task_Name(uart->Task_ID, "UART RX/TX");
		Task_Add_Heap_Size(uart->Task_ID, uart);

		HAL_UART_Receive_DMA(UART_Handle, uart->RX_Buffer, UART_RX_BUFF_SIZE);
	}
	else
	{
		printf("UART init malloc error\r\n");
	}

	return uart;
}

void Enable_UART(UART * Bus)
{
	HAL_UART_MspInit(Bus->UART_Handle);
	Prep_Queue(&Bus->TX_Queue);
	Bus->TX_Buffer = NULL;
	Bus->Currently_Transmitting = false;
	Bus->UART_Is_Enabled = true;

	HAL_UART_Receive_DMA(Bus->UART_Handle, Bus->RX_Buffer, UART_RX_BUFF_SIZE);
}

void Disable_UART(UART * Bus)
{
	HAL_UART_MspDeInit(Bus->UART_Handle);
	Bus->UART_Is_Enabled = false;
}

UART * Init_SUDO_UART(int32_t (* Transmit_Func_PTR)(uint8_t *, uint16_t), int32_t (* Receive_Func_PTR)(uint8_t *, uint16_t *))
{
	UART * uart = (UART *)malloc(sizeof(UART));

	if(uart)
	{
		uart->UART_Handle = NULL;
		uart->SUDO_Transmit = Transmit_Func_PTR;
		uart->SUDO_Receive = Receive_Func_PTR;
		uart->RX_Buff_Head_Ptr = 0;
		uart->RX_Buff_Tail_Ptr = 0;
		uart->UART_Is_Enabled = true;
	}

	return uart;
}

int32_t UART_Transmit(UART * uart, uint8_t * Data, uint16_t Data_Size)
{
	// Only transmit if the UART is enabled
	if(!uart->UART_Is_Enabled)
		return 0;

	if(Data_Size > 2000)
		return 0;

	if(uart->UART_Handle == NULL)
	{
		// This could be a SUDO UART, check if there is a transmit function pointer
		if(uart->SUDO_Transmit != NULL)
		{
			// Send the data to the function pointer
			int32_t ret = uart->SUDO_Transmit(Data, Data_Size);
			return ret;
		}
		else
		{
			// There is no where to send the data, just return
			return -1;
		}
	}
	else
	{
		UART_TX_Node * new_node = (UART_TX_Node *)Task_malloc(uart->Task_ID, sizeof(UART_TX_Node));
		if(new_node != NULL)
		{
			new_node->Data = (uint8_t *)Task_malloc(uart->Task_ID, Data_Size * sizeof(uint8_t));
			if(new_node->Data != NULL)
			{
				memcpy(new_node->Data, Data, Data_Size);
				new_node->Data_Size = Data_Size;

				Enqueue(&uart->TX_Queue, (void *)new_node);

				return Data_Size;
			}
			else
			{
				Task_free(uart->Task_ID, new_node);
				return -1;
			}
		}
		else
			return -1;
	}
}

int32_t UART_Receive(UART * uart, uint8_t * Data, uint16_t * Data_Size)
{
	*Data_Size = 0;

	// Only transmit if the UART is enabled
	if(!uart->UART_Is_Enabled)
		return 0;

	if(uart->UART_Handle == NULL)
	{
		// This could be a SUDO UART, check if there is a receive function pointer
		if(uart->SUDO_Receive != NULL)
		{
			// Send the data to the function pointer
			int32_t ret = uart->SUDO_Receive(Data, Data_Size);
			return ret;
		}
		else
		{
			// There is no where to receive the data, just return
			return -1;
		}
	}
	else
	{
		if(uart->UART_Handle->RxState == HAL_UART_STATE_BUSY_RX)
		{
			while(uart->RX_Buff_Tail_Ptr != (UART_RX_BUFF_SIZE - uart->UART_Handle->hdmarx->Instance->CNDTR))
			{
				Data[(*Data_Size)++] = uart->RX_Buffer[uart->RX_Buff_Tail_Ptr++];

				if(uart->RX_Buff_Tail_Ptr >= UART_RX_BUFF_SIZE)
					uart->RX_Buff_Tail_Ptr = 0;
			}

			return (int32_t)*Data_Size;
		}

		return -1;
	}
}

void Modify_UART_Baudrate(UART * uart, uint32_t New_Baudrate)
{
	// Only transmit if the UART is enabled
	if(!uart->UART_Is_Enabled)
		return;

	// Flush any messages queued to go out
	UART_Flush_TX(uart);

	// Stop the receiver DMA
	HAL_UART_DMAStop(uart->UART_Handle);
	uart->RX_Buff_Tail_Ptr = 0;

	// Change the UART init baudrate data field
	uart->UART_Handle->Init.BaudRate = New_Baudrate;

	// Init the UART
	HAL_UART_Init(uart->UART_Handle);

	// restart the receiver DMA
	HAL_UART_Receive_DMA(uart->UART_Handle, uart->RX_Buffer, UART_RX_BUFF_SIZE);
}

void UART_Flush_TX(UART * uart)
{
	// Only transmit if the UART is enabled
	if(!uart->UART_Is_Enabled)
		return;

	while(uart->TX_Queue.Size != 0 || uart->Currently_Transmitting)
	{
		UART_Tasks((void *)uart);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	// Find who the callback is for
	int c = 0;
	for(; c < UART_Callback_Handles.Size; c++)
	{
		UART * uart = (UART *)Queue_Peek(&UART_Callback_Handles, c);

		if(uart->UART_Handle == huart)
		{
			uart->Currently_Transmitting = false;
			return;
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	// Find who the callback is for
	int c = 0;
	for(; c < UART_Callback_Handles.Size; c++)
	{
		UART * uart = (UART *)Queue_Peek(&UART_Callback_Handles, c);

		if(uart->UART_Handle == huart)
		{
			uart->RX_Buff_Tail_Ptr = 0;
			uart->Currently_Transmitting = false;
			HAL_DMA_Abort_IT(uart->UART_Handle->hdmarx);
			HAL_UART_DMAStop(uart->UART_Handle);
			HAL_UART_Receive_DMA(uart->UART_Handle, uart->RX_Buffer, UART_RX_BUFF_SIZE);
		}
	}
}

static void UART_Tasks(void * Task_Data)
{
	UART * uart = (UART *)Task_Data;

	if(uart->TX_Queue.Size > 0 && !uart->Currently_Transmitting && uart->UART_Is_Enabled)
	{
		if(uart->TX_Buffer != NULL)
		{
			// Clean up previous transmission
			Task_free(uart->Task_ID, uart->TX_Buffer->Data);
			Task_free(uart->Task_ID, uart->TX_Buffer);
		}

		uart->TX_Buffer = (UART_TX_Node *)Dequeue(&uart->TX_Queue);

		if(uart->Use_DMA)
			HAL_UART_Transmit_DMA(uart->UART_Handle, uart->TX_Buffer->Data, uart->TX_Buffer->Data_Size);
		//else
			//TODO: Handle interrupt based transmit complete callbacks

		uart->Currently_Transmitting = true;
	}
}
