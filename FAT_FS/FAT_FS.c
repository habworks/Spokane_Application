/*
 * FAT_FS.c
 *
 *  Created on: Dec 28, 2020
 *      Author: devin
 */

#include "FAT_FS.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FatFS_Drive * Create_FAT_FS_Drive(FATFS * FS, char * path)
{
	FatFS_Drive * drive = (FatFS_Drive *)malloc(sizeof(FatFS_Drive));

	if(drive != NULL)
	{
		drive->FatFS = FS;
		drive->Path = path;
		drive->Mounted = false;
	}

	return drive;
}

bool Mount_FAT_FS_Drive(FatFS_Drive * Drive)
{
	FRESULT res = f_mount (Drive->FatFS, Drive->Path, 1);
	if(res == FR_OK)
	{
		Drive->Mounted = true;

		// Retrieve Drive info
		res = f_getlabel (Drive->Path, Drive->Drive_Label, &Drive->Drive_Version_Number);

		// Retrieve the drive space info
		uint32_t free_clusters;
		FATFS * fs = Drive->FatFS;
		res = f_getfree (Drive->Path, &free_clusters, &fs);
		if(res == FR_OK)
		{
			Drive->Drive_Size = (uint64_t)(fs->n_fatent - 2) * (uint64_t)(fs->csize * fs->ssize);
			Drive->Drive_Free_Space = (uint64_t)free_clusters * (uint64_t)(fs->csize * fs->ssize);
			//printf("Drive Mounted\r\n");
		}
		else
		{
			Drive->Drive_Size = 0;
			Drive->Drive_Free_Space = 0;
			//printf("ERROR reading drive size %d\r\n", res);
		}

		return true;
	}
	else
		return false;
}

bool Unmount_FAT_FS_Drive(FatFS_Drive * Drive)
{
	FRESULT res = f_mount (NULL, Drive->Path, 0);
	if(res == FR_OK)
	{
		Drive->Mounted = false;
		return true;
	}
	else
	{
		printf("Error unmounting drive %s: %d\r\n", Drive->Path, res);
		return false;
	}
}

bool FAT_FS_Create_Directory(FatFS_Drive * Drive, const char * Directory_Name)
{
	// Drive must be mounted
	if(Drive->Mounted)
	{
		// Create the directory path
		int Directory_Name_Size = strlen(Directory_Name);
		TCHAR folder_path[_MAX_LFN];//3 + Directory_Name_Size + 1]; //strlen(Directory_Name)
		memset(folder_path, 0, sizeof(folder_path));
		memcpy(folder_path, Drive->Path, 3);
		strncpy(&folder_path[3], Directory_Name, Directory_Name_Size);

		// Create the directory, or return true if it already exists
		FRESULT res = f_mkdir(folder_path);
		if((res == FR_OK) || (res == FR_EXIST))
			return true;
		else
		{
			printf("Error creating directory %s/%s: %d\r\n", Drive->Path, Directory_Name, res);
			return false;
		}
	}
	else
		return false;
}

bool FAT_FS_Open_Directory(FatFS_Drive * Drive, const char * Directory_Name, DIR * Directory)
{
	// Drive must be mounted
	if(Drive->Mounted)
	{
		// Create the directory path
		TCHAR folder_path[_MAX_LFN];//4 + strlen(Directory_Name)];
		memset(folder_path, 0, sizeof(folder_path));
		memcpy(folder_path, Drive->Path, 3);
		if(strlen(Directory_Name) > 1)
			strncpy(&folder_path[3], Directory_Name, strlen(Directory_Name) - 1);

		FRESULT res = f_opendir(Directory, folder_path);
		if(res == FR_OK)
			return true;
		else
		{
			printf("Error opening directory %s/%s: %d\r\n", Drive->Path, Directory_Name, res);
			return false;
		}
	}
	else
		return false;
}

