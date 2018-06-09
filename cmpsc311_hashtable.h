#ifndef CMPSC311_HASHTABLE_INCLUDED
#define CMPSC311_HASHTABLE_INCLUDED

////////////////////////////////////////////////////////////////////////////////
//
//  File          : cmpsc311_hashtable.h
//  Description   : This is a generic hashtable implementation used for
//                  data structure storage and access.
////
//  Author   : Patrick McDaniel
//  Created  : Sat Sep  6 08:56:10 EDT 2014
//

// Includes
#include <stdint.h>

// Defines
#define HT_COOKIE_VALUE 0xa3a3
typedef unsigned long HtIndexValue;

// Hash table entry structure
typedef struct HtEntry {
	uint16_t 	    cookie;  // This is a cookie value to detect memory corruption
	HtIndexValue	index;   // This is the "key value" index of the object
	void           *block;   // This is the data block of the stored item
	struct HtEntry *prev;    // This is the previous item in the local chain
	struct HtEntry *next;    // This is the next item in the local chain
} HtEntryData;

// Hash table structure
typedef struct  {
	uint16_t         htTableSize;  // The the bits in the hash values
	uint32_t	     elements;     // This is the number of elements
	HtEntryData    **hasHTable;    // This is the hash table itself
} HTable;

// Hash table iterator
typedef struct {
	HTable      *table; // The table we are iterating through
	uint16_t 	 idx;   // The current index into the hash table
	HtEntryData *ptr;   // The pointer into the linked list at the index
} HtIterator;

//
// Hashtable Interface

int initHashTable( HTable *ht, uint16_t bits );
	// This function initializes the hash table to a width of 2^(bits) width

int cleanupHashTable( HTable *ht );
	// Cleanup the hash table

int insertValueInHashTable( HTable *ht, HtIndexValue idx, void *blk );
	// Insert a value into the hashtable of value idx, block size blk

void * findValueInHashTable( HTable *ht, HtIndexValue idx );
	// Find a block for a particular index value in the table

void * deleteValueFromHashTable( HTable *ht, HtIndexValue idx );
	// Delete a value from the hashtable of value idx, return it

//
// Iterator Functions

int initHashTableIterator( HTable *ht, HtIterator *it );
	// Initialize the iterator

void * iterateHashTable( HtIterator *it );
	// Iterate through the hash table, returns the next value in the table

//
// Unit Testing

int hashTableUnitTest( void );
	// Perform a test of the hash table functionality

#endif
