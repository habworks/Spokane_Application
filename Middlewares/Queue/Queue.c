/*
 * Queue.c
 *
 *  Created on: Apr 10, 2020
 *      Author: devink
 */

#include "Queue.h"
#include <stdlib.h>
#include <stdio.h>

void Prep_Queue(Queue * que)
{
	que->Head = NULL;
	que->Tail = NULL;
	que->Size = 0;
}

uint8_t Enqueue(Queue * que, void * data)
{
	Node * node = (Node *)malloc(sizeof(Node));

	if(node != NULL)
	{
		node->Next = NULL;
		node->Data = data;

		if(que->Size == 0)
		{
			que->Head = node;
			que->Tail = node;
		}
		else
		{
			que->Tail->Next = (Node *)node;
			que->Tail = node;
		}

		que->Size++;
		return 1;
	}
	else
	{
		printf("Enqueue malloc error\r\n");
		return 0;
	}
}

void * Dequeue(Queue * que)
{
	if(que->Size > 0)
	{
		void * return_value = que->Head->Data;

		if(que->Size == 1)
		{
			free(que->Head);
			que->Head = NULL;
			que->Tail = NULL;
		}
		else
		{
			Node * temp = que->Head;
			que->Head = (Node *)que->Head->Next;
			free(temp);
		}

		que->Size--;

		return return_value;
	}
	else
		return NULL;
}

void * Queue_Peek(Queue * que, uint32_t index)
{
	if(index < que->Size)
	{
		Node * travler = que->Head;
		uint32_t counter = 0;
		for(; counter < index; counter++)
			travler = (Node *)travler->Next;

		return travler->Data;
	}
	else
		return NULL;
}

