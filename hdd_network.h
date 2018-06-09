#ifndef HDD_NETWORK_INCLUDED
#define HDD_NETWORK_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File          : hdd_network.h
//  Description   : This is the network definitions for  the HDD simulator.
//
//  Author        : Patrick McDaniel
//  Last Modified : Thu Oct 30 06:59:59 EDT 2014
//

// Include Files

// Project Include Files
#include <hdd_driver.h>

// Defines
#define HDD_MAX_BACKLOG 5
#define HDD_NET_HEADER_SIZE sizeof(HddBitResp)
#define HDD_DEFAULT_IP "127.0.0.1"
#define HDD_DEFAULT_PORT 19876

//
// Functional Prototypes
HddBitResp hdd_client_operation(HddBitCmd cmd, void *buf);
    // This is the implementation of the client operation (hdd_client.c)

int hdd_server( void );
    // This is the implementation of the server application (hdd_server.c)

//
// Network Global Data
extern int            hdd_network_shutdown; // Flag indicating shutdown
extern unsigned char *hdd_network_address;  // Address of HDD server 
extern unsigned short hdd_network_port;     // Port of HDD server

#endif
