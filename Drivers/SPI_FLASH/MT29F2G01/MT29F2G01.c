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


//uint8_t aTxBuffer[] = "!***OSPI communication based on DMA****  ****OSPI communication based on DMA****  ****OSPI communication based on DMA****  ****OSPI communication based on DMA****  ****OSPI communication based on DMA****  ****OSPI communication based on DMA*END";
//uint8_t aRxBuffer[BUFFERSIZE];
uint8_t aTxBuffer[BUFFERSIZE];
uint8_t aRxBuffer[BUFFERSIZE];

uint8_t IDBuffer[2];

uint32_t ADDR = 0x00000000;
uint32_t ReadWriteCount = 0;




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
* @param Address: Adresss of memory to be programmed
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
* @param Address: Adresss of memory to be programmed
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
    sCommand.NbData             = 0; //1
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
* @param Address: Adresss of memory to be programmed
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






MT29F2G01 * Init_MT29F2G01(OSPI_HandleTypeDef * bus_handle)
{
	MT29F2G01 * device = (MT29F2G01 *)malloc(sizeof(MT29F2G01));

	device->Bus_Handle = bus_handle;

	return device;
}





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










void OSPI_Read_Cache_X4(OSPI_HandleTypeDef *hospi)//address needed
{
	 OSPI_RegularCmdTypeDef  sCommand;

	  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
	  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
	  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
	  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
	  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	  sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
	 // sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
	  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
	  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
	  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
	  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

      sCommand.Instruction = 0x0B;//READ_CACHE_X4;
      sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
      sCommand.Address     = ADDR;//0x00;
      sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;//HAL_OSPI_DATA_4_LINES;
      sCommand.NbData      = BUFFERSIZE+1;
      sCommand.DummyCycles = 8;//DUMMY_CLOCK_CYCLES_READ;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        Error_Handler();
      }

      if (HAL_OSPI_Receive(hospi, aRxBuffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        Error_Handler();
      }
//      printf("%s",aRxBuffer);


}

void OSPI_Read_ID(OSPI_HandleTypeDef *hospi)
{
	 OSPI_RegularCmdTypeDef  sCommand;
	 uint8_t reg[2];

	 	 sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
		  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
		  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
		  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
		  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
		//  sCommand.AddressSize        = HAL_OSPI_ADDRESS_16_BITS;
		  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
		  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
		  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
		  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
		  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

	      sCommand.Instruction = 0x9F;//READ_CACHE_X4;
	      sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
	      //sCommand.Address     = ADDR;
	      sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;//HAL_OSPI_DATA_4_LINES;
	      sCommand.NbData      = 3;
	      sCommand.DummyCycles = 0;//DUMFERSIZMY_CLOCK_CYCLES_READ;



		 if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		 {
		 	  Error_Handler();
		 }

		 if (HAL_OSPI_Receive(hospi, reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		 {
		 	  Error_Handler();
		 }
		 printf("%d",reg[1]);
}



void CS_HIGH(void)
{
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);
}

void CS_LOW(void)
{
    HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
}


bool readWriteCompare(void)
{
    if (memcmp((char *)aRxBuffer, (char *)aTxBuffer, BUFFERSIZE) == 0)
        return(true);
    else
        return(false);
}


void initTest(void)
{
    ReadWriteCount = 0;
    ADDR = 2048 * ReadWriteCount;
    uint8_t Value = 0;
    memset(aTxBuffer, 0x00, sizeof(aTxBuffer));
    for (uint16_t Index = 0; Index < (sizeof(aTxBuffer) - 8); Index++)
    {
        aTxBuffer[Index] = Value;
        Value++;
    }

}

void clearRxBuffer(void)
{
    memset(aRxBuffer, 0x00, sizeof(aRxBuffer));
}


void incTxBuffer(void)
{
    static uint8_t FillValue = 1;
    memset(aTxBuffer, FillValue, BUFFERSIZE);
    FillValue++;
}
