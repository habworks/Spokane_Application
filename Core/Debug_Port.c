/*
 * Debug_Port.c
 *
 *  Created on: Jan 15, 2024
 *      Author: Victoria
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "Scheduler/Scheduler.h"
#include "Debug_Port.h"
#include "MT29F2G01.h"
#include "octospi.h"
#include "spi.h"
#include "usart.h"
#include <string.h>

MT29F2G01 * MT29F2G01_1;
uint8_t aTxBuffer[BUFFERSIZE];
uint8_t aRxBuffer[BUFFERSIZE];
uint32_t ADDR;
uint32_t ReadWriteCount = 0;
MT29F2G01 *MT29F2G01_Device;

static bool readWriteCompare(void);
static void clearRxBuffer(void);
static void initTest(void);
static void incTxBuffer(void);



/**
 * @brief	Starts the state machine.
 */
void Debug_Task(void)
{
    MT29F2G01_Device = Init_MT29F2G01(&hospi1);
    Start_Task(Init_Some, NULL, 0);//start the state machiene
}

/**
 * @brief Small state machine to conduct actions in steps. Also if I dont do it this way, the system hard faults.
 */
void Init_Some(void * Task_Data)//Is a task
{
	static int state = 0;
	uint8_t Status;
	switch(state)
	{
	case 0://HAL INIT
	{
	    printf("\033[2J\033[H");
	    printf("Hello Hab\r\n");
	    initTest();
		state++;
	}
	break;

	case 1:// INIT DRIVERS
	{
	    state++;

	    // Init Device: SET FEATURES, RESET, GET FEATURES
//	    uint8_t BlockConfigValue = NONE_ALL_BLOCKS_UNLOCKED;
//		OSPI_Set_Features(&hospi1, MT29F_REG_BLOCK_LOCK, &BlockConfigValue);
//
//		OSPI_Reset(&hospi1);
//
//		while ((OSPI_Get_Features(&hospi1, MT29F_REG_STATUS) & MT29F_STATUS_MASK_OIP) != 0);


//		uint16_t Memory_ID = OSPI_Read_ID(&hospi1);
//		printf("Device ID: 0x%04X\r\n", Memory_ID);
//		if (Memory_ID != MT29F2G_ID)
//		{
//		    printf("ERROR: Incorrect Device ID\r\n");
//		    Error_Handler();
//		}
	    MT29F2G01_Init(MT29F2G01_Device);
	    printf("Device ID: 0x%04X\r\n", MT29F2G01_Device->MFG_Memory_ID);


		// Block Erase: WRITE ENABLE, BLOCK ERASE, GET FEATURES
//		OSPI_WriteEnable(&hospi1);//0x06
//		do
//		{
//		    Status = OSPI_Get_Features(&hospi1, MT29F_REG_STATUS);
//		} while( ((Status & MT29F_STATUS_MASK_OIP) != 0) || ((Status & MT29F_STATUS_MASK_WEL) != MT29F_STATUS_MASK_WEL) );
//
//		OSPI_Erase_Block(&hospi1, ADDR);
//		do
//		{
//		    Status = OSPI_Get_Features(&hospi1, MT29F_REG_STATUS);
//		} while ((Status & MT29F_STATUS_MASK_OIP) != 0);
//		if (Status & MT29F_STATUS_MASK_E_FAIL)
//		    printf("ERROR: Fail to erase block\r\n");
		MT29F2G01_BlockErase(MT29F2G01_Device, ADDR);
	}
	break;

	case 2:
	{
	    incTxBuffer();
	    state++;

	    // Write: WRITE ENABLE, PROGRAM LOAD, PROGRAM EXECUTE, GET FEATURES
//		OSPI_WriteEnable(&hospi1);
//		do
//        {
//            Status = OSPI_Get_Features(&hospi1, MT29F_REG_STATUS);
//        } while( ((Status & MT29F_STATUS_MASK_OIP) != 0) || ((Status & MT29F_STATUS_MASK_WEL) != MT29F_STATUS_MASK_WEL) );
//
//		OSPI_Program_Load(&hospi1, MT29F_CMD_PROGRAM_LOAD_X1, ADDR, aTxBuffer, BUFFERSIZE);
//
//		OSPI_Program_Execute(&hospi1, ADDR);//0x10
//
//		while ((OSPI_Get_Features(&hospi1, MT29F_REG_STATUS) & MT29F_STATUS_MASK_OIP) != 0);
	    MT29F2G01_MemoryWrite(MT29F2G01_Device, ADDR, aTxBuffer, BUFFERSIZE);
	}
	break;

	case 3:
	{
	    clearRxBuffer();
	    state++;

	    // Read: PAGE READ, GET FEATURES, READ FROM CACHE
//		OSPI_Page_Read(&hospi1, ADDR);
//
//		while ((OSPI_Get_Features(&hospi1, MT29F_REG_STATUS) & MT29F_STATUS_MASK_OIP) != 0);
//
//		OSPI_Read_Cache(&hospi1, MT29F_CMD_READ_CACHE_X1, ADDR, aRxBuffer, BUFFERSIZE);
//
//		while ((OSPI_Get_Features(&hospi1, MT29F_REG_STATUS) & MT29F_STATUS_MASK_OIP) != 0);
	    MT29F2G01_MemoryRead(MT29F2G01_Device, ADDR, aRxBuffer, BUFFERSIZE);
	}
	break;

	case 4:
	{
	    ReadWriteCount++;
	    if (readWriteCompare())
	    {
	        printf("OK Addr: %d  Count: %d\r\n", (int)ADDR, (int)ReadWriteCount);
	    }
        else
        {
            printf("ERROR Addr: %d  Count: %d\r\n", (int)ADDR, (int)ReadWriteCount);
        }

	    STATUS_LED_TOGGLE();
	    ADDR += 2048;
	    if (ADDR >= 2048 * 32) //268435456)
	    {
	        printf("End of memory reached\r\n");
	        while(1);
	    }
	    state = 2;
	}
	break;

	} // END OF SWITCH
}



