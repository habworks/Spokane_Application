/*
 * MT29F2G01.h
 *
 *  Created on: Feb 14, 2024
 *      Author: Victoria
 */

#ifndef SPI_FLASH_MT29F2G01_MT29F2G01_H_
#define SPI_FLASH_MT29F2G01_MT29F2G01_H_

#include "main.h"
#include "Partition_Manager/Partition_Manager.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFERSIZE    245

typedef struct
{
	OSPI_HandleTypeDef * Bus_Handle;
	//uint16_t Device_Address;

	uint8_t Working_Buff[4096];				// All work is done in 4K blocks, this buffer is working room.
}MT29F2G01;

MT29F2G01 * Init_MT29F2G01(OSPI_HandleTypeDef * bus_handle);
void OSPI_WriteEnable(OSPI_HandleTypeDef *hospi);
void OSPI_WriteDisable(OSPI_HandleTypeDef *hospi);
void OSPI_Erase_Block(OSPI_HandleTypeDef *hospi);
void OSPI_Get_Features(OSPI_HandleTypeDef *hospi);
void OSPI_Program_Load(OSPI_HandleTypeDef *hospi);
void OSPI_Program_Execute(OSPI_HandleTypeDef *hospi);
void OSPI_Page_Read(OSPI_HandleTypeDef *hospi);
void OSPI_Read_Cache_X4(OSPI_HandleTypeDef *hospi);
void OSPI_Read_ID(OSPI_HandleTypeDef *hospi);
void OSPI_Reset(OSPI_HandleTypeDef *hospi);
void OSPI_Set_Features(OSPI_HandleTypeDef *hospi);
void UselessFunct(void);
bool readWriteCompare(void);
void prepareForCompare(void);
void initTest(void);

#endif /* SPI_FLASH_MT29F2G01_MT29F2G01_H_ */
