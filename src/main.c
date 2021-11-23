/**
 * ----------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "blockemulator.h"

/**
 * ----------------------------------------------------------------------------
 * Defines
 * ----------------------------------------------------------------------------
 */
#define BLOCKS                      10
#define BLOCK_SIZE                  16
#define OFFSET_BLOCK_DEVICE_ADDRESS 128

#define print_all(A,B,C,D) (print_blocks(A,B,C,D,__LINE__,__FILE__))

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
void print_divider( size_t width, char c )
{
    for ( size_t i = 0; i < width; ++i )
    {
        printf("%c", c);
    }
    printf("\n");
}
/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
void print_blocks( block_device_t* p_device, size_t blocks, size_t blocksize, uintptr_t start_address, uint32_t line, char *file )
{
    uint8_t* p_buffer = malloc( blocksize );

    printf( "%s:%u\n", file, line );
    print_divider((blocksize*3)-1+7+8, '-');
    printf( "v-addr      |  data (block)\n" );
    print_divider((blocksize*3)-1+7+8, '-');
    printf( "0x%08X  |  ", (unsigned int)start_address );
    for ( size_t i = 0; i < blocks * blocksize; ++i )
    {
        if ( ( i > 0 ) && ( i % blocksize == 0 ) )
        {
            printf( "\n" );
            printf( "0x%08X  |  ", (unsigned int)(start_address+i) );
        }
        uint8_t data = 0xFF;
        if ( false == block_read( p_device, i+start_address, sizeof( uint8_t ), &data ) )
        {
            printf( "ERROR reading data \n" );
        }
        printf( "%02X ", data );
    }
    printf("\n");
    print_divider((blocksize*3)-1+7+8, '-');
    printf( "\n\n" );

    free( p_buffer );
}

/**
 * ----------------------------------------------------------------------------
 * Interface functions
 * ----------------------------------------------------------------------------
 */

/**
 * ***************************************************************************
 * function
 * ***************************************************************************
 */
