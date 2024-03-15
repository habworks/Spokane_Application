/*
 * Console.c
 *
 *  Created on: Oct 23, 2020
 *      Author: devink
 */

#include "Console.h"
#include "Scheduler/Scheduler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



static Console console;
static void Check_RX_Data(void * Task_Data);
static void Flush_TX_Data(void * Task_Data);
static void Repeat_Previous_Command(void * Task_Data);
static void Handle_RX_Command(void);
static void Clear_Screen(void * Data);


void Init_Console(UART * uart, void * Print_Banner_Function_Ptr)
{
	console.Bus_Handle = uart;
	console.TX_Buff_Size = 0;
	console.RX_Buff_Size = 0;
	console.Command_State = eConsole_Wait_For_Commands;

	Prep_Queue(&console.Console_Commands);

	console.RX_Task_ID = Start_Task(Check_RX_Data, NULL, 0);
	Set_Task_Name(console.RX_Task_ID, "Console RX");

	console.Flush_Task_ID = Start_Task(Flush_TX_Data, NULL, 10);
	Set_Task_Name(console.Flush_Task_ID, "Console Flush");

	console.Repeat_Commands_Task_ID = Start_Task(Repeat_Previous_Command, NULL, 1000);
	Set_Task_Name(console.Repeat_Commands_Task_ID, "Console Commands");
	Halt_Task(console.Repeat_Commands_Task_ID);

	// Add debug commands
	Add_Console_Top_Level_Command("clear", "Clear the screen.", Clear_Screen, NULL);

	// Call the print banner function pointer to display the product start up banner
	if(Print_Banner_Function_Ptr != NULL)
	{
		void (*Print_Exe)(void) = Print_Banner_Function_Ptr;
		Print_Exe();
	}

	// Print the command prompt
	printf("\r\n\r\nCommand: ");
}

bool Add_Console_Top_Level_Command(const char * Command, const char * Description, void * Call_Function, void * Call_Function_Data)
{
	Console_Command * command = (Console_Command *)Task_malloc(console.Repeat_Commands_Task_ID, sizeof(Console_Command));

	if(command != NULL)
	{
		command->Command = (const char *)Task_malloc(console.Repeat_Commands_Task_ID, (strlen(Command) + 1) * sizeof(char));

		if(command->Command != NULL)
		{
			strcpy((char *)command->Command, Command);

			command->Description = (const char *)Task_malloc(console.Repeat_Commands_Task_ID, (strlen(Description) + 1) * sizeof(char));

			if(command->Description != NULL)
			{
				strcpy((char *)command->Description, Description);

				command->Call_Function = Call_Function;
				command->Call_Function_Data = Call_Function_Data;
				command->Command_Type = eConsole_Command_Top_Level;

				Enqueue(&console.Console_Commands, (void *)command);

				return true;
			}
		}
	}

	return false;
}

bool Add_Console_Complete_Command(const char * Command, void * Call_Function, void * Call_Function_Data)
{
	Console_Command * command = (Console_Command *)Task_malloc(console.Repeat_Commands_Task_ID, sizeof(Console_Command));

	if(command != NULL)
	{
		command->Command = (const char *)Task_malloc(console.Repeat_Commands_Task_ID, (strlen(Command) + 1) * sizeof(char));

		if(command->Command != NULL)
		{
			strcpy((char *)command->Command, Command);

			command->Call_Function = Call_Function;
			command->Call_Function_Data = Call_Function_Data;
			command->Command_Type = eConsole_Complete_Command;

			Enqueue(&console.Console_Commands, (void *)command);

			return true;

		}
	}

	return false;
}

bool Add_Console_Partial_Command(const char * Command, void * Call_Function, void * Call_Function_Data)
{
	Console_Command * command = (Console_Command *)Task_malloc(console.Repeat_Commands_Task_ID, sizeof(Console_Command));

	if(command != NULL)
	{
		command->Command = (const char *)Task_malloc(console.Repeat_Commands_Task_ID, (strlen(Command) + 1) * sizeof(char));

		if(command->Command != NULL)
		{
			strcpy((char *)command->Command, Command);

			command->Call_Function_With_Data = Call_Function;
			command->Call_Function_Data = Call_Function_Data;
			command->Command_Type = eConsole_Partial_Command;

			Enqueue(&console.Console_Commands, (void *)command);

			return true;

		}
	}

	return false;
}

