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
#define BLOCKS     10
#define BLOCK_SIZE 16

#define print_all(A,B,C) (print_blocks(A,B,C,__LINE__,__FILE__))

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
void print_blocks( block_device_t* p_device, size_t blocks, size_t blocksize, uint32_t line, char *file )
{
    uint8_t* p_buffer = malloc( blocksize );
    printf( "%s:%u\n", file, line );
    for ( size_t i = 0; i < blocks * blocksize; ++i )
    {
        if ( ( i > 0 ) && ( i % 16 == 0 ) )
        {
            printf( "\n" );
        }
        uint8_t data = 0xFF;
        if ( false == block_read( p_device, i, sizeof( uint8_t ), &data ) )
        {
            printf( "ERROR reading data \n" );
        }
        printf( "%02X ", data );
    }
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
    block_device_t* p_device = block_create( BLOCKS, BLOCK_SIZE );
    // Create block device
    if ( false == p_device )
    {
        printf( "Failed to create block device.\n" );
        return 1;
    }

    print_all( p_device, BLOCKS, BLOCK_SIZE );

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

    block_write( p_device, 0*BLOCK_SIZE, 16, first );
    print_all( p_device, BLOCKS, BLOCK_SIZE );
    block_write( p_device, 1*BLOCK_SIZE, 16, second );
    print_all( p_device, BLOCKS, BLOCK_SIZE );
    block_write( p_device, 2*BLOCK_SIZE, 16, yzy );
    print_all( p_device, BLOCKS, BLOCK_SIZE );
    block_write( p_device, 3*BLOCK_SIZE, 16, zyz );
    print_all( p_device, BLOCKS, BLOCK_SIZE );

    block_write( p_device, 4*BLOCK_SIZE, 16, first );
    block_write( p_device, 4*BLOCK_SIZE, 16, second );
    print_all( p_device, BLOCKS, BLOCK_SIZE );


    block_write( p_device, 5*BLOCK_SIZE, 16, yzy );
    block_write( p_device, 5*BLOCK_SIZE, 16, zyz );
    print_all( p_device, BLOCKS, BLOCK_SIZE );

    block_erase( p_device, 0*BLOCK_SIZE, 2 );
    print_all( p_device, BLOCKS, BLOCK_SIZE );

    block_erase( p_device, 30, 5 );
    print_all( p_device, BLOCKS, BLOCK_SIZE );

    block_erase_all( p_device );
    print_all( p_device, BLOCKS, BLOCK_SIZE );

    block_write( p_device, 0, sizeof(pattern_aa), &pattern_aa);
    print_all( p_device, BLOCKS, BLOCK_SIZE );

    block_write( p_device, 0, sizeof(pattern_55), &pattern_55);
    print_all( p_device, BLOCKS, BLOCK_SIZE );

    // Free / Destroy block
    block_free( p_device );

    return 0;
}