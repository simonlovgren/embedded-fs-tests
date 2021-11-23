#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * ----------------------------------------------------------------------------
 * Typedefs
 * ----------------------------------------------------------------------------
 */

typedef struct _block_device_s block_device_t;

/**
 * ----------------------------------------------------------------------------
 * Interface prototypes
 * ----------------------------------------------------------------------------
 */

/**
 * @brief  Create block device.
 *         Only one block device can exist at a time.
 * @param  blocks
 *         Number of blocks to allocate
 * @param  block_size
 *         size of blocks
 * @return Pointer to block memory device if ok, NULL otherwise.
 */
block_device_t* block_create( size_t blocks, size_t block_size );

/**
 * @brief  Free block device.
 * @return True if successful, false otherwise.
 */
void block_free( block_device_t* p_device );

/**
 * @brief  Write to block device.
 * @param  p_device
 *         Pointer to block device.
 * @param  address
 *         Address to write to.
 * @param  size
 *         Size of data to write.
 * @param  pData
 *         pointer to input data buffer.
 * @return True if successful, false otherwise.
 */
bool block_write( block_device_t* p_device, uintptr_t address, size_t size, void *p_data );

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
bool block_read( block_device_t* p_device, uintptr_t address, size_t size, void *p_data );

/**
 * @brief  Erase from block device.
 *         Device can only erase PAGE_SIZE at a time.
 *         If one byte is erased in a page, the whole page is cleared.
 * @param  p_device
 *         Pointer to block device.
 * @param  address
 *         Address to erase from.
 * @param  size
 *         Size to erase.
 * @return True if successful, false otherwise.
 */
bool block_erase( block_device_t* p_device, size_t address, size_t size );

/**
 * @brief  Erase whole block device memory.
 * @param  p_device
 *         Pointer to block device.
 * @return True if successful, false otherwise.
 */
bool block_erase_all( block_device_t* p_device );