bool Add_Console_Repeat_Command(const char * Command, void * Call_Function, void * Call_Function_Data, uint32_t Repeat_Timer)
{
	Console_Command * command = (Console_Command *)Task_malloc(console.Repeat_Commands_Task_ID, sizeof(Console_Command));

	if(command != NULL)
	{
		command->Command = (const char *)Task_malloc(console.Repeat_Commands_Task_ID, (strlen(Command) + 1) * sizeof(char));

		if(command->Command != NULL)
		{
			strcpy((char *)command->Command, Command);

			command->Call_Function = Call_Function;
			command->Call_Function_Data = Call_Function_Data;
			command->Repeat_Time = Repeat_Timer;
			command->Command_Type = eConsole_Repeat_Command;

			Enqueue(&console.Console_Commands, (void *)command);

			return true;

		}
	}

	return false;
}

bool Add_Console_Debug_Command(const char * Command, void * Start_Call_Function, void * Stop_Call_Function, void * Call_Function_Data)
{
	Console_Command * command = (Console_Command *)Task_malloc(console.Repeat_Commands_Task_ID, sizeof(Console_Command));

	if(command != NULL)
	{
		command->Command = (const char *)Task_malloc(console.Repeat_Commands_Task_ID, (strlen(Command) + 1) * sizeof(char));

		if(command->Command != NULL)
		{
			strcpy((char *)command->Command, Command);

			command->Call_Function = Start_Call_Function;
			command->Debug_Stop_Call_Function = Stop_Call_Function;
			command->Call_Function_Data = Call_Function_Data;
			command->Command_Type = eConsole_Debug_Command;

			Enqueue(&console.Console_Commands, (void *)command);

			return true;

		}
	}

	return false;
}


#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{

	// Save the byte
	console.TX_Buff[console.TX_Buff_Size++] = ch;

	// If the byte is a \n then flush the buffer
	if(ch == '\r')
	{
		UART_Transmit(console.Bus_Handle, console.TX_Buff, console.TX_Buff_Size);
		console.TX_Buff_Size = 0;
	}

	// If the buffer is full then flush the buffer
	if(console.TX_Buff_Size >= MAX_CONSOLE_BUFF_SIZE)
	{
		UART_Transmit(console.Bus_Handle, console.TX_Buff, console.TX_Buff_Size);
		console.TX_Buff_Size = 0;
	}

	return ch;
}


static void Check_RX_Data(void * Task_Data)
{
	uint8_t data[UART_RX_BUFF_SIZE];
	uint16_t data_size;
	UART_Receive(console.Bus_Handle, data, &data_size);

	uint16_t c = 0;
	for(; c < data_size; c++)
	{
		if(console.Command_State == eConsole_Wait_For_Commands)
		{
			if(data[c] == '\b' || data[c] == 0x7F)		// Backspace or delete
			{
				if(console.RX_Buff_Size > 0)
				{
					uint8_t backspace_buff[3] = {'\b', ' ', '\b'};
					UART_Transmit(console.Bus_Handle, backspace_buff, 3);
					console.RX_Buff_Size--;
				}
			}
			else
			{
				console.RX_Buff[console.RX_Buff_Size++] = data[c];

				// check for terminating character
				if(data[c] == '\r')
				{
					// Convert to string
					console.RX_Buff[console.RX_Buff_Size- 1] = 0;
					Handle_RX_Command();
					console.RX_Buff_Size = 0;
				}
				else
				{
					// Echo the data we got back
					UART_Transmit(console.Bus_Handle, &data[c], 1);
				}

				// Check if we need to clear the buffer
				if(console.RX_Buff_Size >= MAX_CONSOLE_BUFF_SIZE)
				{
					console.RX_Buff_Size = 0;
				}
			}
		}
		else
		{
			// Wait for a \n to halt the repeat command streaming
			if(data[c] == '\r')
			{
				// Call the debug stop function if we are in a debugging mode
				if(console.Command_State == eConsole_Servicing_Debug_Command)
				{
					if(console.Previous_Command->Debug_Stop_Call_Function != NULL)
						console.Previous_Command->Debug_Stop_Call_Function(console.Previous_Command->Call_Function_Data);
				}

				Halt_Task(console.Repeat_Commands_Task_ID);
				console.Command_State = eConsole_Wait_For_Commands;

				// Clear the screen
				printf("\033[2J");
				printf("%c[2j%c[H",27,27);
				printf("Command: ");
			}
		}
	}
}

