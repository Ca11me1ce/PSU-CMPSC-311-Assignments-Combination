#ifndef CMPSC311_UTIL_INCLUDED
#define CMPSC311_UTIL_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_util.h
//  Description   : This is a set of general-purpose utility functions we use
//                  for the 311 homework assignments.
//
//  Author   : Patrick McDaniel
//  Created  : Sat Sep 21 06:47:40 EDT 2013
//
//  Change Log:
//
//  10/11/13	Added the timer comparison function definition (PDM)
//

// Includes
#include <stdint.h>
#include <gcrypt.h>

// Defines
#define CMPSC311_HASH_TYPE GCRY_MD_SHA1
#define CMPSC311_HASH_LENGTH (gcry_md_get_algo_dlen(CMPSC311_HASH_TYPE))

// Functional prototypes

int generate_md5_signature( unsigned char *buf, uint32_t size,
		                    unsigned char *sig, uint32_t *sigsz );
    // Generate MD5 signature from buffer

int bufToString( unsigned char *buf, uint32_t blen,
		         unsigned char *str, uint32_t slen );
    // Convert the buffer into a readable hex string

uint32_t getRandomValue( uint32_t min, uint32_t max );
    // Using strong randomness, generate random number

long compareTimes(struct timeval * tm1, struct timeval * tm2);
    // Compare two timer values 

uint64_t htonll64(uint64_t val);
	// Create a 64-byte host-to-network conversion

uint64_t ntohll64(uint64_t val);
	// Create a 64-byte network-to-host conversion

int b64UnitTest( void );
	// 64-bit conversion unit test
#endif
