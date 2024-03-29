/*
 * MT29F2G01.c
 *
 *  Created on: Feb 14, 2024
 *      Author: Victoria
 */


/*
 * MT25QL256.c
 *
 *  Created on: Apr 26, 2021
 *      Author: devink
 */

#include "MT29F2G01.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



/******************************************************************************************************
* @brief Reset of the MT29F device.  Places the memory in a known state and can also be used to abort
* various command sequences in progress: READ, PROGRAM, and ERASE.
*
* @author original: Victoria modified by Hab Collector \n
*
* @note: The reset command does not change the state of set features.  Set features can only be changed
* by a subsequent command of set features
*
* @param hospi: OctoSpi Handler
*
* STEP 1: Load command struct
* STEP 2: Execute command
* *****************************************************************************************************/
void OSPI_Reset(OSPI_HandleTypeDef *hospi)
{
    // STEP 1: Load command struct
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction        = MT29F_CMD_RESET;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
    sCommand.DataMode           = HAL_OSPI_DATA_NONE;
    sCommand.NbData             = 0;
    sCommand.DummyCycles        = 0;

    // STEP 2: Execute command
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

} // END OF OSPI_Reset



/******************************************************************************************************
* @brief This command is used to read the device status.
*
* @author original: Victoria modified by Hab Collector \n
*
* @note: Auto polling may substitute for this
*
* @param hospi: OctoSpi Handler
*
* @return Status register value
*
* STEP 1: Load command struct
* STEP 2: Execute command and get the response
* *****************************************************************************************************/
uint8_t OSPI_Get_Features(OSPI_HandleTypeDef *hospi, uint8_t FeatureAddress)
{
    uint8_t StatusRegisterValue;

    // STEP 1: Load command struct
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.Instruction        = MT29F_CMD_GET_FEATURES;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.Address            = FeatureAddress;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_8_BITS;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
    sCommand.NbData             = 1;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

    // STEP 2: Execute command and get the response
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_OSPI_Receive(hospi, &StatusRegisterValue, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }
    return(StatusRegisterValue);

} // END OF OSPI_Get_Features



/******************************************************************************************************
* @brief Enable write to the device.  The write enable is required for: Page Program, OTP Area Program
* and Block Erase
*
* @author original: Victoria modified by Hab Collector \n
*
* @param hospi: OctoSpi Handler
*
* STEP 1: Load command struct
* STEP 2: Execute command
* *****************************************************************************************************/
void OSPI_WriteEnable(OSPI_HandleTypeDef *hospi)
{
    // STEP 1: Load command struct
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.Instruction        = MT29F_CMD_WRITE_ENABLE;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode           = HAL_OSPI_DATA_NONE;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

    // STEP 2: Execute command
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

} // END OF OSPI_WriteEnable



/******************************************************************************************************
* @brief Erase block command is used to erase at the block level.  The blocks are organized as:
* – Page size x1: 2176 bytes (2048 + 128 bytes)
* – Block size: 64 pages (128K + 8K bytes)
* – Plane size: 1Gb (1024 blocks per plane)
*
* @author original: Victoria modified by Hab Collector \n
*
* @note: Typical erase time is 2ms
*
* @note: The follow sequence is necessary to erase a block: Write Enable, Block Erase, Get Features
*
* @param hospi: OctoSpi Handler
* @param BlockAddress: Address of the block to be erased.  Address must resolve to a block
*
* STEP 1: Load command struct
* STEP 2: Execute command
* *****************************************************************************************************/
void OSPI_Erase_Block(OSPI_HandleTypeDef *hospi, uint32_t BlockAddress)
{
    // STEP 1: Load command struct
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction        = MT29F_CMD_BLOCK_ERASE;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.Address            = BlockAddress;
    sCommand.DataMode           = HAL_OSPI_DATA_NONE;
    sCommand.DummyCycles        = 0;

    // STEP 2: Execute command
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

} // END OF OSPI_Erase_Block



