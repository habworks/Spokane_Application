/*
 * MT29F2G01.h
 *
 *  Created on: Feb 14, 2024
 *      Author: Victoria
 */

#ifndef SPI_FLASH_MT29F2G01_MT29F2G01_H_
#define SPI_FLASH_MT29F2G01_MT29F2G01_H_

#ifdef __cplusplus
extern"C" {
#endif

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// DEFINES
#define BUFFERSIZE    245
#define MT29F2G_ID                        ((uint16_t)(0x242C))
//#define MT29F2G01_FLASH_SIZE            0x00200000// 2GB
//#define MT29F2G01_PAGE_SIZE             0x800//1 page is 2k
//#define MT29F2G01_BLOCK_SIZE            0x022000//0x20000//128k
//#define ADDR          0x7FE0000//0x87BC000//0x2A8000//0x154000//0x000000//0x154000 //0x022000//0x20000//128k
// MT29F COMMANDS
#define MT29F_CMD_RESET                     0xFF
#define MT29F_CMD_WRITE_ENABLE              0x06
#define MT29F_CMD_GET_FEATURES              0x0F
#define MT29F_CMD_SET_FEATURES              0x1F
#define MT29F_CMD_PAGE_READ                 0x13
#define MT29F_CMD_PROGRAM_EXECUTE           0x10
#define MT29F_CMD_BLOCK_ERASE               0xD8
#define MT29F_CMD_PROGRAM_LOAD_X1           0x02
#define MT29F_CMD_PROGRAM_LOAD_X4           0x32
#define MT29F_CMD_READ_CACHE_X1             0x0B
#define MT29F_CMD_READ_CACHE_X4             0x6B
#define MT29F_CMD_READ_ID                   0X9F
// MT29F REGISTER FEATURE ADDRESS
#define MT29F_REG_BLOCK_LOCK                0xA0
#define MT29F_REG_CONFIG                    0xB0
#define MT29F_REG_STATUS                    0xC0
#define MT29F_REG_DIE_SELECT                0xD0
// MT29F BLOCK LOCK VALUES: Relates to TB, BP3, BP2, BP1, BP0 bits only - BRWD AND WP must be ORed with this value
#define NONE_ALL_BLOCKS_UNLOCKED            0x00
#define UPPER_1024_TO_2047_LOCKED           0x50
#define UPPER_0000_TO_1023_LOCKED           0x54
// MT29F STATUS REGISTER MASK
#define MT29F_STATUS_MASK_OIP               ((uint8_t)(0x01 << 0))
#define MT29F_STATUS_MASK_WEL               ((uint8_t)(0x01 << 1))
#define MT29F_STATUS_MASK_E_FAIL            ((uint8_t)(0x01 << 2))
#define MT29F_STATUS_MASK_P_FAIL            ((uint8_t)(0x01 << 3))
#define MT29F_STATUS_MASK_ECCSO             ((uint8_t)(0x01 << 4))
#define MT29F_STATUS_MASK_ECCS1             ((uint8_t)(0x01 << 5))
#define MT29F_STATUS_MASK_ECCS2             ((uint8_t)(0x01 << 6))
#define MT29F_STATUS_MASK_CRBSY             ((uint8_t)(0x01 << 7))


//#define WRITE_DISABLE                 0x04//
//#define BLOCK_ERASE                   0xD8
//#define READ_ID                       0x9F

//#define READ_CACHE_QUAD                 0x6B
////define PAGE_PROGRAM                   0x9F
//#define PROGRAM_LOAD_X4                 0x32

//#define READ_CACHE_X4                   0x6B

//#define DUMMY_CLOCK_CYCLES_READ_QUAD         10
//#define DUMMY_CLOCK_CYCLES_READ_REG             4
//#define DUMMY_CLOCK_CYCLES_READ     6



// TYPEDEFS AND ENUMS
typedef struct
{
	OSPI_HandleTypeDef *    Bus_Handle;
	uint16_t                MFG_Memory_ID;
//	uint8_t                 Working_Buff[4096];				// All work is done in 4K blocks, this buffer is working room.
} MT29F2G01;


// FUNCTION PROTOTYPES
void OSPI_Reset(OSPI_HandleTypeDef *hospi);
uint8_t OSPI_Get_Features(OSPI_HandleTypeDef *hospi, uint8_t FeatureAddress);
void OSPI_WriteEnable(OSPI_HandleTypeDef *hospi);
void OSPI_Erase_Block(OSPI_HandleTypeDef *hospi, uint32_t BlockAddress);
void OSPI_Set_Features(OSPI_HandleTypeDef *hospi, uint8_t FeatureAddress, uint8_t *ConfigValue);
void OSPI_Program_Load(OSPI_HandleTypeDef *hospi, uint8_t ProgramCommandType, uint32_t Address, uint8_t *DataBuffer, uint32_t DataLength);
void OSPI_Program_Execute(OSPI_HandleTypeDef *hospi, uint32_t Address);
void OSPI_Page_Read(OSPI_HandleTypeDef *hospi, uint32_t Address);
void OSPI_Read_Cache(OSPI_HandleTypeDef *hospi, uint8_t ReadCachCommandType, uint32_t Address, uint8_t *DataBuffer, uint32_t DataBufferLength);
uint16_t OSPI_Read_ID(OSPI_HandleTypeDef *hospi);


bool MT29F2G01_Init(MT29F2G01 *Device);


MT29F2G01 * Init_MT29F2G01(OSPI_HandleTypeDef *Bus_Handle);
bool MT29F2G01_BlockErase(MT29F2G01 *Device, uint32_t Address);
bool MT29F2G01_MemoryWrite(MT29F2G01* Device, uint32_t Address, uint8_t *TxBuffer, uint32_t TxBufferLength);
bool MT29F2G01_MemoryRead(MT29F2G01* Device, uint32_t Address, uint8_t *RxBuffer, uint32_t RxBufferLength);





#ifdef __cplusplus
}
#endif
#endif /* SPI_FLASH_MT29F2G01_MT29F2G01_H_ */
