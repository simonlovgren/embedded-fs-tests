#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "blockemulator.h"

#define BLOCKS     10
#define BLOCK_SIZE 16

#define print_all(A,B) (print_blocks(A,B,__LINE__,__FILE__))

void print_blocks( size_t blocks, size_t blocksize, uint32_t line, char *file )
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
    char *yzy    = "YZYZYZYZYZYZYZYZ";
    char *zyz    = "ZYZYZYZYZYZYZYZY";

    block_write( 0*BLOCK_SIZE, 16, first );
    print_all( BLOCKS, BLOCK_SIZE );
    block_write( 1*BLOCK_SIZE, 16, second );
    print_all( BLOCKS, BLOCK_SIZE );
    block_write( 2*BLOCK_SIZE, 16, yzy );
    print_all( BLOCKS, BLOCK_SIZE );
    block_write( 3*BLOCK_SIZE, 16, zyz );
    print_all( BLOCKS, BLOCK_SIZE );

    block_write( 4*BLOCK_SIZE, 16, first );
    block_write( 4*BLOCK_SIZE, 16, second );
    print_all( BLOCKS, BLOCK_SIZE );


    block_write( 5*BLOCK_SIZE, 16, yzy );
    block_write( 5*BLOCK_SIZE, 16, zyz );
    print_all( BLOCKS, BLOCK_SIZE );

    block_erase( 0*BLOCK_SIZE, 2 );
    print_all( BLOCKS, BLOCK_SIZE );

    block_erase( 30, 5 );
    print_all( BLOCKS, BLOCK_SIZE );


    // Free / Destroy block
    block_free();

    return 0;
}