
#include "bc_flash.h"
#include "flash_config.h"
#include "bc62xx_flash.h"

#ifndef RELEASE
#define RELEASE
#endif

#define APP_START_ADDR			(SECTION_START_ADDR+SECTION_SIZE*SECTION_NUM)
#define APP_SIZE				(FLASH_START_ADDR+FLASH_SIZE-APP_START_ADDR)

uint32_t bc_flash_erase(uint32_t addr, uint32_t page_cnt)
{

	if((addr&(PAGE_SIZE-1))!= 0 && (addr+PAGE_SIZE*page_cnt)>APP_SIZE)
		return 1;
	addr += APP_START_ADDR;
#ifndef RELEASE
		printf( "erase data addr=0x%08x\r\n", addr);
#endif
	QSPI_Init();
	for(int i = 0; i < page_cnt; ++i){
		QSPI_PageEraseFlash(addr+i*PAGE_SIZE);
	}
	return 0;	
}

uint32_t bc_flash_read(uint32_t addr, uint32_t len, uint8_t* buf)
{
	if(len < 1 ||(((addr+len-1)&PAGE_MASK) != ((addr)&PAGE_MASK)) || buf == NULL){
		return 1;
	}
	addr += APP_START_ADDR;
#ifndef RELEASE
	printf("read addr=%08xx\r\n", addr);
#endif
	QSPI_Init();
	QSPI_ReadFlashData(addr,len,buf);
#ifndef RELEASE
	for(int i = 0; i < len; ++i){
		printf("%02x ", buf[i]);
	}
	printf("\r\n");
#endif

	return 0;
}

uint32_t bc_flash_write(uint32_t addr, uint32_t len, uint8_t* buf)
{
	if(len < 1 || (((addr+len-1)&PAGE_MASK) != ((addr)&PAGE_MASK))|| buf == NULL){
		return 1;
	}
	addr += APP_START_ADDR;
#ifndef RELEASE
	printf("write addr = %08x\r\n", addr);
#endif
	QSPI_Init();
	QSPI_WriteFlashData(addr,len,buf);
#ifndef RELEASE
	printf("write end\r\n");
#endif
	return 0;
}

