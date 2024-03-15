/*
 * Partition_Manager.h
 *
 *  Created on: Apr 9, 2020
 *      Author: devin
 */

#ifndef PARTITION_MANAGER_PARTITION_MANAGER_H_
#define PARTITION_MANAGER_PARTITION_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NUMBER_OF_MEMORY_DRIVE_PARTITIONS	16
#define MEMORY_DRIVE_PARTITION_KEY_ADDRESS		0
#define MEMORY_DRIVE_PARTITION_COUNT_ADDRESS	4
#define MEMORY_DRIVE_PARTITION_TABLE_ADDRESS	8
#define MEMORY_DRIVE_PARTITION_DATA_ADDRESS		472

typedef struct __attribute__((__packed__))
{
	char Name[20];
	uint32_t Start_Address;
	uint32_t End_Address;
	uint8_t Encrpytion;
}Partition_Record;

typedef struct
{
	bool Drive_Is_Partitioned;
	uint32_t Number_Of_Partitions;
	Partition_Record Partition_Table[MAX_NUMBER_OF_MEMORY_DRIVE_PARTITIONS];
}Partition_Manager;

typedef struct
{
	uint32_t Drive_Size;
	void (*Write)(void * Flash, uint32_t Address, uint8_t * Transmit_Data, uint16_t Transmit_Data_Size);
	void (*Read)(void * Flash, uint32_t Address, uint8_t * Received_Data, uint16_t Received_Data_Size);
	void * Flash_Handle;

	Partition_Manager partition_manager;
}Memory_Drive;


void Mount_Memory_Drive(Memory_Drive * Drive);
void Format_Memory_Drive(Memory_Drive * Drive);
bool Create_Partition(Memory_Drive * Drive, const char * Partition_Name, uint32_t Partition_Size, bool Encrypt_Partition);
bool Does_Partition_Exist(Memory_Drive * Drive, const char * Partition_Name);
bool Memory_Drive_Write(Memory_Drive * Drive, const char * Partition_Name, uint32_t Address, uint8_t * Data, uint32_t Data_Size);
bool Memory_Drive_Read(Memory_Drive * Drive, const char * Partition_Name, uint32_t Address, uint8_t * Data, uint32_t Data_Size);
bool Is_Drive_Formated(Memory_Drive *Drive);

#ifdef __cplusplus
}
#endif

#endif /* PARTITION_MANAGER_PARTITION_MANAGER_H_ */