int main( int argc, char** argv )
{
    block_device_t* p_device;
    uint8_t read_buf[256];

    char* first  = "ABCDEFGHIJKLMNOP";
    char* second = "QRSTUVWXYZ012345";
    char* yzy    = "YZYZYZYZYZYZYZYZ";
    char* zyz    = "ZYZYZYZYZYZYZYZY";

    uint8_t pattern_aa[] = {
        0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
        0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA
    };
    uint8_t pattern_55[] = {
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55
    };

    // ------------------------------------
    // Device with invalid block size
    // ------------------------------------
    p_device = block_create( BLOCKS, 9, 0 );
    // Create block device
    if ( p_device )
    {
        block_free( p_device );
        printf( "FAIL: Created block device with invalid block size.\n" );
    }
    else
    {
        printf( "SUCCESS: Did not create block device with invalid block size.\n" );
    }

    // ------------------------------------
    // Device with unaligned start address
    // ------------------------------------
    p_device = block_create( BLOCKS, BLOCK_SIZE, 100 );
    // Create block device
    if ( p_device )
    {
        block_free( p_device );
        printf( "FAIL: Created block device with invalid start address.\n" );
    }
    else
    {
        printf( "SUCCESS: Did not create block device with invalid start address.\n" );
    }

    // ------------------------------------
    // Access outside block device range
    // ------------------------------------
    p_device = block_create( BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );
    // Create block device
    if ( NULL == p_device )
    {
        printf( "Failed to create block device.\n" );
        return 1;
    }

    if( block_read( p_device, 2*BLOCKS*BLOCK_SIZE+OFFSET_BLOCK_DEVICE_ADDRESS, 16, &read_buf ) || block_read( p_device, 0, 16, &read_buf ) )
    {
        printf( "FAIL: Granted access to memory outside block device range for READ.\n" );
        return 1;
    }
    else
    {
        printf( "SUCCESS: Denied access to memory outside block device range for READ.\n" );
    }
    if( block_write( p_device, 2*BLOCKS*BLOCK_SIZE+OFFSET_BLOCK_DEVICE_ADDRESS, 16, first ) || block_write( p_device, 0, 16, first ) )
    {
        printf( "FAIL: Granted access to memory outside block device range for WRITE.\n" );
        return 1;
    }
    else
    {
        printf( "SUCCESS: Denied access to memory outside block device range for WRITE.\n" );
    }
    if( block_erase( p_device, 2*BLOCKS*BLOCK_SIZE+OFFSET_BLOCK_DEVICE_ADDRESS, 2 ) || block_erase( p_device, 0, 2 ) )
    {
        printf( "FAIL: Granted access to memory outside block device range for ERASE.\n" );
        return 1;
    }
    else
    {
        printf( "SUCCESS: Denied access to memory outside block device range for ERASE.\n" );
    }

    // Free / Destroy block
    block_free( p_device );

    // ------------------------------------
    // Device with start address 0
    // ------------------------------------
    p_device  = block_create( BLOCKS, BLOCK_SIZE, 0 );
    // Create block device
    if ( NULL == p_device )
    {
        printf( "Failed to create block device.\n" );
        return 1;
    }

    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );

    block_write( p_device, 0*BLOCK_SIZE, 16, first );
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );
    block_write( p_device, 1*BLOCK_SIZE, 16, second );
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );
    block_write( p_device, 2*BLOCK_SIZE, 16, yzy );
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );
    block_write( p_device, 3*BLOCK_SIZE, 16, zyz );
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );

    block_write( p_device, 4*BLOCK_SIZE, 16, first );
    block_write( p_device, 4*BLOCK_SIZE, 16, second );
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );


    block_write( p_device, 5*BLOCK_SIZE, 16, yzy );
    block_write( p_device, 5*BLOCK_SIZE, 16, zyz );
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );

    block_erase( p_device, 0*BLOCK_SIZE, 2 );
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );

    block_erase( p_device, 30, 5 );
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );

    block_erase_all( p_device );
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );

    block_write( p_device, 0, sizeof(pattern_aa), &pattern_aa);
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );

    block_write( p_device, 0, sizeof(pattern_55), &pattern_55);
    print_all( p_device, BLOCKS, BLOCK_SIZE, 0 );

    // Free / Destroy block
    block_free( p_device );

    // ------------------------------------
    // Device with start address 128
    // ------------------------------------
    p_device  = block_create( BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );
    // Create block device
    if ( NULL == p_device )
    {
        printf( "Failed to create block device with offset address.\n" );
        return 1;
    }

    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );

    block_write( p_device, (0*BLOCK_SIZE)+OFFSET_BLOCK_DEVICE_ADDRESS, 16, first );
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );
    block_write( p_device, (1*BLOCK_SIZE)+OFFSET_BLOCK_DEVICE_ADDRESS, 16, second );
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );
    block_write( p_device, (2*BLOCK_SIZE)+OFFSET_BLOCK_DEVICE_ADDRESS, 16, yzy );
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );
    block_write( p_device, (3*BLOCK_SIZE)+OFFSET_BLOCK_DEVICE_ADDRESS, 16, zyz );
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );

    block_write( p_device, (4*BLOCK_SIZE)+OFFSET_BLOCK_DEVICE_ADDRESS, 16, first );
    block_write( p_device, (4*BLOCK_SIZE)+OFFSET_BLOCK_DEVICE_ADDRESS, 16, second );
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );


    block_write( p_device, (5*BLOCK_SIZE)+OFFSET_BLOCK_DEVICE_ADDRESS, 16, yzy );
    block_write( p_device, (5*BLOCK_SIZE)+OFFSET_BLOCK_DEVICE_ADDRESS, 16, zyz );
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );

    block_erase( p_device, (0*BLOCK_SIZE)+OFFSET_BLOCK_DEVICE_ADDRESS, 2 );
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );

    block_erase( p_device, 30+OFFSET_BLOCK_DEVICE_ADDRESS, 5 );
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );

    block_erase_all( p_device );
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );

    block_write( p_device, OFFSET_BLOCK_DEVICE_ADDRESS, sizeof(pattern_aa), &pattern_aa);
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );

    block_write( p_device, OFFSET_BLOCK_DEVICE_ADDRESS, sizeof(pattern_55), &pattern_55);
    print_all( p_device, BLOCKS, BLOCK_SIZE, OFFSET_BLOCK_DEVICE_ADDRESS );

    // Free / Destroy block
    block_free( p_device );

    return 0;
}