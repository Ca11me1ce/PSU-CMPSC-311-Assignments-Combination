////////////////////////////////////////////////////////////////////////////////
//
//  File          : hdd_client.c
//  Description   : This is the client side of the CRUD communication protocol.
//
//   Author       : Yang Chen
//  Last Modified : Thu Nov 30 01:13:59 EDT 2017
//

// Include Files
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

// Project Include Files
#include <hdd_network.h>
#include <cmpsc311_log.h>
#include <cmpsc311_util.h>
#include <hdd_driver.h>


//global variable
int client_socket_fd=-1;//client socket file descriptor

//help function
//hdd_get_info: get necessary info from block
//hdd_client_connect: get connect
//hdd_value_to_send: send value parket to server
//hdd_value_to_receive: receive necessary value parket
int hdd_get_info(HddBitResp hdd_bit_resp, int *operation_code, int *file_block_length, uint8_t *block_flags, int *hdd_r, int32_t *block_id);
int hdd_client_connect(void);
int hdd_value_to_send(HddBitCmd request, void *buf);
HddBitResp hdd_value_to_receive(void *buf);


////////////////////////////////////////////////////////////////////////////////
//
// Function     : hdd_client_operation
// Description  : This the client operation that sends a request to the CRUD
//                server.   It will:
//
//                1) if INIT make a connection to the server
//                2) send any request to the server, returning results
//                3) if CLOSE, will close the connection
//
// Inputs       : cmd - the request opcode for the command
//                buf - the block to be read/written from (READ/WRITE)
// Outputs      : the response structure encoded as needed
HddBitResp hdd_client_operation(HddBitCmd cmd, void *buf){


	HddBitResp hdd_bit_response;//response of receive
	int hdd_send_record;//send statu record

	int operation_code;//op
	int file_block_length;//block size
	int hdd_return_value;//var to store return value
	uint8_t block_flags;//block flags
	int hdd_r;//hdd block r
	int32_t block_id;//block id



	hdd_return_value=hdd_get_info(cmd, &operation_code, &file_block_length, &block_flags, &hdd_r, &block_id);//get necessary info from block
	if(hdd_return_value!=0||hdd_r!=0){//test infomation from block
		return -1;
	}


	if (block_flags==HDD_INIT){//if the block flag is initilization
		if(hdd_client_connect()==-1){//get connect
			return -1;//test connection, if failure, return -1
		}
	}

	hdd_send_record=hdd_value_to_send(cmd, buf);//send value
	if (hdd_send_record!=0){//test send status
		return -1;
	}


	hdd_bit_response=hdd_value_to_receive(buf);//receive value parket
	if(hdd_bit_response==-1){//test value parket
		return -1;
	}


	if (block_flags==HDD_SAVE_AND_CLOSE){//if block is save and close, close cressponding socket
		close(client_socket_fd);//close socket
		client_socket_fd=-1;//reset the file descriptor
	}

	return hdd_bit_response;//return the 
}


//help function
//get necessary info from block
int hdd_get_info(HddBitResp hdd_bit_resp, int *operation_code, int *file_block_length, uint8_t *block_flags, int *hdd_r, int32_t *block_id){

	*operation_code=((hdd_bit_resp>>62)&0x3);
	*file_block_length=((hdd_bit_resp>>36)&0x3ffffff);
	*block_flags=(uint8_t) ((hdd_bit_resp>>33)&0x7);
	*hdd_r=(int)((hdd_bit_resp>>32)&1);//that used to get the value of block r
	*block_id=(int32_t)(hdd_bit_resp&0xffffffff);//get block id in block
	return 0;


}


