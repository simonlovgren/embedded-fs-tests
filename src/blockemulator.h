#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief  Create block device.
 *         Only one block device can exist at a time.
 * @param  blocks
 *         Number of blocks to allocate
 * @param  block_size
 *         size of blocks
 * @return True if successful, false otherwise.
 */
bool block_create( size_t blocks, size_t block_size );

/**
 * @brief  Free block device.
 * @return True if successful, false otherwise.
 */
void block_free( void );

/**
 * @brief  Write to block device.
 * @param  address
 *         Address to write to.
 * @param  size
 *         Size of data to write.
 * @param  pData
 *         pointer to input data buffer.
 * @return True if successful, false otherwise.
 */
bool block_write( uint32_t address, uint32_t size, void *pData );

/**
 * @brief  Read from block device.
 * @param  address
 *         Address to read from.
 * @param  size
 *         Size to read.
 * @param  pData
 *         pointer to output data buffer.
 * @return True if successful, false otherwise.
 */
bool block_read( uint32_t address, uint32_t size, void *pData );

/**
 * @brief  Erase from block device.
 *         Device can only erase PAGE_SIZE at a time.
 *         If one byte is erased in a page, the whole page is cleared.
 * @param  address
 *         Address to erase from.
 * @param  size
 *         Size to erase.
 * @param  pData
 *         pointer to data buffer.
 * @return True if successful, false otherwise.
 */
bool block_erase( uint32_t address, uint32_t size );
