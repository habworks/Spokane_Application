/*
 * Partition_Manager.c
 *
 *  Created on: Apr 9, 2020
 *      Author: devin
 */

#include "Partition_Manager.h"
#include <string.h>

static const uint32_t Drive_Key = 0x464D4444;

void Mount_Memory_Drive(Memory_Drive * Drive)
{
	// Read the drive at address 0 to see if it hold the Drive_Key
	uint32_t key;
	Drive->Read(Drive->Flash_Handle, MEMORY_DRIVE_PARTITION_KEY_ADDRESS, (uint8_t *)&key, 4);

	if(key == Drive_Key)
	{
		Drive->partition_manager.Drive_Is_Partitioned = true;

		// Read in the number of partitions and the partition table
		Drive->Read(Drive->Flash_Handle, MEMORY_DRIVE_PARTITION_COUNT_ADDRESS, (uint8_t *)&Drive->partition_manager.Number_Of_Partitions, 4);

		int c = 0;
		for(; c < Drive->partition_manager.Number_Of_Partitions; c++)
		{
			Drive->Read(Drive->Flash_Handle, 8 + (c * sizeof(Partition_Record)), (uint8_t *)&Drive->partition_manager.Partition_Table[c], sizeof(Partition_Record));
		}
	}
	else
	{
		Drive->partition_manager.Drive_Is_Partitioned = false;
		Drive->partition_manager.Number_Of_Partitions = 0;
	}
}

void Format_Memory_Drive(Memory_Drive * Drive)
{
	Drive->partition_manager.Number_Of_Partitions = 0;

	// Write out the values to the drive to flag the drive has been formatted
	Drive->Write(Drive->Flash_Handle, MEMORY_DRIVE_PARTITION_COUNT_ADDRESS, (uint8_t *)&Drive->partition_manager.Number_Of_Partitions, 4);
	Drive->Write(Drive->Flash_Handle, MEMORY_DRIVE_PARTITION_KEY_ADDRESS, (uint8_t *)&Drive_Key, 4);

	Drive->partition_manager.Drive_Is_Partitioned = true;
}

bool Create_Partition(Memory_Drive * Drive, const char * Partition_Name, uint32_t Partition_Size, bool Encrypt_Partition)
{
	if(!Drive->partition_manager.Drive_Is_Partitioned)
		return false; 	// Drive is not formatted

	if(Drive->partition_manager.Number_Of_Partitions >= MAX_NUMBER_OF_MEMORY_DRIVE_PARTITIONS)
		return false;	// Already full of partitions

	// Determine if the partition already exists
	int c = 0;
	for(; c < Drive->partition_manager.Number_Of_Partitions; c++)
	{
		if(strcmp(Partition_Name, Drive->partition_manager.Partition_Table[c].Name) == 0)
			return false;	// Partition name found
	}

	// Create the partition
	Drive->partition_manager.Partition_Table[Drive->partition_manager.Number_Of_Partitions].Encrpytion = Encrypt_Partition;			// Save the encryption status
	strncpy(Drive->partition_manager.Partition_Table[Drive->partition_manager.Number_Of_Partitions].Name, Partition_Name, 19);		// Save the partition name

	if(Drive->partition_manager.Number_Of_Partitions == 0)
	{
		Drive->partition_manager.Partition_Table[Drive->partition_manager.Number_Of_Partitions].Start_Address = MEMORY_DRIVE_PARTITION_DATA_ADDRESS;
	}
	else
	{
		Drive->partition_manager.Partition_Table[Drive->partition_manager.Number_Of_Partitions].Start_Address = Drive->partition_manager.Partition_Table[Drive->partition_manager.Number_Of_Partitions - 1].End_Address + 1;
	}

	Drive->partition_manager.Partition_Table[Drive->partition_manager.Number_Of_Partitions].End_Address = (Drive->partition_manager.Partition_Table[Drive->partition_manager.Number_Of_Partitions].Start_Address + Partition_Size) - 1;

	// Check that we are not partitioning outside the size of the disk
	if(Drive->partition_manager.Partition_Table[Drive->partition_manager.Number_Of_Partitions].End_Address >= Drive->Drive_Size)
		return false;		// Partition overflows the drive size

	Drive->partition_manager.Number_Of_Partitions++;

	// Write out the new partition table
	Drive->Write(Drive->Flash_Handle, MEMORY_DRIVE_PARTITION_COUNT_ADDRESS, (uint8_t *)&Drive->partition_manager.Number_Of_Partitions, 4);
	Drive->Write(Drive->Flash_Handle, MEMORY_DRIVE_PARTITION_TABLE_ADDRESS, (uint8_t *)&Drive->partition_manager.Partition_Table, 464);

	return true;
}

