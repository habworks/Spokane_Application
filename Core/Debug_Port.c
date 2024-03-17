/*
 * Debug_Port.c
 *
 *  Created on: Jan 15, 2024
 *      Author: Victoria
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
//#include "Console/Console.h"
#include "Scheduler/Scheduler.h"
//#include "usart.h"
//#include "Console_Router.h"
//#include "Debug_Port.h"
//#include "UART/UART.h"
//#include "LG77LICMD/LG77LICMD.h"
#include "Debug_Port.h"
//#include "ff.h"
//#include "UART/UART.h"
//#include "FAT_FS.h"
//#include "fatfs.h"
//#include "ACI_I2C.h"
//#include "i2c.h"
//#include "Partition_Manager.h"
//#include "MAG.h"
//#include "Radar_LOG_Parser.h"
//#include "Nimbelink_QBG96.h"
//#include "Radar_Check.h"
//#include "Nimble_Check.h"
#include "MT29F2G01.h"
#include "octospi.h"
//#include "SPI/ACI_SPI.h"
//#include "SPI/ACI_OSPI.h"
#include "spi.h"
#include "usart.h"
#include <string.h>

//void InitDebugPort(void);
//void InitGPS(void);
//static void Print_Startup_Banner(void);
//static void Print_USB_Help_Message(void * Device);
//static void Mount_USB_FLASH(void * Task_Data);
//static void Mount_Status(void);
//void Create_USB_CMDS (void);
//void InitUart(void);
//void GPS_Module_RESET(void);
//void Is_USB_Connected(void);
//static void Create_USB_Drive(void);
//extern uint8_t USB_MSD_Mounted;
//FatFS_Drive * 					USB_Flash_Drive;
//static bool Is_Mounted = false;

//UART * UART_2;
//UART * UART_4;
//UART * UART_3;
//GPIO *GPS_EN;
//I2C * I2C_1;
//I2C * I2C_2;


MT29F2G01 * MT29F2G01_1;
extern uint32_t ADDR;
extern uint32_t ReadWriteCount;



/**
 * @brief	Starts the state machine.
 */
void Debug_Task(void)
{
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
		state ++;
	}
	break;

	case 1:// INIT DRIVERS
	{
	    uint8_t BlockConfigValue = NONE_ALL_BLOCKS_UNLOCKED;
		OSPI_Set_Features(&hospi1, MT29F_REG_BLOCK_LOCK, &BlockConfigValue);
		OSPI_Reset(&hospi1);//0xFF
//		OSPI_Set_Features(&hospi1);//0x1F
		while ((OSPI_Get_Features(&hospi1, MT29F_REG_STATUS) & MT29F_STATUS_MASK_OIP) != 0);

		OSPI_WriteEnable(&hospi1);//0x06
		do
		{
		    Status = OSPI_Get_Features(&hospi1, MT29F_REG_STATUS);
		} while( ((Status & MT29F_STATUS_MASK_OIP) != 0) || ((Status & MT29F_STATUS_MASK_WEL) != MT29F_STATUS_MASK_WEL) );




		OSPI_Erase_Block(&hospi1, ADDR);
		do
		{
		    Status = OSPI_Get_Features(&hospi1, MT29F_REG_STATUS);
		} while ((Status & MT29F_STATUS_MASK_OIP) != 0);
		if (Status & MT29F_STATUS_MASK_E_FAIL)
		    printf("ERROR: Fail to erase block\r\n");

		state ++;
	}
	break;

	case 2:
	{
		// Write
		OSPI_WriteEnable(&hospi1);//0x06
		OSPI_Get_Features(&hospi1, MT29F_REG_STATUS);//0x0f
		OSPI_Program_Load(&hospi1);//0x02
		OSPI_Program_Execute(&hospi1);//0x10
		OSPI_Get_Features(&hospi1, MT29F_REG_STATUS);//0x0f
		state ++;
	}
	break;

	case 3:
	{
		// Read
		OSPI_Page_Read(&hospi1);//0x13
		OSPI_Get_Features(&hospi1, MT29F_REG_STATUS);//0x0f
		OSPI_Read_Cache_X4(&hospi1);//0x0B
		state ++;
		break;
	}

	case 4:
	{
	    ReadWriteCount++;
	    if (readWriteCompare())
	        printf("OK Addr: %d  Count: %d\r\n", (int)ADDR, (int)ReadWriteCount);
        else
            printf("ERROR Addr: %d\r\n", (int)ADDR);

	    prepareForCompare();
	    STATUS_LED_TOGGLE();
	    ADDR += 2048;
	    if (ADDR >= 268435456)
	    {
	        printf("End of memory reached\r\n");
	        while(1);
	    }
	    state = 2;
	}
	break;

	} // END OF SWITCH
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

