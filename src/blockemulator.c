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

#define ERASE_STATE ( 0xFF )
/**
 * ----------------------------------------------------------------------------
 * Types
 * ----------------------------------------------------------------------------
 */
typedef struct _block_device_s
{
    size_t    blocks;
    size_t    block_size;
    uintptr_t start_address;
    uint8_t*  buffer[];
} block_device_t;

/**
 * ----------------------------------------------------------------------------
 * Prototypes
 * ----------------------------------------------------------------------------
 */

/**
 * Validates a virtual address falls within range of block device.
 * @param   p_device
 *          Pointer to block device
 * @param   virtual_address
 *          Virtual address to validate.
 * @returns true if valid, false if not.
 */
bool valid_virtual_address( block_device_t* p_device, uintptr_t virtual_address );

/**
 * Converts virtual (block device) address to real address.
 * @param   p_device
 *          Pointer to block device
 * @param   virtual_address
 *          Virtual address to convert to real address.
 * @returns Pointer to where in the device data buffer the
 *          virtual address points.
 */
void *virtual_address_to_real( block_device_t* p_device, uintptr_t virtual_address );

/**
 * Get virtual address to start of block where given address redides.
 * @param   p_device
 *          Pointer to block device.
 * @param   virtual_address
 *          virtual address to get start of block for.
 * @returns Pointer to start of block.
 */
uintptr_t get_block( block_device_t* p_device, uintptr_t virtual_address );

/**
 * ----------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------
 */

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
block_device_t* block_create( size_t blocks, size_t block_size, uintptr_t start_address )
{
    // Basic sanity check
    if ( block_size % 2 != 0 || start_address % block_size != 0 )
    {
        // block size not power of two OR
        // start address not aligned on block size
        return false;
    }

    block_device_t* p_device = malloc( sizeof(block_device_t) + ( blocks * block_size ) );
    if ( NULL == p_device )
    {
        return NULL;
    }
    // Set up block device header/metadata
    p_device->block_size    = block_size;
    p_device->blocks        = blocks;
    p_device->start_address = start_address;
    // Initiate blocks to erased state
    memset( (char *)p_device->buffer, ERASE_STATE, p_device->blocks * p_device->block_size );

    return p_device;
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
void block_free( block_device_t* p_device )
{
    if( NULL != p_device)
    {
        free( p_device );
    }
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
bool block_write( block_device_t* p_device, uintptr_t address, size_t size, void *p_data )
{
    if( NULL == p_device || !valid_virtual_address( p_device, address ) )
    {
        return false;
    }
    void *p_address = virtual_address_to_real( p_device, address );
    for ( size_t i = 0; i < size; ++i )
    {
        ((uint8_t *)p_address)[i] = ( ((uint8_t *)p_address)[i] & ((uint8_t*)p_data)[i] );
    }
    return true;
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
bool block_read( block_device_t* p_device, uintptr_t address, size_t size, void *p_data )
{
    if( NULL == p_device || !valid_virtual_address( p_device, address ) )
    {
        return false;
    }
    void *p_address = virtual_address_to_real( p_device, address );
    memcpy( p_data, p_address, size );
    return true;
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
bool block_erase( block_device_t* p_device, uintptr_t address, size_t size )
{
    size_t    _size       = size;    // Size left to erase
    uintptr_t _address    = address; // Current addres to erase from

    if( NULL == p_device || !valid_virtual_address( p_device, address ) )
    {
        return false;
    }

    while( _size > 0 )
    {
        // What we actually erase
        size_t block_addr = get_block( p_device, _address );
        void *p_real_addr   = virtual_address_to_real( p_device, block_addr );
        memset( (char *)p_real_addr, ERASE_STATE, p_device->block_size );

        // What we erase in the requested address range
        // Minimum of full block size and from given address to end of block
        size_t block_offset = ( _address % p_device->block_size ); // Offset of _address into block
        size_t erased       = block_offset > 0 ? p_device->block_size-block_offset : p_device->block_size;
        _size = ( _size <= erased ) ? 0 : ( _size - erased );
        _address += erased;
    }

    return true;
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
bool block_erase_all( block_device_t* p_device )
{
    if( NULL == p_device)
    {
        return false;
    }
    memset( (char *)p_device->buffer, ERASE_STATE, p_device->blocks * p_device->block_size );
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
bool valid_virtual_address( block_device_t* p_device, uintptr_t virtual_address )
{
    return ( virtual_address >= p_device->start_address ) &&
           ( virtual_address < ((p_device->block_size * p_device->blocks) + p_device->start_address ));
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
void *virtual_address_to_real( block_device_t* p_device, uintptr_t virtual_address )
{
    void *p_base_addr = &(p_device->buffer);
    return ( (uint8_t*)p_base_addr + ( virtual_address - p_device->start_address) );
}

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
uintptr_t get_block( block_device_t* p_device, uintptr_t virtual_address )
{
    return ( virtual_address / p_device->block_size ) * p_device->block_size;
}