int hdd_client_connect(void){

	struct sockaddr_in caddr;//construct socket type sruct 
	int connect_signal;//var to store the statu of connect

	client_socket_fd=socket(PF_INET, SOCK_STREAM, 0);//create socket 
	if (client_socket_fd==-1){//test socket
		return -1;
	}

	caddr.sin_family=AF_INET;//protocol family
	caddr.sin_port=htons(HDD_DEFAULT_PORT);//port
	if (inet_aton(HDD_DEFAULT_IP, &(caddr.sin_addr))==0){//ip address
		return -1;
	}

	connect_signal=connect(client_socket_fd, (const struct sockaddr *)&caddr, sizeof(struct sockaddr));//socket connect
	if (connect_signal==-1){//test connect signal

		return -1;
	}
	return 0;
}


int hdd_value_to_send(HddBitCmd hdd_bit_command, void *buf){

	int hdd_command_len=sizeof(HddBitCmd);//command length
	int hdd_command_write;//var to write to server

	int operation_code;//operation code
	int file_block_length;//length
	int write_bytes;//write bytes
	uint8_t block_flags;//block flags
	int hdd_r;//hdd block r
	int32_t block_id;//block id

	int hdd_return_value;//var to store return value

	hdd_return_value=hdd_get_info(hdd_bit_command, &operation_code, &file_block_length, &block_flags, &hdd_r, &block_id);//get info from block
	if(hdd_return_value!=0||hdd_r!=0){//test info of block
		return -1;
	}

	HddBitCmd *hdd_net_command=malloc(hdd_command_len);//allocate space
	*hdd_net_command=htonll64(hdd_bit_command);//create a 64-byte host-to-network conversion 


	hdd_command_write=write(client_socket_fd, hdd_net_command, hdd_command_len); //send command to server
	while (hdd_command_len>hdd_command_write){//send all bytes to server with whlie loop
		hdd_command_write=hdd_command_write+write(client_socket_fd, &((char *)hdd_net_command)[hdd_command_write],hdd_command_len-hdd_command_write);
	}
	free(hdd_net_command);//free allocated space


	if (operation_code==HDD_BLOCK_CREATE||operation_code==HDD_BLOCK_OVERWRITE){//determine op type to send buf
		write_bytes=write(client_socket_fd, buf, file_block_length);//sending all bytes
		while (write_bytes<file_block_length){
			write_bytes=write_bytes+write(client_socket_fd, &((char *)buf)[write_bytes], file_block_length-write_bytes);
		}
	}

	return 0;
}




HddBitResp hdd_value_to_receive(void *buf){


	HddBitResp hdd_bit_resp;//hdd response
	int hdd_resp_size=sizeof(HddBitResp);//response size

	int hdd_command_read;//var to read from server
	int read_bytes;//read bytes

	int file_block_length;//block length
	int operation_code;//operation code
	int hdd_return_value;//var to store return value
	uint8_t block_flags;//block flags
	int hdd_r;//hdd block r
	int32_t block_id;//block id

	HddBitResp *hdd_bit_response=malloc(hdd_resp_size);//allocate space
	hdd_command_read=read(client_socket_fd, hdd_bit_response, hdd_resp_size);//receive bytes
	while (hdd_command_read<hdd_resp_size){//receive all bytes from server
		hdd_command_read=hdd_command_read+read(client_socket_fd, &((char *)hdd_bit_response)[hdd_command_read], hdd_resp_size-hdd_command_read);
	}

	hdd_bit_resp=ntohll64(*hdd_bit_response);//create a 64-byte network-to-host conversion
	free(hdd_bit_response);//free allocated space

	hdd_return_value=hdd_get_info(hdd_bit_resp, &operation_code, &file_block_length, &block_flags, &hdd_r, &block_id);//get info from block
	if(hdd_return_value!=0||hdd_r!=0){//test info
		return -1;
	}




	if (operation_code==HDD_BLOCK_READ){//determine op type
		read_bytes=read(client_socket_fd, buf, file_block_length);//receive bytes

		while (read_bytes<file_block_length){//receive all bytes from server
			read_bytes=read_bytes+read(client_socket_fd, &((char *)buf)[read_bytes], file_block_length-read_bytes);
		}
	}

	return hdd_bit_resp;
}