bool Does_Partition_Exist(Memory_Drive * Drive, const char * Partition_Name)
{
	if(!Drive->partition_manager.Drive_Is_Partitioned)
		return false; 	// Drive is not formatted


	// Determine if the partition already exists
	int c = 0;
	for(; c < Drive->partition_manager.Number_Of_Partitions; c++)
	{
		if(strcmp(Partition_Name, Drive->partition_manager.Partition_Table[c].Name) == 0)
			return true;	// Partition name found
	}

	// the partition was not found in the partition table
	return false;
}

bool Memory_Drive_Write(Memory_Drive * Drive, const char * Partition_Name, uint32_t Address, uint8_t * Data, uint32_t Data_Size)
{
	// Find the start and stop address
	uint32_t start_add = 0;
	uint32_t end_add = 0;

	int c = 0;
	for(; c < Drive->partition_manager.Number_Of_Partitions; c++)
	{
		if(strcmp(Drive->partition_manager.Partition_Table[c].Name, Partition_Name) == 0)
		{
			start_add = Drive->partition_manager.Partition_Table[c].Start_Address;
			end_add = Drive->partition_manager.Partition_Table[c].End_Address;
		}
	}

	if(start_add == 0)
		return false;		// Did not find the partition


	if((start_add + Address + Data_Size - 1) > end_add)
		return false;		// Writing outside the partition space

	// Write the data
	Drive->Write(Drive->Flash_Handle, start_add + Address, Data, Data_Size);

	return true;
}

bool Memory_Drive_Read(Memory_Drive * Drive, const char * Partition_Name, uint32_t Address, uint8_t * Data, uint32_t Data_Size)
{
	// Find the start and stop address
	uint32_t start_add = 0;
	uint32_t end_add = 0;

	int c = 0;
	for(; c < Drive->partition_manager.Number_Of_Partitions; c++)
	{
		if(strcmp(Drive->partition_manager.Partition_Table[c].Name, Partition_Name) == 0)
		{
			start_add = Drive->partition_manager.Partition_Table[c].Start_Address;
			end_add = Drive->partition_manager.Partition_Table[c].End_Address;
			break;
		}
	}

	if(start_add == 0)
		return false;		// Did not find the partition


	if((start_add + Address + Data_Size - 1) > end_add)
		return false;		// reading outside the partition space

	// Read the data
	Drive->Read(Drive->Flash_Handle, start_add + Address, Data, Data_Size);

	return true;
}




/*******************************************************************************************************
* @brief Performs a check to determine if the drive has been previously formated.  The drive is formatted
* when the memory offset MEMORY_DRIVE_PARTITION_KEY_ADDRESS is loaded with the value Drive_Key
*
* @author original:	Hab Collector \n
*
* @param Drive: The NVM storage system handler
*
* @return True if previously formated
*
* STEP 1: Check for formatted drive
********************************************************************************************************/
bool Is_Drive_Formated(Memory_Drive *Drive)
{
	// STEP 1: Check for formatted drive
	uint32_t DriveFormatKeyValue;
	Drive->Read(Drive->Flash_Handle, MEMORY_DRIVE_PARTITION_KEY_ADDRESS, (uint8_t *)&DriveFormatKeyValue, sizeof(DriveFormatKeyValue));
	if (DriveFormatKeyValue == Drive_Key)
		return(true);
	else
		return(false);

} // END OF Is_Drive_Formated

