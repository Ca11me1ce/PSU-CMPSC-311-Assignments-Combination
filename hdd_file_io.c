////////////////////////////////////////////////////////////////////////////////
//
//  File           : hdd_file_io.c
//  Description    : input/output file
//
//  Author         : Yang Chen
//  Last Modified  : 2017 12 01 12:04PM
//

// Includes
#include <malloc.h>
#include <string.h>

// Project Includes
#include <hdd_file_io.h>
#include <hdd_driver.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>
#include <hdd_network.h>

// Defines
#define CIO_UNIT_TEST_MAX_WRITE_SIZE 1024
#define HDD_IO_UNIT_TEST_ITERATIONS 10240


// Type for UNIT test interface
typedef enum {
	CIO_UNIT_TEST_READ   = 0,
	CIO_UNIT_TEST_WRITE  = 1,
	CIO_UNIT_TEST_APPEND = 2,
	CIO_UNIT_TEST_SEEK   = 3,
} HDD_UNIT_TEST_TYPE;

char *cio_utest_buffer = NULL;  // Unit test buffer

struct _file_recording{//Construct a struct to pass the infomation of each file               
	char path_to_pass[MAX_FILENAME_LENGTH];//the files' name during pass
	int open_signal;//the open signal for file
	uint32_t file_cursor_location;//Present the current cursor position in each file
	uint32_t block_id;//the id of constructing block
	uint32_t block_length;//the length of block
}hdd_file_record[MAX_HDD_FILEDESCR];


int hdd_initial_signal=0;//set an initial signal
uint32_t pass_global_bid=0;//make a global var for bid to pass
char hdd_file_io_scaning_data[HDD_MAX_BLOCK_SIZE];//construct a buffer to scan data         

//help function
//hdd_block_build: build block
//hdd_block_boom: communicate to hdd and get info from block
HddBitCmd hdd_block_build(uint32_t access_id_of_block,HDD_OP_TYPES operation_code, uint32_t opened_file_length, uint8_t block_flags, uint8_t hdd_r);
int hdd_block_boom(HddBitCmd hdd_bit_command, uint32_t *access_id_of_block, HDD_OP_TYPES *operation_code, uint32_t *opened_file_length, uint8_t *block_flags, uint8_t *hdd_r);



