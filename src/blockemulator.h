#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool block_create( size_t blocks, size_t block_size );

void block_free( void );

bool block_write( uint32_t address, uint32_t size, void *pData );

bool block_read( uint32_t address, uint32_t size, void *pData );

bool block_erase( uint32_t address, uint32_t size );
