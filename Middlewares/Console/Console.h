/*
 * Console.h
 *
 *  Created on: Oct 23, 2020
 *      Author: devink
 */

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include "UART/UART.h"
#include "Queue/Queue.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_CONSOLE_BUFF_SIZE		256

typedef enum
{
	eConsole_Wait_For_Commands = 0,
	eConsole_Servicing_Repeat_Command,
	eConsole_Servicing_Debug_Command
}Console_Command_States;

typedef enum
{
	eConsole_Command_Top_Level = 0,
	eConsole_Complete_Command,
	eConsole_Partial_Command,
	eConsole_Repeat_Command,
	eConsole_Debug_Command
}Console_Command_Types;

typedef struct
{
	Console_Command_Types Command_Type;
	const char * Command;
	const char * Description;
	void (*Call_Function)(void *);
	void (*Call_Function_With_Data)(const char *, void *);
	void (*Debug_Stop_Call_Function)(void *);
	void * Call_Function_Data;
	uint32_t Repeat_Time;
}Console_Command;

typedef struct
{
	UART * Bus_Handle;
	uint8_t TX_Buff[MAX_CONSOLE_BUFF_SIZE];
	uint32_t TX_Buff_Size;
	uint8_t RX_Buff[MAX_CONSOLE_BUFF_SIZE];
	uint32_t RX_Buff_Size;
	uint32_t RX_Task_ID;
	uint32_t Flush_Task_ID;

	Console_Command_States Command_State;
	Queue Console_Commands;
	uint32_t Repeat_Commands_Task_ID;
	Console_Command * Previous_Command;
}Console;

void Init_Console(UART * uart, void * Print_Banner_Function_Ptr);

/* Functions to add menu items to the console port */
bool Add_Console_Top_Level_Command(const char * Command, const char * Description, void * Call_Function, void * Call_Function_Data);
bool Add_Console_Complete_Command(const char * Command, void * Call_Function, void * Call_Function_Data);
bool Add_Console_Partial_Command(const char * Command, void * Call_Function, void * Call_Function_Data);
bool Add_Console_Repeat_Command(const char * Command, void * Call_Function, void * Call_Function_Data, uint32_t Repeat_Timer);
bool Add_Console_Debug_Command(const char * Command, void * Start_Call_Function, void * Stop_Call_Function, void * Call_Function_Data);

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_CONSOLE_H_ */

