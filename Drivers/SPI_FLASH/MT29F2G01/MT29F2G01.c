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
uint8_t aTxBuffer[2048];
uint8_t aRxBuffer[2048];

uint8_t IDBuffer[2];

uint32_t ADDR = 0x00000000;
uint32_t ReadWriteCount = 0;


void UselessFunct(void);
void CS_HIGH(void);
void CS_LOW(void);


/******************************************************************************************************
* @brief Reset of the MT29F device.  Places the memory in a known state and can also be used to abort
* various command sequences in progress: READ, PROGRAM, and ERASE.
*
* @author original: Victoria modified by Hab Collector \n
*
* @param hospi: OctoSpi Handler
*
* STEP 1: Load and execute command
* *****************************************************************************************************/
void OSPI_Reset(OSPI_HandleTypeDef *hospi)
{
    // STEP 1: Load and execute command
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
    // Execute command
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
* @note: Auto polling may subsitute for this
*
* @param hospi: OctoSpi Handler
*
* STEP 1: Load and execute command and receive response
* *****************************************************************************************************/
uint8_t OSPI_Get_Features(OSPI_HandleTypeDef *hospi)
{
    uint8_t reg;

    // STEP 1: Load and execute command and receive response
    OSPI_RegularCmdTypeDef  sCommand;
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.Instruction        = MT29F_CMD_GET_FEATURES;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.Address            = MT29F_REG_STATUS;
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
//    do
//    {
        // Execute command
        if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            Error_Handler();
        }
        // Receive Response
        if (HAL_OSPI_Receive(hospi, &reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            Error_Handler();
        }
//    } while((reg[0] & MEMORY_READY_MASK_VALUE) != MEMORY_READY_MATCH_VALUE);//while not x00
        return(reg);

} // END OF OSPI_Get_Features



MT29F2G01 * Init_MT29F2G01(OSPI_HandleTypeDef * bus_handle)
{
	MT29F2G01 * device = (MT29F2G01 *)malloc(sizeof(MT29F2G01));

	device->Bus_Handle = bus_handle;

	return device;
}



void OSPI_WriteEnable(OSPI_HandleTypeDef *hospi)
{
  OSPI_RegularCmdTypeDef  sCommand;

  uint8_t reg[2];

  /* Enable write operations ------------------------------------------ */

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
  } while((reg[0] & WRITE_ENABLE_MASK_VALUE) != WRITE_ENABLE_MATCH_VALUE);//when matches x02, we know write enable is ready


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


void OSPI_Erase_Block(OSPI_HandleTypeDef *hospi)//address needed
{
	  OSPI_RegularCmdTypeDef  sCommand;

	  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
	  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
	  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
	  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
	  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	  sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
	  //sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
	  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
	  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
	  sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
	  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

	  sCommand.Instruction =  0xD8;
	  sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
	  sCommand.Address     = ADDR;//address;
	  sCommand.DataMode    = HAL_OSPI_DATA_NONE;
	  sCommand.DummyCycles = 0;


	  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
      {
        Error_Handler();

      }


}



void OSPI_Set_Features(OSPI_HandleTypeDef *hospi)
{
	  OSPI_RegularCmdTypeDef  sCommand;
	  uint8_t reg = 0x00;//2 bytes

	   sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
	   sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
	   sCommand.Instruction        = 0x1F;
	   sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
	   sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
	   sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	   sCommand.Address            = 0xA0;
	   sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
	   sCommand.AddressSize        = HAL_OSPI_ADDRESS_8_BITS;
	   sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
	   sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
	   sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
	   sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
	   sCommand.NbData             = 1; //1 byte
	   sCommand.DummyCycles        = 0;//DUMMY_CLOCK_CYCLES_READ_REG;
	   sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
	   sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

	   if(HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	   {
	   	      Error_Handler();
	   	}


	   if(HAL_OSPI_Transmit(hospi, &reg ,HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	   	 {
	   	      Error_Handler();
	   	 }

}

void OSPI_Program_Load(OSPI_HandleTypeDef *hospi)//address needed
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

	 sCommand.Instruction = 0x02;//PROGRAM_LOAD_X4;
	 sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
	 sCommand.Address     = ADDR;//0x00;
	 sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;//HAL_OSPI_DATA_4_LINES;
	 sCommand.NbData      = sizeof(aTxBuffer); // Hab edit was: BUFFERSIZE;
	 sCommand.DummyCycles = 0;

	 if(HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	 {
	      Error_Handler();
	 }

	 if(HAL_OSPI_Transmit(hospi, aTxBuffer,HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	 {
	      Error_Handler();
	 }
}

void OSPI_Program_Execute(OSPI_HandleTypeDef *hospi)//address needed
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

		 sCommand.Instruction = MT29F_CMD_PROGRAM_EXECUTE;
		 sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
		 sCommand.Address     = ADDR;//0x00;
		 sCommand.DataMode    = HAL_OSPI_DATA_NONE;
		 sCommand.NbData      = 1;
		 sCommand.DummyCycles = 0;

		  if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		  {
		      Error_Handler();

		  }
}

void OSPI_Page_Read(OSPI_HandleTypeDef *hospi)//address needed
{
	 OSPI_RegularCmdTypeDef  sCommand;

	 	sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
	 	sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
	 	sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
	 	sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
	 	sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
	 	sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
		//sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
		sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
		sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
		sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
		sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
		sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

		sCommand.Instruction = MT29F_CMD_PAGE_READ;
		sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;
		sCommand.Address     = ADDR;//0x00;
		sCommand.DataMode    = HAL_OSPI_DATA_NONE;
		sCommand.NbData      = 0;
		sCommand.DummyCycles = 0;

		 if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			 Error_Handler();

		 }
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



void UselessFunct(void)
{

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
    if (strncmp((char *)aRxBuffer, (char *)aTxBuffer, sizeof(aTxBuffer)) == 0)
        return(true);
    else
        return(false);
}


void initTest(void)
{
    ReadWriteCount = 65530;
    ADDR = 2048 * ReadWriteCount;
    uint8_t Value = 0;
    memset(aTxBuffer, 0x00, sizeof(aTxBuffer));
    for (uint16_t Index = 0; Index < (sizeof(aTxBuffer) - 8); Index++)
    {
        aTxBuffer[Index] = Value;
        Value++;
    }

    prepareForCompare();
}

void prepareForCompare(void)
{
    memset(aRxBuffer, 0x00, sizeof(aRxBuffer));
}


