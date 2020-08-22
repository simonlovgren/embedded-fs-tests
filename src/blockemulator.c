#include "blockemulator.h"
#include <string.h>
#include <stdlib.h>

/**
 * ----------------------------------------------------------------------------
 * Defines
 * ----------------------------------------------------------------------------
 */
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

/**
 * ----------------------------------------------------------------------------
 * Types
 * ----------------------------------------------------------------------------
 */
typedef struct
{
    size_t  block_size;
    size_t  blocks;
    uint8_t *p_buffer;
} block_data_t;

/**
 * ----------------------------------------------------------------------------
 * Prototypes
 * ----------------------------------------------------------------------------
 */

void *virtual_address_to_real( uint32_t virtual_address );

uint32_t get_block( uint32_t address );

/**
 * ----------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------
 */
static block_data_t blockdata = {
    .block_size = 0,
    .blocks     = 0,
    .p_buffer   = NULL
};

/**
 * ----------------------------------------------------------------------------
 * Public functions
 * ----------------------------------------------------------------------------
 */

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
bool block_create( size_t blocks, size_t block_size )
{
    if ( 0 != blockdata.block_size )
    {
        return false;
    }

    blockdata.p_buffer = malloc( blocks * block_size );
    if ( NULL == blockdata.p_buffer )
    {
        return false;
    }
    // Initiate blocks to 0xFF
    memset( (char *)blockdata.p_buffer, 0xFF, blocks * block_size );

    blockdata.blocks     = blocks;
    blockdata.block_size = block_size;

    return true;
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
void block_free( void )
{  
    free( blockdata.p_buffer );
    blockdata.block_size = 0;
    blockdata.blocks     = 0;
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
bool block_write( uint32_t address, uint32_t size, void *pData )
{
    void *p_address = virtual_address_to_real( address );
    for ( size_t i = 0; i < size; ++i )
    {
        ((uint8_t *)p_address)[i] = ( ((uint8_t *)p_address)[i] & ((uint8_t*)pData)[i] );
    }
    return true;
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
bool block_read( uint32_t address, uint32_t size, void *pData )
{
    void *p_address = virtual_address_to_real( address );
    memcpy( pData, p_address, size );
    return true;
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
bool block_erase( uint32_t address, uint32_t size )
{
    size_t   erase_size  = size;
    uint32_t _address    = address;

    while( erase_size > 0 )
    {
        // What we actually erase
        uint32_t block_addr = get_block( address );
        void *p_real_addr   = virtual_address_to_real( (uint32_t)block_addr );
        memset( (char *)p_real_addr, 0xFF, blockdata.block_size );

        // What we erase in the requested address range
        // Minimum of full block size and from given address to end of block
        uint32_t erased = min( blockdata.block_size, ( address % blockdata.block_size ) );
        erase_size = ( erase_size <= erased ) ? 0 : ( erase_size - erased );
        _address += blockdata.block_size;
    }

    return true;
}

/**
 * ----------------------------------------------------------------------------
 * Private functions
 * ----------------------------------------------------------------------------
 */

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
void *virtual_address_to_real( uint32_t virtual_address )
{
    void *p_base_addr = blockdata.p_buffer;
    return ( (char*)p_base_addr + virtual_address );
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
uint32_t get_block( uint32_t address )
{
    return ( address / blockdata.block_size ) * blockdata.block_size;
}