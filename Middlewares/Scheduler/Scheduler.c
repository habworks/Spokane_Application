/*
 * Scheduler.c
 *
 *  Created on: Apr 13, 2020
 *      Author: devink
 */

#include "Scheduler.h"
#ifdef USE_CONSOLE
#include "Console/Console.h"
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "main.h"
#include <stdbool.h>

static uint32_t Task_ID = 0xFFFFFFFF;

static tScheduler Scheduler;
static void Scheduler_Tasks(void * task_data);

#ifdef USE_CONSOLE
static void Display_Console_Help(void * Data);
static void Display_Processes(void * Data);
static void Halt_Process(uint8_t * Console_Buff, void * Data);
static void Resume_Process(uint8_t * Console_Buff, void * Data);
#endif

void Start_Scheduler(void)
{
	Prep_Queue(&Scheduler.Tasks);
	Scheduler.Next_TID = 1;

	Task_ID = Start_Task(Scheduler_Tasks,NULL,1000);
	Set_Task_Name(Task_ID,"Scheduler");
	tTask * t = (tTask *)Queue_Peek(&Scheduler.Tasks,Task_ID - 1);
	Task_Add_Heap_Size(Task_ID, t);
}

static void Scheduler_Tasks(void * task_data)
{
#ifdef USE_CONSOLE
	static bool Console_Commands_Added = false;
	if(!Console_Commands_Added)
	{
		Add_Console_Top_Level_Command("proc", "Display and modify Scheduler processes", Display_Processes, NULL);
		Add_Console_Complete_Command("proc help", Display_Console_Help, NULL);
		Add_Console_Repeat_Command("proc over", Display_Processes, NULL, 1000);
		Add_Console_Partial_Command("proc halt ", Halt_Process, NULL);
		Add_Console_Partial_Command("proc resume ", Resume_Process, NULL);
		Console_Commands_Added = true;
	}
#endif

	tTask * task;
	uint32_t counter = 0;
	for(; counter < Scheduler.Tasks.Size; counter++)
	{
		task = (tTask *)Queue_Peek(&Scheduler.Tasks,counter);

		float percent = (float)task->Task_Running_Time / 1000.0;
		task->Task_Running_Percentage = percent * 100;

		task->Task_Running_Time = 0;
	}
}

void Run_Scheduler_Tasks(void)
{
	if(Scheduler.Tasks.Size > 0)
	{
		// Loop through all of the tasks
		tTask * task;
		uint32_t counter = 0;
		for(; counter < Scheduler.Tasks.Size; counter++)
		{
			task = (tTask *)Queue_Peek(&Scheduler.Tasks,counter);

			// Test if it is time to process the task
			if((HAL_GetTick() - task->Start_Time) > task->Timeout)
			{
				// Execute the task
				if(!task->Task_Halted)
				{
					uint32_t start = HAL_GetTick();
					task->Task_Exe(task->Task_Data);
					uint32_t stop = HAL_GetTick();
					task->Task_Running_Time += (stop - start);
				}

				task->Start_Time = HAL_GetTick();
			}
		}
	}
}

uint32_t Start_Task(void * Task_CallBack, void * Data, uint32_t timeout)
{
	tTask * task = (tTask *)malloc(sizeof(tTask));

	if(Task_ID != 0xFFFFFFFF)
		Task_Add_Heap_Size(Task_ID, task);

	if(task != NULL)
	{
		task->TID = Scheduler.Next_TID;
		Scheduler.Next_TID++;

		task->Task_Halted = 0;
		task->Start_Time = HAL_GetTick();
		task->Timeout = timeout;
		task->Task_Running_Time = 0;
		strcpy((char *)task->Task_Name,"               ");
		task->Task_Exe = Task_CallBack;
		task->Task_Data = Data;
		task->Task_Heap_Usage = 0;

		if(Enqueue(&Scheduler.Tasks,(void *)task))
			return task->TID;
		else
			return 0;
	}
	else
	{
		printf("Task start malloc error\r\n");
		return 0;
	}
}

void Delete_Task(void)
{

}

void Resume_Task(uint32_t task)
{
	// Loop through all of the tasks
	tTask * t;
	uint32_t counter = 0;
	for(; counter < Scheduler.Tasks.Size; counter++)
	{
		t = (tTask *)Queue_Peek(&Scheduler.Tasks,counter);

		if(t->TID == task)
		{
			t->Task_Halted = 0;

			return;
		}
	}
}

void Halt_Task(uint32_t task)
{
	// Loop through all of the tasks
	tTask * t;
	uint32_t counter = 0;
	for(; counter < Scheduler.Tasks.Size; counter++)
	{
		t = (tTask *)Queue_Peek(&Scheduler.Tasks,counter);

		if(t->TID == task)
		{
			t->Task_Halted = 1;

			return;
		}
	}
}