void initTest(void)
{
    ReadWriteCount = 0;
    ADDR = 2048 * ReadWriteCount;
    memset(aTxBuffer, 0x00, sizeof(aTxBuffer));
}

static bool readWriteCompare(void)
{
    if (memcmp((char *)aRxBuffer, (char *)aTxBuffer, BUFFERSIZE) == 0)
        return(true);
    else
        return(false);
}

static void clearRxBuffer(void)
{
    memset(aRxBuffer, 0x00, sizeof(aRxBuffer));
}

static void incTxBuffer(void)
{
    static uint8_t FillValue = 1;
    memset(aTxBuffer, FillValue, BUFFERSIZE);
    FillValue++;
}

int _write(int file, char *ptr, int len)
{
    if (HAL_UART_Transmit_IT(&huart2, (uint8_t *)ptr, len) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_Delay(1);
    return(len);

} // END OF _write















/**
 * @brief not necessary. I just have this here in case i disable it somehow.
 */

#if(0)
void InitUart(void)
{
//	Enable_UART(UART_3);
}

/**
 * @brief Startup Text on Console screen.
 */
static void Print_Startup_Banner(void)
{
	printf("\r\n\r\n");
	printf("***Startup Filler Text***");


}

/**
 * @brief SET the GPS pin. Without it, it will not stream NMEA data.
 */
void GPS_Module_RESET(void)
{
	//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);

}

/**
 * @brief When "USB detect" is entered by user, will return if the USB is detected.
 */
void Is_USB_Connected(void)
{
	printf("STATUS-->");

	if(USB_MSD_Mounted)
	{
		printf("Detected");
	}
	else
	{
		printf("Not Detected");
	}

}

/**
 * @brief	USB Commands for console
 */
void Create_USB_CMDS (void)
{
	char command[64];
	sprintf(command, "USB");
	Add_Console_Top_Level_Command(command, "Connection Status", NULL, NULL);

	sprintf(command, "USB help");
	Add_Console_Complete_Command(command, Print_USB_Help_Message, NULL);

	sprintf(command, "USB detect");
	Add_Console_Complete_Command(command, Is_USB_Connected, NULL);

	sprintf(command, "USB mount");
	Add_Console_Complete_Command(command, Mount_Status, NULL);
}

/**
 * @brief list of commands from "USB help"
 */
static void Print_USB_Help_Message(void * Device)
{
	printf("\r\n-Available Commands-\r\n");
	printf("USB detect: This will return USB detection status.\r\n");
	printf("USB mount: This will return USB mount status.\r\n");
}

/**
 * @brief Creating the USB Drive
 */
static void Create_USB_Drive(void)
{
	USB_Flash_Drive = Create_FAT_FS_Drive(&USBHFatFS, USBHPath);
}

/**
 * @brief Mount the USB if detected but not yet Mounted OR unmount the USB if it is not detected but not mounted
 */
static void Mount_USB_FLASH(void * Task_Data)
{
	if(!Is_Mounted && USB_MSD_Mounted)
	{
		//printf("Inside");
		if(Mount_FAT_FS_Drive(USB_Flash_Drive))
		{
			//printf("FR_OK\r\n");
			Is_Mounted = true;
		}
		else
		{
			//printf("!FR_OK\r\n");
		}
	}
	if(!USB_MSD_Mounted && Is_Mounted)
	{
		//printf("Inside2");
		Unmount_FAT_FS_Drive(USB_Flash_Drive);
		Is_Mounted = false;
	}

}

/**
 * @brief Mount status is returned when "USB mount" is enterd into console.
 */
static void Mount_Status(void)
{
	printf("STATUS-->");
	if(Is_Mounted)
	{
		printf("Mounted\r\n");
	}
	else
	{
		printf("Not Mounted\r\n");
	}
}
#endif

