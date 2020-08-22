#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "blockemulator.h"

#define BLOCKS     10
#define BLOCK_SIZE 16

void print_all( size_t blocks, size_t blocksize )
{
    uint8_t* p_buffer = malloc( blocksize );

    for ( size_t i = 0; i < blocks * blocksize; ++i )
    {
        if ( ( i > 0 ) && ( i % 16 == 0 ) )
        {
            printf( "\n" );
        }
        uint8_t data = 0xFF;
        if ( false == block_read( i, sizeof( uint8_t ), &data ) )
        {
            printf( "ERROR reading data \n" );
        }
        printf( "%02X ", data );
    }
    printf( "\n\n" );

    free( p_buffer );
}

int main( int argc, char** argv )
{
    // Create block device
    if ( false == block_create( BLOCKS, BLOCK_SIZE ) )
    {
        printf( "Failed to create block device.\n" );
        return 1;
    }

    print_all( BLOCKS, BLOCK_SIZE );

    char *first  = "ABCDEFGHIJKLMNOP";
    char *second = "QRSTUVWXYZ012345";
    block_write( 0, 16, first );
    print_all( BLOCKS, BLOCK_SIZE );
    block_write( 16, 16, second );
    print_all( BLOCKS, BLOCK_SIZE );
    block_write( 32, 16, first );
    block_write( 32, 16, second );
    block_write( 32, 16, first );
    print_all( BLOCKS, BLOCK_SIZE );

    block_erase( 0, 2 );
    print_all( BLOCKS, BLOCK_SIZE );


    // Free / Destroy block
    block_free();

    return 0;
}