bool FAT_FS_File_Exists(FatFS_Drive * Drive, const char * File_Name)
{
	// Drive must be mounted
	if(Drive->Mounted)
	{
		// Create the file path
		TCHAR file_path[_MAX_LFN];//3 + strlen(File_Name)];
		memset(file_path, 0, sizeof(file_path));
		memcpy(file_path, Drive->Path, 3);
		strcpy(&file_path[3], File_Name);

		// Attempt to open the file
		FIL open_file;
		FRESULT res = f_open(&open_file, file_path, FA_READ);
		if(res == FR_OK)
		{
			// The file was opened, close the file
			res = f_close(&open_file);
			if(res == FR_OK)
				return true;
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

bool FAT_FS_Search_File(FatFS_Drive * Drive, const char * Search_Directory, const char * Search_String, char File_Path[256])
{
	FRESULT fr;
	DIR dj;
	FILINFO fno;

	// Build the directory string to search
	TCHAR file_path[_MAX_LFN];//3 + strlen(Search_Directory)];
	memset(file_path, 0, sizeof(file_path));
	memcpy(file_path, Drive->Path, 3);
	strcpy(&file_path[3], Search_Directory);

	fr = f_findfirst(&dj, &fno, file_path, Search_String);

	if(fr == FR_OK && fno.fname[0])
	{
		// we found a file that matches
		memcpy(File_Path, fno.fname, 256);
		return true;
	}
	else
		return false;
}

bool FAT_FS_Create_File(FatFS_Drive * Drive, const char * File_Name)
{
	// Drive must be mounted
	if(Drive->Mounted)
	{
		// Create the file path
		TCHAR file_path[_MAX_LFN];//3 + strlen(File_Name)];
		memset(file_path, 0, sizeof(file_path));
		memcpy(file_path, Drive->Path, 3);
		strcpy(&file_path[3], File_Name);

		// Attempt to open the file
		FIL open_file;
		FRESULT res = f_open(&open_file, file_path, FA_WRITE | FA_CREATE_ALWAYS);
		if(res == FR_OK)
		{
			// Sync all the changes
			res = f_sync(&open_file);

			if(res == FR_OK)
			{
				// Close the file
				res = f_close(&open_file);

				if(res == FR_OK)
					return true;
				else
					return false;
			}
			else
			{
				f_close(&open_file);
				return false;
			}
		}
		else
		{
			printf("Error creating file %s/%s: %d\r\n", Drive->Path, File_Name, res);
			return false;
		}
	}
	else
		return false;
}

bool FAT_FS_Delete_File(FatFS_Drive * Drive, const char * File_Name)
{
	// Drive must be mounted
	if(Drive->Mounted)
	{
		// Create the file path
		TCHAR file_path[_MAX_LFN];//3 + strlen(File_Name)];
		memset(file_path, 0, sizeof(file_path));
		memcpy(file_path, Drive->Path, 3);
		strcpy(&file_path[3], File_Name);

		// Unlink the file
		FRESULT res = f_unlink(file_path);
		if(res == FR_OK)
			return true;
		else
		{
			printf("Error deleting file %s/%s: %d\r\n", Drive->Path, File_Name, res);
			return false;
		}
	}
	else
		return false;
}

uint32_t FAT_FS_Write_To_File(FatFS_Drive * Drive, const char * File_Name, uint8_t * Data, uint32_t Data_Size)
{
	// Drive must be mounted
	if(Drive->Mounted)
	{
		// Open the file
		TCHAR file_path[_MAX_LFN];//3 + strlen(File_Name) + 1];
		memset(file_path, 0, sizeof(file_path));
		memcpy(file_path, Drive->Path, 3);
		strcpy(&file_path[3], File_Name);

		FIL open_file;
		FRESULT res = f_open(&open_file, file_path, FA_WRITE | FA_OPEN_APPEND);
		if(res == FR_OK)
		{
			// Write the data
			UINT data_written = 0;
			res = f_write(&open_file, Data, Data_Size, &data_written);
			if(res == FR_OK)
			{
				// Sync all the changes
				res = f_sync(&open_file);
				res = f_close(&open_file);

				return (uint32_t)data_written;
			}
			else
			{
				f_close(&open_file);
				printf("Error writing to %s/%s: %d\r\n", Drive->Path, File_Name, res);

				// Retry writing the data
				printf("Retrying... ");
				res = f_open(&open_file, file_path, FA_WRITE | FA_OPEN_APPEND);
				if(res == FR_OK)
				{
					res = f_write(&open_file, Data, Data_Size, &data_written);
					if(res == FR_OK)
					{
						printf("OK\r\n");

						// Sync all the changes
						res = f_sync(&open_file);
						res = f_close(&open_file);

						return (uint32_t)data_written;
					}
					else
					{
						f_close(&open_file);
						printf("FAILED\r\n");
					}
				}
				else
					printf("FAILED\r\n");

				return 0;
			}
		}
		else
		{
			printf("Error opening file to write %s/%s: %d\r\n", Drive->Path, File_Name, res);
			return 0;
		}
	}
	else
		return 0;
}

uint32_t FAT_FS_Read_From_File(FatFS_Drive * Drive, const char * File_Name, uint8_t * Data, uint32_t Data_Size, uint32_t File_Offset)
{
	// Drive must be mounted
	if(Drive->Mounted)
	{
		// Open the file
		TCHAR file_path[_MAX_LFN];//3 + strlen(File_Name)];
		memset(file_path, 0, sizeof(file_path));
		memcpy(file_path, Drive->Path, 3);
		strcpy(&file_path[3], File_Name);

		FIL open_file;
		FRESULT res = f_open(&open_file, file_path, FA_READ);
		if(res == FR_OK)
		{
			// Move the file pointer to the offset
			f_lseek(&open_file, File_Offset);

			// Read the data
			UINT data_read = 0;
			res = f_read(&open_file, Data, Data_Size, &data_read);
			if(res != FR_OK)
				printf("Error rading from %s/%s: %d\r\n", Drive->Path, File_Name, res);

			// Close the file
			f_close(&open_file);

			// Return the amount of data that was read
			return (uint32_t)data_read;
		}
		else
		{
			printf("Error opening file to read %s/%s: %d\r\n", Drive->Path, File_Name, res);
			return 0;
		}
	}
	else
		return 0;
}

uint64_t FAT_FS_Read_File_Size(FatFS_Drive * Drive, const char * File_Name)
{
	// Drive must be mounted
	if(Drive->Mounted)
	{
		// Open the file
		TCHAR file_path[_MAX_LFN];//3 + strlen(File_Name)];
		memset(file_path, 0, sizeof(file_path));
		memcpy(file_path, Drive->Path, 3);
		strcpy(&file_path[3], File_Name);

		FIL open_file;
		FRESULT res = f_open(&open_file, file_path, FA_READ);
		if(res == FR_OK)
		{
			FSIZE_t size = f_size(&open_file);

			// Close the file
			f_close(&open_file);

			return (uint64_t)size;
		}
		else
		{
			printf("Error opening file to get size %s/%s: %d\r\n", Drive->Path, File_Name, res);
			return 0;
		}
	}
	else
		return 0;
}

bool FAT_FS_Update_File_Time_Stamp(FatFS_Drive * Drive, const char * File_Name, FILINFO * New_File_Info)
{
	// Drive must be mounted
	if(Drive->Mounted)
	{
		// create the file path
		TCHAR file_path[_MAX_LFN];//3 + strlen(File_Name)];
		memset(file_path, 0, sizeof(file_path));
		memcpy(file_path, Drive->Path, 3);
		strcpy(&file_path[3], File_Name);

		FRESULT res = f_utime(file_path, New_File_Info);
		if(res == FR_OK)
			return true;
		else
		{
			printf("Error updating file time stamp %s/%s: %d\r\n", Drive->Path, File_Name, res);
			return false;
		}
	}
	else
		return false;
}



/*******************************************************************************************************
* @brief Check the specified path to see if there is a file that starts with the file name prefix.
* The function returns true on the first instance of a file found that matches the name prefix. The specified
* path and sub-directories of that path will be checked.
*
* @author original: Hab Collector \n
*
* @note: Drive must be previously mounted
* @note: Unless FAT-FS is configured for relative PATH Path _FS_RPATH >= 1 Path is ignored and only the root
* directory is considered
*
* @param Drive: Handle to the FAT-FS Drive
* @param FileNamePrefix: Prefix of the file name to look for
* @param Path: Path of where to look for the file - See notes
* @param FoundName: Name of file found that matches the prefix - returned by pointer
*
* @return True on the first occurrence of a file with that name prefix.  If none found return false
*
* STEP 1: Open the specified directory based on path
* STEP 2: Read of preset directory for file or other directory
* STEP 3: If present read is directory change to directory
* STEP 4: If present read is file - does file match the file name prefix
********************************************************************************************************/
bool FAT_FS_IsFileWithPrefix(FatFS_Drive *Drive, char *FileNamePrefix, char *Path, char *FoundFileName)
{
	bool Status = false;

	if(Drive->Mounted)
	{
		FRESULT FileStatus;
		DIR Directory;
		static FILINFO FileInfo;

		// STEP 1: Open the specified directory based on path
#if _FS_RPATH >= 1
		UINT Index_PathEnd;
		FileStatus = f_opendir(&Directory, Path);
#else
		FileStatus = f_opendir(&Directory, "/");
#endif
		if (FileStatus == FR_OK)
		{
			for (;;)
			{
				// STEP 2: Read of preset directory for file or other directory
				FileStatus = f_readdir(&Directory, &FileInfo);
				if (FileStatus != FR_OK || FileInfo.fname[0] == 0)
					break;
#if _FS_RPATH >= 1
				// STEP 3: If present read is directory change to directory
				if (FileInfo.fattrib & AM_DIR)
				{
					Index_PathEnd = strlen(Path);
					sprintf(&Path[Index_PathEnd], "/%s", FileInfo.fname);
					FileStatus = f_chdir(Path);
					if (FileStatus != FR_OK)
						break;
					Path[Index_PathEnd] = 0;
				}
				else
#endif
				// STEP 4: If present read is file - does file match the file name prefix
				{
					int NameDifferences;
					NameDifferences = strncmp(FileInfo.fname, FileNamePrefix, strlen(FileNamePrefix));
					if (NameDifferences == 0)
					{
						memcpy(FoundFileName, FileInfo.fname, strlen(FileInfo.fname));
						Status = true;
						break;
					}
				}
			}
			f_closedir(&Directory);
			return(Status);
		}
	}
	else
		return(false);

	return(Status);

} // END OF FAT_FS_IsFileWithPrefix



/*******************************************************************************************************
* @brief Copies a file from one drive to another - the drives are not the same.
*
* @author original: Hab Collector \n
*
* @note: Both drives must be previously mounted
* @note: Unless FAT-FS is configured for relative PATH Path _FS_RPATH >= 1 Path is ignored and only the root
* directory is considered
*
* @param DestinationDrive: Handle to the FAT-FS Destination Drive
* @param SourceDrive: Handle to the FAT-FS Source Drive
* @param DestinationFileName: File name of destination
* @param SourceFileName: File name of source
*
* @return True if the file is copied successfully
*
* STEP 1: Verify Drives have been assigned with simple test
* STEP 2: Open the source file
* STEP 3: Open the destination file stream for writing - force creation
* STEP 4: Allocate memory buffer for file read write transfer
* STEP 5: Copy the file contents from Source To Buffer To Destination until all bytes copied
* STEP 6: Free resources and return status
********************************************************************************************************/
bool FAT_FS_FileCopyDriveToDrive(FatFS_Drive *DestinationDrive, char *DestinationFileName, FatFS_Drive *SourceDrive, char *SourceFileName)
{
	FIL DestinationFile;
	FIL SourceFile;
	FRESULT FileStatus;
	UINT BytesRead;
	UINT BytesWritten;
	uint32_t TotalBytesTransfer = 0;

	// STEP 1: Verify Drives have been assigned with simple test
	if ((DestinationDrive == NULL) || (SourceDrive == NULL))
		return(false);

	// Build the destination path
	TCHAR destination_path[_MAX_LFN];//3 + strlen(DestinationFileName)];
	memset(destination_path, 0, sizeof(destination_path));
	memcpy(destination_path, DestinationDrive->Path, 3);
	strcpy(&destination_path[3], DestinationFileName);

	// Build the source path
	TCHAR source_path[_MAX_LFN];//3 + strlen(SourceFileName)];
	memset(source_path, 0, sizeof(source_path));
	memcpy(source_path, SourceDrive->Path, 3);
	strcpy(&source_path[3], SourceFileName);

	// STEP 2: Open the source file
	FileStatus = f_open(&SourceFile, source_path, FA_READ);
	if (FileStatus != FR_OK)
	{
		printf("Error opening source: %s\r\n", source_path);
		return(false);
	}

	// STEP 3: Open the destination file stream for writing - force creation
	FileStatus = f_open(&DestinationFile, destination_path, FA_WRITE | FA_CREATE_ALWAYS);
	if (FileStatus != FR_OK)
	{
		f_close(&SourceFile);
		printf("Error creating destination file: %s\r\n", destination_path);
		return(false);
	}

	// STEP 4: Allocate memory buffer for file read write transfer
	char *FileBuffer = (char *)malloc(FILE_TRANSFER_BUFFER_SIZE);
	if (FileBuffer == NULL)
	{
		f_close(&SourceFile);
		f_close(&DestinationFile);
		return(false);
	}

	// STEP 5: Copy the file contents from Source To Buffer To Destination until all bytes copied
	bool FileTransferStatus = true;
	do
	{
		FileStatus = f_read(&SourceFile, FileBuffer, FILE_TRANSFER_BUFFER_SIZE, &BytesRead);
		if ((FileStatus == FR_OK) && BytesRead)
		{
			TotalBytesTransfer += BytesRead;
			FileStatus = f_write(&DestinationFile, FileBuffer, BytesRead, &BytesWritten);
			if (FileStatus != FR_OK)
			{
				printf("Error in file transfer\r\n");
				FileTransferStatus = false;
				break;
			}
		}
	} while ((FileStatus == FR_OK) && (BytesRead >= FILE_TRANSFER_BUFFER_SIZE));
	printf("Bytes Transfered: %d\r\n", (int)TotalBytesTransfer);

	// STEP 6: Free resources and return status
	f_close(&SourceFile);
	f_close(&DestinationFile);
	free(FileBuffer);
	return(FileTransferStatus);

} // END OF FAT_FS_FileCopyDriveToDrive



/*******************************************************************************************************
* @brief Check the specified path to count the number of files found with a specified file name prefix.
* If set to do so the function will also store the names of said files as it counts.
*
* @author original: Hab Collector \n
*
* @note: Drive must be previously mounted
* @note: Unless FAT-FS is configured for relative PATH Path _FS_RPATH >= 1 Path is ignored and only the root
* directory is considered
*
* @param Drive: Handle to the FAT-FS Drive
* @param FileNamePrefix: Prefix of the file name to look for
* @param Path: Path of where to look for the file - See notes
* @param FoundName: Pointer to a pointer (an array of an array) of file names
*
* @return Number of files found with said prefix
*
* STEP 1: Open the specified directory based on path
* STEP 2: Read of preset directory for file or other directory
* STEP 3: If present read is directory change to directory
* STEP 4: If present read is file - does file match the file name prefix
********************************************************************************************************/
uint16_t FAT_FS_CountFilesWithPrefix(FatFS_Drive *Drive, char *FileNamePrefix, char *Path, char **FoundFileNames, bool StoreNames)
{
	uint16_t FileCount = 0;

	if(Drive->Mounted)
	{
		FRESULT FileStatus;
		DIR Directory;
		static FILINFO FileInfo;

		// STEP 1: Open the specified directory based on path
#if _FS_RPATH >= 1
		UINT Index_PathEnd;
		FileStatus = f_opendir(&Directory, Path);
#else
		FileStatus = f_opendir(&Directory, "/");
#endif
		if (FileStatus == FR_OK)
		{
			for (;;)
			{
				// STEP 2: Read of preset directory for file or other directory
				FileStatus = f_readdir(&Directory, &FileInfo);
				if (FileStatus != FR_OK || FileInfo.fname[0] == 0)
					break;
#if _FS_RPATH >= 1
				// STEP 3: If present read is directory change to directory
				if (FileInfo.fattrib & AM_DIR)
				{
					Index_PathEnd = strlen(Path);
					sprintf(&Path[Index_PathEnd], "/%s", FileInfo.fname);
					FileStatus = f_chdir(Path);
					if (FileStatus != FR_OK)
						break;
					Path[Index_PathEnd] = 0;
				}
				else
#endif
				// STEP 4: If present read is file - does file match the file name prefix
				{
					int NameDifferences;
					NameDifferences = strncmp(FileInfo.fname, FileNamePrefix, strlen(FileNamePrefix));
					if (NameDifferences == 0)
					{
						if (StoreNames)
							memcpy(FoundFileNames[FileCount], FileInfo.fname, strlen(FileInfo.fname));
						FileCount++;
					}
				}
			}
			f_closedir(&Directory);
		}
	}
	return(FileCount);

} // END OF FAT_FS_CountFilesWithPrefix



/*******************************************************************************************************
* @brief Update the drive size and drive free space.  This is necessary as the drive handle struct members
* are only loaded when the drive is mounted.  During usage the actual amount of free space will change.
*
* @author original: Hab Collector \n
*
* @note: Drive must be previously mounted
*
* @param Drive: Handle to the FAT-FS Drive
*
* @return True on success
*
* STEP 1: Check drive is mounted, if so, Update the drive space information
********************************************************************************************************/
bool FAT_FS_UpdateDriveSpace(FatFS_Drive *Drive)
{
	uint32_t FreeClusters;
	FRESULT Status;

	// STEP 1: Check drive is mounted, if so, Update the drive space information
	if(Drive->Mounted)
	{
		Status = f_getfree (Drive->Path, &FreeClusters, &Drive->FatFS);
		if(Status == FR_OK)
		{
			Drive->Drive_Size = (uint64_t)(Drive->FatFS->n_fatent - 2) * (uint64_t)(Drive->FatFS->csize * Drive->FatFS->ssize);
			Drive->Drive_Free_Space = (uint64_t)FreeClusters * (uint64_t)(Drive->FatFS->csize * Drive->FatFS->ssize);
			return(true);
		}
	}
	return(false);

} // END OF FAT_FS_UpdateDriveSpace



/*******************************************************************************************************
* @brief Get the drive size of the FAT FS file system in Mega Bytes
*
* @author original: Hab Collector \n
*
* @note: Drive must be previously mounted
*
* @param Drive: Handle to the FAT-FS Drive
*
* @return Drive size in Mega Bytes MB
********************************************************************************************************/
uint32_t FAT_FS_GetDriveSizeIn_MB(FatFS_Drive *Drive)
{
	return((uint32_t)(Drive->Drive_Size / MEGA_BYTE));

} // END OF FAT_FS_GetDriveSizeIn_MB



/*******************************************************************************************************
* @brief Get the amount of free space on the FAT FS drive in Mega Bytes
*
* @author original: Hab Collector \n
*
* @note: Drive must be previously mounted
*
* @param Drive: Handle to the FAT-FS Drive
*
* @return Drive free space size in Mega Bytes MB
********************************************************************************************************/
uint32_t FAT_FS_GetDriveFreeSpaceIn_MB(FatFS_Drive *Drive)
{
	return((uint32_t)(Drive->Drive_Free_Space / MEGA_BYTE));

} // END OF FAT_FS_GetDriveFreeSpaceIn_MB



/*******************************************************************************************************
* @brief Writes data to file at the specified file offset.
*
* @author original: Hab Collector \n
*
* @note: Drive must be previously mounted
* @note: Unless FAT-FS is configured for relative PATH Path _FS_RPATH >= 1 Path is ignored and only the root
* directory is considered
*
* @param Drive: Handle to the FAT-FS Drive
* @param FileName: File name of the file to operate on
* @param DataBuffer: Buffer where data to be write to file is taken from
* @param DataSize: The number of bytes that will be copied
* @param FileOffset: File offset from which the data will be overwritten
*
* @return True if operation was successful
*
* STEP 1: Open the file
* STEP 2: At the file file offset write the desired data
********************************************************************************************************/
bool FAT_FS_Write_To_File_At(FatFS_Drive *Drive, const char *FileName, uint8_t *DataBuffer, uint32_t DataSize, uint32_t FileOffset)
{
	bool Status = false;
	if(Drive->Mounted)
	{
		// STEP 1: Open the file
		TCHAR FilePath[_MAX_LFN];
		memset(FilePath, 0, sizeof(FilePath));
		memcpy(FilePath, Drive->Path, 3);
		strcpy(&FilePath[3], FileName);
		FIL FileHandle;
		FRESULT FileStatus = f_open(&FileHandle, FilePath, FA_WRITE | FA_OPEN_APPEND);

		// STEP 2: At the file file offset write the desired data
		if(FileStatus == FR_OK)
		{
			UINT BytesWritten = 0;
			f_lseek(&FileHandle, FileOffset);
			FileStatus = f_write(&FileHandle, DataBuffer, DataSize, &BytesWritten);
			f_sync(&FileHandle);
			f_close(&FileHandle);
			if (BytesWritten == DataSize)
				Status = true;
		}
	}
	return(Status);

} // END OF FAT_FS_Write_To_File_At



/*******************************************************************************************************
* @brief Rename of a file on the specified drive
*
* @author original: Hab Collector \n
*
* @note: Drive must be previously mounted
* @note: Unless FAT-FS is configured for relative PATH Path _FS_RPATH >= 1 Path is ignored and only the root
* directory is considered
*
* @param Drive: Handle to the FAT-FS Drive
* @param OldFileName: The file name to be changed
* @param NewFileName: The new file name
*
* @return True if operation was successful
*
* STEP 1: Create full file paths
* STEP 2: Rename file
********************************************************************************************************/
bool FAT_FS_Rename_File(FatFS_Drive *Drive, const char *OldFileName, const char *NewFileName)
{
	// Drive must be mounted
	if (Drive->Mounted)
	{
		// STEP 1: Create full file paths
		// Create new file path
		TCHAR NewFilePath[_MAX_LFN];
		memset(NewFilePath, 0, sizeof(NewFilePath));
		memcpy(NewFilePath, Drive->Path, 3);
		strcpy(&NewFilePath[3], NewFileName);
		// Create old file path
		TCHAR OldFilePath[_MAX_LFN];
		memset(OldFilePath, 0, sizeof(OldFilePath));
		memcpy(OldFilePath, Drive->Path, 3);
		strcpy(&OldFilePath[3], OldFileName);

		// STEP 2: Rename file
		FRESULT Status = f_rename(OldFilePath, NewFilePath);
		if (Status == FR_OK)
			return(true);
		else
			return(false);
	}
	else
	{
		return(false);
	}

} // END OF FAT_FS_Rename_File



/*******************************************************************************************************
* @brief Return the size of a file in bytes
*
* @author original: Hab Collector \n
*
* @note: Drive must be previously mounted
* @note: Unless FAT-FS is configured for relative PATH Path _FS_RPATH >= 1 Path is ignored and only the root
* directory is considered
*
* @param Drive: Handle to the FAT-FS Drive
* @param FileName: The file name to be changed
*
* @return Returns the size of the file in bytes, 0 if an error occurred
*
* STEP 1: Create full file paths
* STEP 2: Return file size
********************************************************************************************************/
FSIZE_t FAT_FS_Get_FileSize(FatFS_Drive *Drive, const char *FileName)
{
	// Drive must be mounted
	if (Drive->Mounted)
	{
		// STEP 1: Create full file paths
		TCHAR FilePath[_MAX_LFN];
		memset(FilePath, 0, sizeof(FilePath));
		memcpy(FilePath, Drive->Path, 3);
		strcpy(&FilePath[3], FileName);

		// STEP 2: Return file size
		FIL FileHandle;
		FSIZE_t FileSize;
		FRESULT FileStatus = f_open(&FileHandle, FilePath, FA_WRITE | FA_OPEN_APPEND);
		if (FileStatus != FR_OK)
		    FileSize = 0;
		else
		    FileSize = f_size(&FileHandle);
		f_close(&FileHandle);
		return(FileSize);
	}
	else
	{
		return(0);
	}

} // END OF FAT_FS_Get_FileSize



/*******************************************************************************************************
* @brief Deletes all the files in the specified directory.  Note, only the files are deleted.  Any sub-
* directories found within this path are not removed.
*
* @author original: Hab Collector \n
*
* @note: Drive must be previously mounted
* @note: Unless FAT-FS is configured for relative PATH Path _FS_RPATH >= 1 Path is ignored and only the root
* directory is considered
*
* @param Path: Path where files to be deleted are found
* @param SubDirFound: Return by reference the number of sub-directories found
* @param FilesDeleted: Return by reference the number of files deleted
*
* @return File processing result, but SubDirFound and FilesDeleted (returned by reference) is the valued information
*
* STEP 1: Clear the count of Sub-Directories found and files deleted
* STEP 2: Search for and delete files - only count sub-directories
********************************************************************************************************/
FRESULT FAT_FS_DeleteAllFilesInDirectory(const char *Path, uint8_t *SubDirFound, uint8_t *FilesDeleted)
{
    DIR Directory;
    FRESULT Status;
    FILINFO FileInformation;

    // STEP 1: Clear the count of Sub-Directories found and files deleted
    *SubDirFound = 0;
    *FilesDeleted = 0;

    // STEP 2: Search for and delete files - only count sub-directories
    char * PathFileNameToDelete = (char *)malloc(MAX_FILE_NAME_LENGTH * sizeof(char));
    if (PathFileNameToDelete == NULL)
        return(FR_NOT_READY);
    Status = f_opendir(&Directory, Path);
    if (Status == FR_OK)
    {
        for(;;)
        {
            Status = f_readdir(&Directory, &FileInformation);
            // Check for end of reading the directory
            if (Status != FR_OK || FileInformation.fname[0] == 0)
                break;
            if (FileInformation.fattrib & AM_DIR)
            {
                // Count only the sub-directories
                *SubDirFound = *SubDirFound + 1;
            }
            else
            {
                // Delete and count the files found
                memset(PathFileNameToDelete, 0x00, MAX_FILE_NAME_LENGTH);
                sprintf(PathFileNameToDelete, "%s/%s", Path, FileInformation.fname);
                Status = f_unlink(PathFileNameToDelete);
                if (Status == FR_OK)
                    *FilesDeleted = *FilesDeleted + 1;
            }
        }
        f_closedir(&Directory);
    }
    free(PathFileNameToDelete);
    return(Status);

} // END OF FAT_FS_DeleteAllFilesInDirectory
