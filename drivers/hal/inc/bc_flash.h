#ifndef __BC_FLASH_H__
#define __BC_FLASH_H__
#include <stdint.h>

#define BC_PAGE_SIZE			256
#define BC_FLASH_START_ADDR		0
#define BC_FLASH_SIZE			0x1A00 //(0x80000 - 0x7e600)
/**
 ****************************************************************************************
 * @brief flash earase function
 *
 * @param[in] addr		flash page addr(has to be a multiple of BC_PAGE_SIZE)
 * @param[in] page_cnt	the count of earase pages
 *
 * @return 0 success, 1 failed.
 ****************************************************************************************
 */
uint32_t bc_flash_erase(uint32_t addr, uint32_t page_cnt);

/**
 ****************************************************************************************
 * @brief flash read function
 *
 * @param[in] addr		flash read start addr
 * @param[in] buf		data buf to save read flash data
 * @param[in] len		read data length
 *
 * @return 0 success, 1 failed.
 ****************************************************************************************
 */
uint32_t bc_flash_read(uint32_t addr, uint32_t len, uint8_t* buf);

/**
 ****************************************************************************************
 * @brief flash write function without earase
 *
 * @param[in] addr		flash write start addr
 * @param[in] buf		write data buffer
 * @param[in] len		write data length
 *
 * @return 0 success, 1 failed.
 ****************************************************************************************
 */
uint32_t bc_flash_write(uint32_t addr, uint32_t len, uint8_t* buf);

#if 0
_inline void bc_flash_example()
{
	bc_flash_erase(0, 1);
	bc_flash_read(0, buf, 256);
	memset(buf, 2,256 );
	bc_flash_write(0, buf, 256);
	bc_flash_read(0, buf, 256);
}
#endif

#endif/*__BC_FLASH_H__*/