//
// Implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_format
// Description  : The function will make hdd drive formating and add a struct record for file
//
// Inputs       : void
// Outputs      : if sucessful, return 0, otherwise return -1
//
uint16_t hdd_format(void) {

	printf("---START HDD_FORMAT---\n");//start to format
	HddBitCmd hdd_bit_command;//usigned 64bits command
	HddBitResp hdd_bit_response;//usigned 64bits response

	uint32_t access_id_of_block;//block id from block
	HDD_OP_TYPES operation_code;//op
	uint32_t opened_file_length;//block length
	uint8_t block_flags;//flags
	uint8_t hdd_r;//hdd_r
	uint8_t hdd_return_value;//set a var to get return value from commnication to hardware
	

	if(hdd_initial_signal==0){//test initialize of file system

		hdd_bit_command=hdd_block_build(0, HDD_DEVICE, 0, HDD_INIT, 0);//initial the block
		hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_io_scaning_data);//communicat to hardware, get response from the block block
		hdd_return_value=hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r);//get needed info from block
		if(hdd_r || hdd_return_value){//test the return value from getting info
			return -1;
		}

		hdd_initial_signal=1;//set the initial signal to 1
	}

	hdd_bit_command=hdd_block_build(0, HDD_DEVICE, 0, HDD_FORMAT, 0);//format the block
	hdd_bit_response=hdd_client_operation(hdd_bit_command, NULL);//communicat to hardware, get response from the block
	hdd_return_value=hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r);//get needed info from block

	if(hdd_return_value||hdd_r){//test the return value of commnicating to block
		return -1;
	}


	memset(hdd_file_record, 0x0, sizeof(hdd_file_record));//initial a file struct

	hdd_bit_command=hdd_block_build(0, HDD_BLOCK_CREATE, sizeof(hdd_file_record), HDD_META_BLOCK, 0);//create the meta block
	hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_record);//communicat to hardware, get response from the block
	if((hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r) != 0)||(hdd_r != 0)){
		return -1;
	}//test the return value

	pass_global_bid=access_id_of_block;//store the bid we get into the global var to pass

	return 0;//else, return 0 successful
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_mount
// Description  : The function will mount the active hdd file system and get
//			a strcut record for file
//
// Inputs       : void
// Outputs      : if sucessful, return 0, otherwise return -1
//
uint16_t hdd_mount(void) {

	printf("---START HDD_MOUNT---\n");//start to mount
	HddBitCmd hdd_bit_command;//usigned 64bits command
	HddBitResp hdd_bit_response;//usigned 64bits response

	uint32_t access_id_of_block;//block id
	HDD_OP_TYPES operation_code;//op
	uint32_t opened_file_length;//block length
	uint8_t block_flags;//flags
	uint8_t hdd_r;//hdd_r
	uint8_t hdd_return_value;//var to store return value from communicating to hardware


	memset(hdd_file_record, 0x0, sizeof(struct _file_recording)*MAX_HDD_FILEDESCR);//initial a file struct

	if(hdd_initial_signal==0){//test initialize of file system

		hdd_bit_command=hdd_block_build(0, HDD_DEVICE, 0, HDD_INIT, 0);//initial the block
		hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_io_scaning_data);//communicat to hardware, get response from the block
		hdd_return_value=hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r);//get needed info from block
		if(hdd_r || hdd_return_value){//test the return value from getting info
			return -1;
		}

		hdd_initial_signal=1;//set the initial signal to 1
	}



	hdd_bit_command=hdd_block_build(0, HDD_BLOCK_READ, HDD_MAX_BLOCK_SIZE, HDD_META_BLOCK, 0);//read the meta block

	hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_io_scaning_data);//communicate to block, and get information

	if((hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r) != 0)||(hdd_r != 0)){
		return -1;
	}

	if((sizeof(hdd_file_record))!=opened_file_length){//check the size of struct
		return -1;
	}

	

	pass_global_bid=access_id_of_block;//store bid to global var to pass
	memcpy(hdd_file_record, hdd_file_io_scaning_data, opened_file_length);//copy infomation to struct
	for(int file_index=0; file_index<MAX_HDD_FILEDESCR; file_index++){//reset the file to initial
		hdd_file_record[file_index].file_cursor_location=0;
		hdd_file_record[file_index].open_signal=0;
	}

	return 0;
		
}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_unmount
// Description  : The function will unmount the active hdd file system, 
//			then save and close
//
// Inputs       : void
// Outputs      : if sucessful, return 0, otherwise return -1
//
uint16_t hdd_unmount(void) {

	printf("---START HDD_UNMOUNT---\n");//start to unmount
	HddBitCmd hdd_bit_command;//usigned 64bits command
	HddBitResp hdd_bit_response;//usigned 64bits response

	uint32_t access_id_of_block;//block id
	HDD_OP_TYPES operation_code;//op
	uint32_t opened_file_length;//file length
	uint8_t block_flags;//block flags
	uint8_t hdd_r;//hdd r
	uint8_t hdd_return_value;//var to store return value from communicating to hardware


	hdd_bit_command=hdd_block_build(pass_global_bid, HDD_BLOCK_OVERWRITE, sizeof(hdd_file_record), HDD_META_BLOCK, 0);//update the meta block
	hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_record);//communicate to block
	//get information from block
	if((hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r) != 0)||(hdd_r != 0)){
		return -1;
	}


	hdd_bit_command=hdd_block_build(0, HDD_DEVICE, 0, HDD_SAVE_AND_CLOSE, 0);//save and close
	hdd_bit_response=hdd_client_operation(hdd_bit_command, NULL);//communicate to the block
	//get current information from block
	hdd_return_value=hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r);
	if(hdd_r || hdd_return_value){//test the return value from getting info
		return -1;
	}

	hdd_initial_signal=0;//unmount, set initial signal to 0, and return

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_open
// Description  : This function will open files and return a file handle which point to the file
//
// Inputs       : path which is the path of file in the list of files
// Outputs      : if successful, return file handle, otherwise return -1
//
int16_t hdd_open(char *path) {
	printf("---START HDD_OPEN---\n");//start to open
	int16_t file_handle;// that is the file handle we will pass it in whole process
	int16_t file_not_create=-1;//when file is not created, show this signal

	HddBitCmd hdd_bit_command;//usigned 64bits command
	HddBitResp hdd_bit_response;//usigned 64bits response


	uint32_t access_id_of_block;//block id
	HDD_OP_TYPES operation_code;//op
	uint32_t opened_file_length;//block length
	uint8_t block_flags;//flags
	uint8_t hdd_r;//hdd_r
	uint8_t hdd_return_value;//var to store the return value


	if(hdd_initial_signal==0){//initial the file system
		return-1;
		for(int file_index=0; file_index<MAX_HDD_FILEDESCR; file_index++){//initial every struct for each file
			strcpy(hdd_file_record[file_index].path_to_pass, "");
			hdd_file_record[file_index].open_signal=0;
			hdd_file_record[file_index].file_cursor_location=0;
			hdd_file_record[file_index].block_id=0;
			hdd_file_record[file_index].block_length=0;
		}	
		

		hdd_bit_command=hdd_block_build(0, HDD_DEVICE, 0, HDD_INIT, 0);//initial the block
		hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_io_scaning_data);//communicat to hardware, get response from the block
		hdd_return_value=hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r);//get needed info from block
		if(hdd_r || hdd_return_value){//test the return value from getting info
			return -1;
		}


		hdd_initial_signal=1;//set initial signal to 1
	}

	for(int16_t file_index=0; file_index<MAX_HDD_FILEDESCR; file_index++){//go though all files
		if(hdd_file_record[file_index].path_to_pass !=NULL){//if path_to_pass is allocated
			if(strncmp(hdd_file_record[file_index].path_to_pass, path, strlen(path))==0){//compare the content of path to pass
				file_handle=file_index;//then record the file handle
				break;//get out of for-loop after we get the file handle
			}
			else{
				file_handle=file_not_create;//else the path_to_pass is nothing, mark the signal
			}
		}
	}

	if(file_handle != file_not_create){//if the file handle is recorded
		hdd_file_record[file_handle].file_cursor_location=0;//set the cursor position to the beginning of file
		hdd_file_record[file_handle].open_signal=1;//the file is opened
		return file_handle;//return file handle successfully
	}
	else if(file_handle==file_not_create){//if the file is not created
		for(int file_index=0; file_index<MAX_HDD_FILEDESCR; file_index++){//go though files
			if(strlen(hdd_file_record[file_index].path_to_pass) == 0){//if nothing in path_to_pass
				strncpy(hdd_file_record[file_index].path_to_pass, path, MAX_FILENAME_LENGTH);//copy path to path_to_pass

				hdd_file_record[file_index].open_signal=1;//the file is opened
				hdd_file_record[file_index].file_cursor_location=0;//set the cursor position to the beginning of file
				hdd_file_record[file_index].block_length=0;//nothing in the block, the block length is 0
				file_handle=file_index;//record the file handle
				return file_handle;//return file handle successfully
			}
		}
	}
	return -1;//else return -1, open failure
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_close
// Description  : This function will close files after finishing all operations
//
// Inputs       : fh-file handle of the object that uses to close
// Outputs      : if sucessful, return 0, otherwise return -1
//
int16_t hdd_close(int16_t fh) {
	printf("---START HDD_CLOSE---\n");

	if(hdd_initial_signal==0){//check the initial signal
		return-1;

	}


	if(hdd_file_record[fh].path_to_pass !=NULL){//check whether open files sucessfully
		hdd_file_record[fh].open_signal=0;//close files, set the open signal to 0
		hdd_file_record[fh].file_cursor_location=0;//reset the cursor position to the biginning of file
		return 0;//close successful, return 0
	}

	return -1;//else situation, return -1(failure)
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_read
// Description  : This function reads up to count_bytes from the file handle into the data
//
// Inputs       : fh-file handle of file for the read
// 			data-the buffer to take the read_byte into
//			count-the amount of byte that will read from the file
// Outputs      : if successful, return read bytes, otherwise return -1
//
int32_t hdd_read(int16_t fh, void * data, int32_t count) {
	printf("---START HDD_READ---\n");

	uint32_t opened_file_length;//the file length
	uint32_t bytes_to_read_infile;//bytes of reading
	
	HddBitCmd hdd_bit_command;//unsigned 64bits command
	HddBitResp hdd_bit_response;//unsigned 64bits response
	HDD_OP_TYPES operation_code;//operation code
	uint32_t access_id_of_block;//block id
	uint8_t hdd_r;//hdd r
	uint8_t block_flags;//block flags

	if(hdd_initial_signal==0){//check the global initial signal
		return-1;

	}


	if(hdd_file_record[fh].path_to_pass ==NULL){//check whether open files sucessfully
		return -1;
	}

	if(hdd_file_record[fh].block_id == 0){//if there is nothing in block
		return 0;
	}


	hdd_bit_command=hdd_block_build(hdd_file_record[fh].block_id, HDD_BLOCK_READ, HDD_MAX_BLOCK_SIZE, HDD_NULL_FLAG, 0);//read the block, and build
	hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_io_scaning_data);//communicate to the block
	//get information from the block
	if((hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r) != 0)||(hdd_r != 0)){
		return -1;
	}

	if(hdd_file_record[fh].block_id !=access_id_of_block){//check the block id
		return -1;
	}



	if(opened_file_length <= hdd_file_record[fh].file_cursor_location){
		bytes_to_read_infile=0;//if file length is shorter, nothing to read, return 0
	}
	else{//if in range
		if(opened_file_length < hdd_file_record[fh].file_cursor_location+count){//test count size with current position
			bytes_to_read_infile=opened_file_length-hdd_file_record[fh].file_cursor_location;//record the read bytes
		}
		else{
			bytes_to_read_infile=count;//else count the read bytes
		}
	
		memcpy(data, &hdd_file_io_scaning_data[hdd_file_record[fh].file_cursor_location], bytes_to_read_infile);//read content in file to the buffer with read bytes
		hdd_file_record[fh].file_cursor_location = hdd_file_record[fh].file_cursor_location + bytes_to_read_infile;//record the current cursor position in this file
	}

	return bytes_to_read_infile;//successful, return read bytes
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_write
// Description  : This function writes up to count_bytes from the buffer
//
// Inputs       : fh-file handle for file to write
//			data-the buffer to write
//			count-the amount of bytes that will be written
// Outputs      : if successful, return write bytes, otherwise return -1
//
int32_t hdd_write(int16_t fh, void *data, int32_t count) {
	printf("---START HDD_WRITE---\n");//start write bytes

	uint32_t opened_file_length;//the file length
	uint32_t bytes_to_write_tofile;//bytes of reading
	
	HddBitCmd hdd_bit_command;//unsigned 64bits command
	HddBitResp hdd_bit_response;//unsigned 64bits response
	HDD_OP_TYPES operation_code;//operation code

	uint8_t hdd_r;//hdd block r
	uint8_t block_flags;//block flags
	uint32_t access_id_of_block;//block id

	if(hdd_initial_signal==0){//check the initial signal
		return-1;

	}

	
	if(hdd_file_record[fh].path_to_pass ==NULL){//check whether open files sucessfully
		return -1;
	}


	logMessage(LOG_INFO_LEVEL, "*******************************");
	if(hdd_file_record[fh].block_id == 0){//if there is nothing in block


		hdd_bit_command=hdd_block_build(0, HDD_BLOCK_CREATE, count, HDD_NULL_FLAG, 0);//create a block
		hdd_bit_response=hdd_client_operation(hdd_bit_command, data);//communicate to the block
		//get information of the block, and test the return value
		if((hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r) != 0)||(hdd_r != 0)){
		return -1;
		}


		hdd_file_record[fh].file_cursor_location=count;//set the file position to the value of count
		hdd_file_record[fh].block_length=count;//the block length is count also
		hdd_file_record[fh].block_id=access_id_of_block;//record the block id
	}
	else{

		hdd_bit_command=hdd_block_build(hdd_file_record[fh].block_id, HDD_BLOCK_READ, HDD_MAX_BLOCK_SIZE, 0, 0);//read the block
		hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_io_scaning_data);//communicate to the block
		//get information from the block ,and test values we get
		if((hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r) != 0)||(hdd_r != 0)){
			return -1;
		}



		if(opened_file_length < hdd_file_record[fh].file_cursor_location+count){//if the write bytes are too big
			if(hdd_file_record[fh].file_cursor_location+count > HDD_MAX_BLOCK_SIZE){//and biger than the max block size, return -1(failure)
				return -1;
			}


			hdd_bit_command=hdd_block_build(hdd_file_record[fh].block_id, HDD_BLOCK_DELETE, 0, HDD_NULL_FLAG, 0);//DELETE the block
			hdd_bit_response=hdd_client_operation(hdd_bit_command, NULL);//communicate to the block
			//get information from the block ,and test values we get
			if((hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r) != 0)||(hdd_r != 0)){
				return -1;
			}



			memcpy(&hdd_file_io_scaning_data[hdd_file_record[fh].file_cursor_location], data, count);//write the buffer content to file with size count
			hdd_file_record[fh].file_cursor_location=hdd_file_record[fh].file_cursor_location+count;//reset the cursor position in file
			hdd_file_record[fh].block_length=hdd_file_record[fh].file_cursor_location;//reset the block length




			hdd_bit_command=hdd_block_build(0, HDD_BLOCK_CREATE, hdd_file_record[fh].block_length, HDD_NULL_FLAG, 0);//create block
			hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_io_scaning_data);//communite to block
			//get block information, and test values we get
			if((hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r) != 0)||(hdd_r != 0)){
				return -1;
			}

			hdd_file_record[fh].block_id=access_id_of_block;//record the block id

		}
		else{
			memcpy(&hdd_file_io_scaning_data[hdd_file_record[fh].file_cursor_location], data, count);//copy information to the buffer
			hdd_file_record[fh].file_cursor_location=hdd_file_record[fh].file_cursor_location+count;//compute position and record


			hdd_bit_command=hdd_block_build(hdd_file_record[fh].block_id, HDD_BLOCK_OVERWRITE, opened_file_length, HDD_NULL_FLAG, 0);//update the block
			hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_io_scaning_data);//communicate to the block
			//get information from the block, and test values we get
			if((hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r) != 0)||(hdd_r != 0)){
				return -1;
			}



		}
	}

	bytes_to_write_tofile=count;//record write bytes
	return bytes_to_write_tofile;//if sucessfully write, return write-bytes
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_seek
// Description  : This function seeks the correct position in the file from file handle
//
// Inputs       : fh-file handle which pointed to file
//			loc-the offset from the beginning of file to the position that will seek
// Outputs      : if successful, return 0, otherwise return 0
//
int32_t hdd_seek(int16_t fh, uint32_t loc) {
	printf("---START HDD_SEEK---\n");//start to seek

	uint32_t opened_file_length;//the file length
	//uint32_t bytes_to_write_tofile;//bytes of reading
	
	HddBitCmd hdd_bit_command;//unsigned 64bits command
	HddBitResp hdd_bit_response;//unsigned 64bits response
	HDD_OP_TYPES operation_code;

	uint8_t hdd_r;//hdd block r
	uint8_t block_flags;//block flags
	uint32_t access_id_of_block;//block id


	if(hdd_initial_signal==0){//check the global initial signal
		return-1;

	}

	if(hdd_file_record[fh].path_to_pass ==NULL){//check whether open files sucessfully
		return -1;
	}


	if(loc<0){//test loc, if not valid, return -1
		return -1;
	}
	if (hdd_file_record[fh].open_signal==0){//if the file is not open, return -1
		return -1;
	}


	hdd_bit_command=hdd_block_build(hdd_file_record[fh].block_id, HDD_BLOCK_READ, HDD_MAX_BLOCK_SIZE, HDD_NULL_FLAG, 0);//read the block
	hdd_bit_response=hdd_client_operation(hdd_bit_command, hdd_file_io_scaning_data);//communicate to the block
		//get information from the block ,and test values we get
	if((hdd_block_boom(hdd_bit_response, &access_id_of_block, &operation_code, &opened_file_length, &block_flags, &hdd_r) != 0)||(hdd_r != 0)){
		return -1;
	}


	if(hdd_file_record[fh].block_id<0){//if the block id is not valid, return -1
		return -1;
	}

	if (opened_file_length<loc){//if loc too big, it is not possible to seek
		return -1;
	}

	hdd_file_record[fh].file_cursor_location = loc;//else record file location and return 0

	return 0;
}

