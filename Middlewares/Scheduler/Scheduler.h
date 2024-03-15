/*
 * Scheduler.h
 *
 *  Created on: Apr 13, 2020
 *      Author: devink
 */

#ifndef MIDDLEWARES_SCHEDULER_SCHEDULER_H_
#define MIDDLEWARES_SCHEDULER_SCHEDULER_H_

#include "Queue/Queue.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint32_t TID;				// Task ID
	uint32_t Start_Time;		// Tick of the last execution of the task
	uint32_t Timeout;			// Timeout in milliseconds of the next time to run the task
	uint8_t Task_Halted;		// Boolean if the task should be run the next time the timeout has occurred
	uint32_t Task_Running_Time;	// This is the time in milliseconds the task takes over a one second period
	uint8_t Task_Running_Percentage;
	uint8_t Task_Name[16];		// Allow a 15 character name with a terminating \0
	void (*Task_Exe)(void *);
	void * Task_Data;			// Data to be passed to the Task_Exe when it is ran
	uint32_t Task_Heap_Usage;	// Tracks how much data has been malloced to the task
}tTask;

typedef struct
{
	Queue Tasks;
	uint32_t Next_TID;
}tScheduler;

void Start_Scheduler(void);
void Run_Scheduler_Tasks(void);

uint32_t Start_Task(void * Task_CallBack,void * Data,uint32_t timeout);		// Task_Callback is called when timeout has occurred, timeout is how often to call Task_CallBack in milliseconds
void Delete_Task(void);
void Resume_Task(uint32_t task);
void Halt_Task(uint32_t task);
void Modify_Task_Timeout(uint32_t task, uint32_t timeout);
void Set_Task_Name(uint32_t task, const char * name);
tScheduler * Get_Scheduler_Tasks(void);

void * Task_malloc(uint32_t task, size_t size);
void Task_free(uint32_t task, void * data_ptr);
void Task_Add_Heap_Size(uint32_t task, void * data_ptr);						// This function is used when space is allocated before a task is created, call this function after the task is created to track the previously allocated memory

#ifdef __cplusplus
}
#endif

#endif /* MIDDLEWARES_SCHEDULER_SCHEDULER_H_ */
