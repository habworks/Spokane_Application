/*
 * Queue.h
 *
 *  Created on: Apr 10, 2020
 *      Author: devink
 */

#ifndef MIDDLEWARES_QUEUE_QUEUE_H_
#define MIDDLEWARES_QUEUE_QUEUE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	void * Data;
	void * Next;
}Node;

typedef struct
{
	Node * Head;
	Node * Tail;
	uint32_t Size;
}Queue;

void Prep_Queue(Queue * que);
uint8_t Enqueue(Queue * que, void * data);
void * Dequeue(Queue * que);
void * Queue_Peek(Queue * que, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif /* MIDDLEWARES_QUEUE_QUEUE_H_ */