static void Flush_TX_Data(void * Task_Data)
{
		fflush(stdout);
		if(console.TX_Buff_Size > 0)
		{
			UART_Transmit(console.Bus_Handle, console.TX_Buff, console.TX_Buff_Size);
			console.TX_Buff_Size = 0;
		}
}

static void Repeat_Previous_Command(void * Task_Data)
{
	printf("\033[2J");
	printf("%c[2j%c[H",27,27);

	console.Previous_Command->Call_Function(console.Previous_Command->Call_Function_Data);
}

static void Handle_RX_Command(void)
{
	if(strcmp("help", (const char *)console.RX_Buff) == 0)
	{
		printf("\r\n");
		// Display all top level commands
		for(int c = 0; c < console.Console_Commands.Size; c++)
		{
			Console_Command * command = (Console_Command *)Queue_Peek(&console.Console_Commands, c);

			if(command->Command_Type == eConsole_Command_Top_Level)
			{
				printf("\r\n%s: %s", command->Command, command->Description);
			}
		}

		printf("\r\n\r\nCommand: ");
	}
	else
	{
		// Loop through all of the commands looking for what was entered
		for(int c = 0; c < console.Console_Commands.Size; c++)
		{
			Console_Command * command = (Console_Command *)Queue_Peek(&console.Console_Commands, c);

			switch(command->Command_Type)
			{
			case eConsole_Command_Top_Level:
			case eConsole_Complete_Command:
			{
				if(strcmp((const char *)console.RX_Buff, command->Command) == 0)
				{
					// Call the callback function if there is one
					if(command->Call_Function != NULL)
					{
						printf("\r\n\r\n");
						command->Call_Function(command->Call_Function_Data);
					}

					printf("\r\n\r\nCommand: ");
					return;
				}
			}
				break;
			case eConsole_Partial_Command:
			{
				if(strstr((const char *)console.RX_Buff, command->Command))
				{
					if(command->Call_Function_With_Data != NULL)
					{
						printf("\r\n\r\n");
						command->Call_Function_With_Data((const char *)console.RX_Buff, command->Call_Function_Data);
					}

					printf("\r\n\r\nCommand: ");
					return;
				}
			}
				break;
			case eConsole_Repeat_Command:
			{
				if(strcmp((const char *)console.RX_Buff, command->Command) == 0)
				{
					if(command->Call_Function != NULL)
					{
						// Call the function once and then repeat calling the function at the desired frequency
						// Clear the screen
						printf("\033[2J");
						printf("%c[2j%c[H",27,27);

						command->Call_Function(command->Call_Function_Data);

						console.Command_State = eConsole_Servicing_Repeat_Command;
						console.Previous_Command = command;
						Modify_Task_Timeout(console.Repeat_Commands_Task_ID, command->Repeat_Time);
						Resume_Task(console.Repeat_Commands_Task_ID);
					}
					else
						printf("\r\nCommand: ");

					return;
				}
			}
				break;
			case eConsole_Debug_Command:
			{
				if(strcmp((const char *)console.RX_Buff, command->Command) == 0)
				{
					if(command->Call_Function != NULL)
					{
						// Call the function once and then repeat calling the function at the desired frequency
						// Clear the screen
						printf("\033[2J");
						printf("%c[2j%c[H",27,27);

						command->Call_Function(command->Call_Function_Data);
						console.Previous_Command = command;
						console.Command_State = eConsole_Servicing_Debug_Command;
					}
					else
						printf("\r\nCommand: ");

					return;
				}
			}
				break;
			}
		}

		// If we get here then we did not find a match to the inputed command
		printf("\r\nCommand: ");
	}
}

static void Clear_Screen(void * Data)
{
	printf("\033[2J");
	printf("%c[2j%c[H",27,27);
}



