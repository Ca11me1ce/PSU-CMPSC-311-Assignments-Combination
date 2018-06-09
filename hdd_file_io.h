#ifndef HDD_FILE_IO_INCLUDED
#define HDD_FILE_IO_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File           : hdd_file_io.h
//  Description    : This is the header file for the standardized IO functions
//                   for used to access the HDD storage system.
//
//  Author         : Patrick McDaniel
//  Last Modified  : Tue Sep 16 19:38:42 EDT 2014
//

// Include files
#include <stdint.h>

// Project include files
#include <hdd_driver.h>

// Defines
#define MAX_HDD_FILEDESCR 1024
#define MAX_FILENAME_LENGTH 128


// Management operations

uint16_t hdd_format(void);
	// This function formats the CRUD drive, and adds the file allocation table.

uint16_t hdd_mount(void);
	// This function mount the current crud file system and loads the file allocation table.

uint16_t hdd_unmount(void);
	// This function unmounts the current crud file system and saves the file allocation table.

//
// Interface functions

int16_t hdd_open(char *path);
	// This function opens the file and returns a file handle

int16_t hdd_close(int16_t fd);
	// This function closes the file

int32_t hdd_read(int16_t fd, void *buf, int32_t count);
	// Reads "count" bytes from the file handle "fh" into the buffer  "buf"

int32_t hdd_write(int16_t fd, void *buf, int32_t count);
	// Writes "count" bytes to the file handle "fh" from the buffer  "buf"

int32_t hdd_seek(int16_t fd, uint32_t loc);
	// Seek to specific point in the file

//
// Unit testing for the module

int hddIOUnitTest(void);
	// Perform a test of the CRUD IO implementation

#endif


