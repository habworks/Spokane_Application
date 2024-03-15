/*
 * FAT_FS.h
 *
 *  Created on: Dec 28, 2020
 *      Author: devin
 */

#ifndef FAT_FS_FAT_FS_H_
#define FAT_FS_FAT_FS_H_

#include "ff.h"
#include <stdbool.h>
#include <stdint.h>

// DEFINES
#define FILE_TRANSFER_BUFFER_SIZE	256
#define LOGICAL_DRIVE_MICRO_SD		"0:"
#define LOGICAL_DRIVE_USB_FLASH		"1:"
#define MEGA_BYTE					1048576U	// 2^20
#define MAX_FILE_NAME_LENGTH        256


// TYPE DEFES AND ENUMS
typedef struct
{
	FATFS * FatFS;
	char * Path;
	bool Mounted;
	TCHAR Drive_Label[32];
	uint32_t Drive_Version_Number;
	uint64_t Drive_Size;						// In bytes
	uint64_t Drive_Free_Space;					// In bytes
}FatFS_Drive;


// MACROS
#define FAT_FS_Append_To_File(a, b, c, d) FAT_FS_Write_To_File(a, b, c, d)


// FUNCTION PROTOTYPES
FatFS_Drive * Create_FAT_FS_Drive(FATFS * FS, char * path);
bool Mount_FAT_FS_Drive(FatFS_Drive * Drive);
bool Unmount_FAT_FS_Drive(FatFS_Drive * Drive);
bool FAT_FS_Create_Directory(FatFS_Drive * Drive, const char * Directory_Name);
bool FAT_FS_Open_Directory(FatFS_Drive * Drive, const char * Directory_Name, DIR * Directory);
bool FAT_FS_File_Exists(FatFS_Drive * Drive, const char * File_Name);
bool FAT_FS_Search_File(FatFS_Drive * Drive, const char * Search_Directory, const char * Search_String, char File_Path[256]);
bool FAT_FS_Create_File(FatFS_Drive * Drive, const char * File_Name);
bool FAT_FS_Delete_File(FatFS_Drive * Drive, const char * File_Name);
uint32_t FAT_FS_Write_To_File(FatFS_Drive * Drive, const char * File_Name, uint8_t * Data, uint32_t Data_Size);
uint32_t FAT_FS_Read_From_File(FatFS_Drive * Drive, const char * File_Name, uint8_t * Data, uint32_t Data_Size, uint32_t File_Offset);
uint64_t FAT_FS_Read_File_Size(FatFS_Drive * Drive, const char * File_Name);
bool FAT_FS_Update_File_Time_Stamp(FatFS_Drive * Drive, const char * File_Name, FILINFO * New_File_Info);
bool FAT_FS_IsFileWithPrefix(FatFS_Drive *Drive, char *FileNamePrefix, char *Path, char *FoundFileName);
bool FAT_FS_FileCopyDriveToDrive(FatFS_Drive *DestinationDrive, char *FileNameDestination, FatFS_Drive *SourceDrive, char *FileNameSource);
uint16_t FAT_FS_CountFilesWithPrefix(FatFS_Drive *Drive, char *FileNamePrefix, char *Path, char **FoundFileNames, bool StoreNames);
bool FAT_FS_UpdateDriveSpace(FatFS_Drive *Drive);
uint32_t FAT_FS_GetDriveSizeIn_MB(FatFS_Drive *Drive);
uint32_t FAT_FS_GetDriveFreeSpaceIn_MB(FatFS_Drive *Drive);
bool FAT_FS_Write_To_File_At(FatFS_Drive *Drive, const char *FileName, uint8_t *DataBuffer, uint32_t DataSize, uint32_t FileOffset);
bool FAT_FS_Rename_File(FatFS_Drive *Drive, const char *OldFileName, const char *NewFileName);
FSIZE_t FAT_FS_Get_FileSize(FatFS_Drive *Drive, const char *FileName);
FRESULT FAT_FS_DeleteAllFilesInDirectory(const char *Path, uint8_t *SubDirFound, uint8_t *FilesDeleted);

#endif /* FAT_FS_FAT_FS_H_ */