/******************************************************************************************************
* @brief This command is used to alter the device configuration.  Once set, a reset command will not clear
* the configuration.  Only another set feature command can alter the configuraiton.
*
* @author original: Victoria modified by Hab Collector \n
*
* @param hospi: OctoSpi Handler
*
* @return Status register value
*
* STEP 1: Load command struct
* STEP 2: Execute command
* STEP 3: Transmit the configuration
* *****************************************************************************************************/
void OSPI_Set_Features(OSPI_HandleTypeDef *hospi, uint8_t FeatureAddress, uint8_t *ConfigValue)
{
    // STEP 1: Load command struct
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.Instruction        = MT29F_CMD_SET_FEATURES;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.Address            = FeatureAddress;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_8_BITS;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.NbData             = 1;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

    // STEP 2: Execute command
    if(HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
          Error_Handler();
    }

    // STEP 3: Transmit the configuration
    if(HAL_OSPI_Transmit(hospi, ConfigValue ,HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
     {
          Error_Handler();
     }

} // END OF OSPI_Set_Features



/******************************************************************************************************
* @brief Used to changed the contents of memory.  This command must be precede by a write enable followed
* by a Program execute.
*
* @author original: Victoria modified by Hab Collector \n
*
* @param hospi: OctoSpi Handler
* @param ProgramCommandType: Type of data input 1 bit or nibble
* @param Address: Address of memory to be programmed
* @param DataBuffer: Buffer of data to be programmed to memory
* @param DataBufferLength: Length of data
*
* @return void
*
* STEP 1: Load command struct
* STEP 2: Execute command
* STEP 3: Transmit the configuration
* *****************************************************************************************************/
void OSPI_Program_Load(OSPI_HandleTypeDef *hospi, uint8_t ProgramCommandType, uint32_t Address, uint8_t *DataBuffer, uint32_t DataBufferLength)
{
    // STEP 1: Load command struct
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction        = ProgramCommandType; //0x02;//PROGRAM_LOAD_X4;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.Address            = Address;
    sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
    sCommand.NbData             = DataBufferLength;
    sCommand.DummyCycles        = 0;

    // STEP 2: Execute command
    if(HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    // STEP 3: Transmit the configuration
    if(HAL_OSPI_Transmit(hospi, DataBuffer,HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

} // END OF OSPI_Program_Load



/******************************************************************************************************
* @brief Used to changed the contents of memory.  This command must be precede by a program load followed
* by a get features.
*
* @author original: Victoria modified by Hab Collector \n
*
* @param hospi: OctoSpi Handler
* @param Address: Address of memory to be programmed
*
* @return void
*
* STEP 1: Load command struct
* STEP 2: Execute command
* *****************************************************************************************************/
void OSPI_Program_Execute(OSPI_HandleTypeDef *hospi, uint32_t Address)
{
    // STEP 1: Load command struct
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction        = MT29F_CMD_PROGRAM_EXECUTE;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.Address            = Address;
    sCommand.DataMode           = HAL_OSPI_DATA_NONE;
    sCommand.NbData             = 0;
    sCommand.DummyCycles        = 0;

    // STEP 2: Execute command
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

} // END OF OSPI_Program_Execute



/******************************************************************************************************
* @brief The PAGE READ (13h) command transfers data from the NAND Flash array to the cache register. It
* requires a 24-bit address consisting of 7 dummy bits and a 17-bit block/page address (8 dummy bits followed
* by an 16-bit block/page address for 1Gb).
*
* @author original: Victoria modified by Hab Collector \n
*
* @param hospi: OctoSpi Handler
* @param Address: Address of memory to be programmed
*
* @return void
*
* STEP 1: Load command struct
* STEP 2: Execute command
* *****************************************************************************************************/
void OSPI_Page_Read(OSPI_HandleTypeDef *hospi, uint32_t Address)
{
    // STEP 1: Load command struct
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction        = MT29F_CMD_PAGE_READ;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.Address            = Address;
    sCommand.DataMode           = HAL_OSPI_DATA_NONE;
    sCommand.NbData             = 0;
    sCommand.DummyCycles        = 0;

    // STEP 2: Execute command
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

} // END OF OSPI_Page_Read



/******************************************************************************************************
* @brief Used to changed the contents of memory.  This command must be precede by a write enable followed
* by a Program execute.
*
* @author original: Victoria modified by Hab Collector \n
*
* @param hospi: OctoSpi Handler
* @param ReadCachCommandType: Type of data cache input 1 bit or nibble
* @param Address: Address of memory to be programmed
* @param DataBuffer: Buffer of where to place read data
* @param DataBufferLength: Length of data to be read
*
* @return void
*
* STEP 1: Load command struct
* STEP 2: Execute command
* STEP 3: Transmit the configuration
* *****************************************************************************************************/
void OSPI_Read_Cache(OSPI_HandleTypeDef *hospi, uint8_t ReadCachCommandType, uint32_t Address, uint8_t *DataBuffer, uint32_t DataBufferLength)
{
    // STEP 1: Load command struct
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction        = ReadCachCommandType;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.Address            = Address;
    sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
    sCommand.NbData             = DataBufferLength;
    sCommand.DummyCycles        = 8;

    // STEP 2: Execute command
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    // STEP 3: Transmit the configuration
    if (HAL_OSPI_Receive(hospi, DataBuffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

} // END OF OSPI_Read_Cache



/******************************************************************************************************
* @brief Read ID returns a two byte value that represents the Mfg ID and the Device ID.
* MFG ID:       0x2C (Micron) Byte 0
* Device ID:    0x24 2Gb 3.3V Byte 1
*
* @author original: Victoria modified by Hab Collector \n
*
* @param hospi: OctoSpi Handler
*
* @return Memory ID as a uint16_t
*
* STEP 1: Load command struct
* STEP 2: Execute command
* STEP 3: Transmit the configuration
* STEP 4: Return the result
* *****************************************************************************************************/
uint16_t OSPI_Read_ID(OSPI_HandleTypeDef *hospi)
{
    uint16_t Device_ID;
    uint8_t Memory_ID[2];

    // STEP 1: Load command struct
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_16_BITS;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
    sCommand.Instruction        = MT29F_CMD_READ_ID;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
    sCommand.Address            = 0x00;
    sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;//HAL_OSPI_DATA_4_LINES;
    sCommand.NbData             = 2;
    sCommand.DummyCycles        = 8;    // 1 Dummy byte;

    // STEP 2: Execute command
    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
    Error_Handler();
    }

    // STEP 3: Transmit the configuration
    if (HAL_OSPI_Receive(hospi, Memory_ID, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
    Error_Handler();
    }

    // STEP 4: Return the result
    Device_ID = Memory_ID[1];
    Device_ID <<= 8;
    Device_ID |= Memory_ID[0];
    return(Device_ID);

} // END OF OSPI_Read_ID






MT29F2G01 * Init_MT29F2G01(OSPI_HandleTypeDef *Bus_Handle)
{
	MT29F2G01 * device = (MT29F2G01 *)malloc(sizeof(MT29F2G01));

	device->Bus_Handle = Bus_Handle;

	return device;
}



bool MT29F2G01_Init(MT29F2G01 *Device)
{
    uint8_t BlockConfigValue = NONE_ALL_BLOCKS_UNLOCKED;
    OSPI_Set_Features(Device->Bus_Handle, MT29F_REG_BLOCK_LOCK, &BlockConfigValue);

    OSPI_Reset(Device->Bus_Handle);

    while ((OSPI_Get_Features(Device->Bus_Handle, MT29F_REG_STATUS) & MT29F_STATUS_MASK_OIP) != 0);

    Device->MFG_Memory_ID = OSPI_Read_ID(Device->Bus_Handle);

    return(true);
}

bool MT29F2G01_BlockErase(MT29F2G01 *Device, uint32_t Address)
{
    uint8_t Status;
    OSPI_WriteEnable(Device->Bus_Handle);
    do
    {
        Status = OSPI_Get_Features(Device->Bus_Handle, MT29F_REG_STATUS);
    } while( ((Status & MT29F_STATUS_MASK_OIP) != 0) || ((Status & MT29F_STATUS_MASK_WEL) != MT29F_STATUS_MASK_WEL) );

    OSPI_Erase_Block(Device->Bus_Handle, Address);
    do
    {
        Status = OSPI_Get_Features(Device->Bus_Handle, MT29F_REG_STATUS);
    } while ((Status & MT29F_STATUS_MASK_OIP) != 0);
    if (Status & MT29F_STATUS_MASK_E_FAIL)
        return(false);
    else
        return(true);

}

bool MT29F2G01_MemoryWrite(MT29F2G01* Device, uint32_t Address, uint8_t *TxBuffer, uint32_t TxBufferLength)
{
    OSPI_WriteEnable(Device->Bus_Handle);

    uint8_t Status;
    do
    {
        Status = OSPI_Get_Features(Device->Bus_Handle, MT29F_REG_STATUS);
    } while( ((Status & MT29F_STATUS_MASK_OIP) != 0) || ((Status & MT29F_STATUS_MASK_WEL) != MT29F_STATUS_MASK_WEL) );

    OSPI_Program_Load(Device->Bus_Handle, MT29F_CMD_PROGRAM_LOAD_X1, Address, TxBuffer, TxBufferLength);

    OSPI_Program_Execute(Device->Bus_Handle, Address);//0x10

    while ((OSPI_Get_Features(Device->Bus_Handle, MT29F_REG_STATUS) & MT29F_STATUS_MASK_OIP) != 0);

    return(true);
}

bool MT29F2G01_MemoryRead(MT29F2G01* Device, uint32_t Address, uint8_t *RxBuffer, uint32_t RxBufferLength)
{
    OSPI_Page_Read(Device->Bus_Handle, Address);

    while ((OSPI_Get_Features(Device->Bus_Handle, MT29F_REG_STATUS) & MT29F_STATUS_MASK_OIP) != 0);

    OSPI_Read_Cache(Device->Bus_Handle, MT29F_CMD_READ_CACHE_X1, Address, RxBuffer, RxBufferLength);

    while ((OSPI_Get_Features(Device->Bus_Handle, MT29F_REG_STATUS) & MT29F_STATUS_MASK_OIP) != 0);

    return(true);
}












#if(0)
void OSPI_WriteDisable(OSPI_HandleTypeDef *hospi)
{
  OSPI_RegularCmdTypeDef  sCommand;

  uint8_t reg[2];

  /* Enable write operations ------------------------------------------ */

 sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.Instruction        = 0x04;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles        = 0;
  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;


  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }



  /* Configure automatic polling mode to wait for write enabling ---- */
  sCommand.Instruction    = MT29F_CMD_GET_FEATURES;
  sCommand.Address        = 0xC0;
  sCommand.AddressMode    = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.AddressSize    = HAL_OSPI_ADDRESS_8_BITS;
  sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
  sCommand.DataMode       = HAL_OSPI_DATA_1_LINE;
  sCommand.DataDtrMode    = HAL_OSPI_DATA_DTR_DISABLE;
  sCommand.NbData         = 1;
  sCommand.DummyCycles    = 0;//DUMMY_CLOCK_CYCLES_READ_REG;

  //CS_LOW();
  do//NOt sure if to wait here or not
  {

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }

    if (HAL_OSPI_Receive(hospi, reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
      Error_Handler();
    }
  } while((reg[0] & 0x02) != 0x00);//when matches x02, we know write enable is ready


}
#endif


