//help function
//hdd_block_build: build a block
HddBitCmd hdd_block_build(uint32_t access_id_of_block, HDD_OP_TYPES operation_code, uint32_t opened_file_length, uint8_t block_flags, uint8_t hdd_r){

	HddBitCmd hdd_bit_command;
	hdd_bit_command=0;//start with 0
	hdd_bit_command = (uint64_t)operation_code << 62;//op will have 62nd and 63rd bit
	hdd_bit_command |= (uint64_t)opened_file_length << 36;//block length will in 36-61
	hdd_bit_command |= (uint64_t)block_flags << 33;//flags will have 3 bits and in 33-35
	hdd_bit_command |= (uint64_t)hdd_r << 32;//1 bit for block r
	hdd_bit_command |= (HddBitCmd)access_id_of_block;//the 32 bits are block id

	return hdd_bit_command;//get the block

}

//communicate to block, and get needed information
int hdd_block_boom(HddBitCmd hdd_bit_command, uint32_t *access_id_of_block, HDD_OP_TYPES *operation_code, uint32_t *opened_file_length, uint8_t *block_flags, uint8_t *hdd_r){

	//go to corresponding position to catch necessary infomation from block
	*operation_code=(int)((hdd_bit_command>>62)&0x3);
	*opened_file_length=(uint32_t)((hdd_bit_command>>36)&0x3ffffff);
	*block_flags=(int)((hdd_bit_command>>33)&0x7);
	*hdd_r=(int)((hdd_bit_command>>32)&1);//that used to get the value of block r
	*access_id_of_block=(int32_t)(hdd_bit_command&0xffffffff);//get block id in block

	return 0;//if sucessful, return 0
	

}