void Modify_Task_Timeout(uint32_t task, uint32_t timeout)
{
	// Loop through all of the tasks
	tTask * t;
	uint32_t t_sub;
	uint32_t counter = 0;
	for(; counter < Scheduler.Tasks.Size; counter++)
	{
		t = (tTask *)Queue_Peek(&Scheduler.Tasks,counter);
		t_sub = t->TID;
		//if(t->TID == task)
		if(t_sub == task)
		{
			t->Timeout = timeout;
			t->Start_Time = HAL_GetTick();
			counter = 0;
			return;
		}
	}
	counter = 0;
}

void Set_Task_Name(uint32_t task, const char * name)
{
	// Loop through all of the tasks
	tTask * t;
	uint32_t counter = 0;
	for(; counter < Scheduler.Tasks.Size; counter++)
	{
		t = (tTask *)Queue_Peek(&Scheduler.Tasks,counter);

		//if(t->TID == task)
			if(7 == task)
		{
			strncpy((char *)t->Task_Name,name,15);
			t->Task_Name[15] = '\0';

			return;
		}
	}
}

tScheduler * Get_Scheduler_Tasks(void)
{
	return &Scheduler;
}

void * Task_malloc(uint32_t task, size_t size)
{
	void * ptr = malloc(size);

	if(ptr != NULL)
	{
		tTask * t;
		for(int counter = 0; counter < Scheduler.Tasks.Size; counter++)
		{
			t = (tTask *)Queue_Peek(&Scheduler.Tasks,counter);

			if(t->TID == task)
			{
				t->Task_Heap_Usage += malloc_usable_size(ptr);
				break;
			}
		}
	}

	return ptr;
}

void Task_free(uint32_t task, void * data_ptr)
{

	// Save the data size
	size_t data_size = malloc_usable_size(data_ptr);
	free(data_ptr);

	tTask * t;
	for(int counter = 0; counter < Scheduler.Tasks.Size; counter++)
	{
		t = (tTask *)Queue_Peek(&Scheduler.Tasks,counter);

		if(t->TID == task)
		{
			t->Task_Heap_Usage -= data_size;
			return;
		}
	}
}

void Task_Add_Heap_Size(uint32_t task, void * data_ptr)
{
	tTask * t;
	for(int counter = 0; counter < Scheduler.Tasks.Size; counter++)
	{
		t = (tTask *)Queue_Peek(&Scheduler.Tasks,counter);

		if(t->TID == task)
		{
			t->Task_Heap_Usage += malloc_usable_size(data_ptr);
			break;
		}
	}
}

#ifdef USE_CONSOLE

static void Display_Console_Help(void * Data)
{
	printf("\t\t- Available Commands -\r\n\r\n");
	printf("proc: \t\t\tDisplay all the Scheduler process information.\r\n");
	printf("proc over: \t\tContinuously display all the Scheduler process information.\r\n");
	printf("proc halt [PID]: \tHalt a running process.\r\n");
	printf("proc resume [PID]: \tResume a halted process.\r\n");
	printf("proc help: \t\tDisplay this help message.\r\n");
}

static void Display_Processes(void * Data)
{
	tScheduler * Scheduler = Get_Scheduler_Tasks();

	printf("\r\nPID\tName\t\tState\t%%(CPU)\tMem(Bytes)\r\n");

	tTask * task;
	uint32_t counter = 0;
	int total_cpu = 0;
	int total_mem = 0;
	for(; counter < Scheduler->Tasks.Size; counter++)
	{
		task = (tTask *)Queue_Peek(&Scheduler->Tasks,counter);

		char state = 'R';
		if(task->Task_Halted)
		state = 'H';

		//printf("%i\t%s\t  %c\t%i\t%i\r\n",(int)task->TID,task->Task_Name,state,(int)task->Task_Running_Percentage, (int)task->Task_Heap_Usage);
		printf("%i\t%s\t  %c",(int)task->TID,task->Task_Name,state);

		if(task->Task_Running_Percentage > 0)
		{
			total_cpu += (int)task->Task_Running_Percentage;
			printf("\t%i", (int)task->Task_Running_Percentage);
		}
		else
			printf("\t-");

		if(task->Task_Heap_Usage > 0)
		{
			total_mem += (int)task->Task_Heap_Usage;
			printf("\t%i\r\n", (int)task->Task_Heap_Usage);
		}
		else
			printf("\t-\r\n");
	}

	printf("---------------------------------------------------\r\n");
	printf("\t\t\t\t%i\t%i\r\n", total_cpu, total_mem);
}

static void Halt_Process(uint8_t * Console_Buff, void * Data)
{
	int process_id;
	sscanf((char *)Console_Buff, "proc halt %d", &process_id);

	Halt_Task((uint32_t)process_id);
	printf("Process %d Halted\r\n", process_id);
}

static void Resume_Process(uint8_t * Console_Buff, void * Data)
{
	int process_id;
	sscanf((char *)Console_Buff, "proc resume %d", &process_id);

	Resume_Task((uint32_t)process_id);
	printf("Process %d Resumed\r\n", process_id);
}

#endif