////////////////////////////////////////////////////////////////////////////////
//
// Function     : hddIOUnitTest
// Description  : Perform a test of the HDD IO implementation
//
// Inputs       : None
// Outputs      : 0 if successful or -1 if failure

int hddIOUnitTest(void) {

	// Local variables
	uint8_t ch;
	int16_t fh, i;
	int32_t cio_utest_length, cio_utest_position, count, bytes, expected;
	char *cio_utest_buffer, *tbuf;
	HDD_UNIT_TEST_TYPE cmd;
	char lstr[1024];

	// Setup some operating buffers, zero out the mirrored file contents
	cio_utest_buffer = malloc(HDD_MAX_BLOCK_SIZE);
	tbuf = malloc(HDD_MAX_BLOCK_SIZE);
	memset(cio_utest_buffer, 0x0, HDD_MAX_BLOCK_SIZE);
	cio_utest_length = 0;
	cio_utest_position = 0;

	// Format and mount the file system
	if (hdd_format() || hdd_mount()) {
		logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : Failure on format or mount operation.");
		return(-1);
	}

	// Start by opening a file
	fh = hdd_open("temp_file.txt");
	if (fh == -1) {
		logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : Failure open operation.");
		return(-1);
	}

	// Now do a bunch of operations
	for (i=0; i<HDD_IO_UNIT_TEST_ITERATIONS; i++) {

		// Pick a random command
		if (cio_utest_length == 0) {
			cmd = CIO_UNIT_TEST_WRITE;
		} else {
			cmd = getRandomValue(CIO_UNIT_TEST_READ, CIO_UNIT_TEST_SEEK);
		}
		logMessage(LOG_INFO_LEVEL, "----------");

		// Execute the command
		switch (cmd) {

		case CIO_UNIT_TEST_READ: // read a random set of data
			count = getRandomValue(0, cio_utest_length);
			logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : read %d at position %d", count, cio_utest_position);
			bytes = hdd_read(fh, tbuf, count);
			if (bytes == -1) {
				logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : Read failure.");
				return(-1);
			}

			// Compare to what we expected
			if (cio_utest_position+count > cio_utest_length) {
				expected = cio_utest_length-cio_utest_position;
			} else {
				expected = count;
			}
			if (bytes != expected) {
				logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : short/long read of [%d!=%d]", bytes, expected);
				return(-1);
			}
			if ( (bytes > 0) && (memcmp(&cio_utest_buffer[cio_utest_position], tbuf, bytes)) ) {

				bufToString((unsigned char *)tbuf, bytes, (unsigned char *)lstr, 1024 );
				logMessage(LOG_INFO_LEVEL, "CIO_UTEST R: %s", lstr);
				bufToString((unsigned char *)&cio_utest_buffer[cio_utest_position], bytes, (unsigned char *)lstr, 1024 );
				logMessage(LOG_INFO_LEVEL, "CIO_UTEST U: %s", lstr);

				logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : read data mismatch (%d)", bytes);
				return(-1);
			}
			logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : read %d match", bytes);


			// update the position pointer
			cio_utest_position += bytes;
			break;

		case CIO_UNIT_TEST_APPEND: // Append data onto the end of the file
			// Create random block, check to make sure that the write is not too large
			ch = getRandomValue(0, 0xff);
			count =  getRandomValue(1, CIO_UNIT_TEST_MAX_WRITE_SIZE);
			if (cio_utest_length+count >= HDD_MAX_BLOCK_SIZE) {

				// Log, seek to end of file, create random value
				logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : append of %d bytes [%x]", count, ch);
				logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : seek to position %d", cio_utest_length);
				if (hdd_seek(fh, cio_utest_length)) {
					logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : seek failed [%d].", cio_utest_length);
					return(-1);
				}
				cio_utest_position = cio_utest_length;
				memset(&cio_utest_buffer[cio_utest_position], ch, count);

				// Now write
				bytes = hdd_write(fh, &cio_utest_buffer[cio_utest_position], count);
				if (bytes != count) {
					logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : append failed [%d].", count);
					return(-1);
				}
				cio_utest_length = cio_utest_position += bytes;
			}
			break;

		case CIO_UNIT_TEST_WRITE: // Write random block to the file
			ch = getRandomValue(0, 0xff);
			count =  getRandomValue(1, CIO_UNIT_TEST_MAX_WRITE_SIZE);
			// Check to make sure that the write is not too large
			if (cio_utest_length+count < HDD_MAX_BLOCK_SIZE) {
				// Log the write, perform it
				logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : write of %d bytes [%x]", count, ch);
				memset(&cio_utest_buffer[cio_utest_position], ch, count);
				bytes = hdd_write(fh, &cio_utest_buffer[cio_utest_position], count);
				if (bytes!=count) {
					logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : write failed [%d].", count);
					return(-1);
				}
				cio_utest_position += bytes;
				if (cio_utest_position > cio_utest_length) {
					cio_utest_length = cio_utest_position;
				}
			}
			break;

		case CIO_UNIT_TEST_SEEK:
			count = getRandomValue(0, cio_utest_length);
			logMessage(LOG_INFO_LEVEL, "HDD_IO_UNIT_TEST : seek to position %d", count);
			if (hdd_seek(fh, count)) {
				logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : seek failed [%d].", count);
				return(-1);
			}
			cio_utest_position = count;
			break;

		default: // This should never happen
			CMPSC_ASSERT0(0, "HDD_IO_UNIT_TEST : illegal test command.");
			break;

		}

	}

	// Close the files and cleanup buffers, assert on failure
	if (hdd_close(fh)) {
		logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : Failure close close.", fh);
		return(-1);
	}
	free(cio_utest_buffer);
	free(tbuf);

	// Format and mount the file system
	if (hdd_unmount()) {
		logMessage(LOG_ERROR_LEVEL, "HDD_IO_UNIT_TEST : Failure on unmount operation.");
		return(-1);
	}

	// Return successfully
	return(0);
}
