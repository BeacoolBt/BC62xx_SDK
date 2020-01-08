/** 
  *@file qspi.h
  *@brief Queued SPI support for application.
  */
#ifndef _BC62XX_FLASH_H_
#define _BC62XX_FLASH_H_

#include "common.h"
//#include "yc_drv_common.h"
//#include "yc_timer.h"
//#include "Drv_debug.h"
#include "flash_config.h"

#define RAM10000  //define if use mram

#ifndef RAM10000

#define RXBUF_ADDR 0x10001fe0 //at least 20 bytes

#endif

/**
  *@brief QSPI register definition.
  */
typedef struct 
{
	uint8_t Ctrl; 		/*!< QSPI configure */
	uint8_t Delay;		/*!< Delay time */
	uint16_t TxLen;		/*!< QSPI transport data length */
	uint16_t TxAddr;	/*!< QSPI transport data address */
	uint16_t RxAddr;	/*!< QSPI receive data address */
	uint16_t RxLen;		/*!< QSPI receive data length */
}QSPI_RegDef;

/**
  *@brief QSPI initialization.
  *@param None.
  *@return None.
  */
void   QSPI_Init();

/**
  *@brief Read the flash, One is less than 0x80.
  *@param flash_addr Flash address.
  *@param len Data length.
  *@param rxbuf Data buffer.
  *@return None.
  */
void __attribute__((noinline,aligned(32)))  QSPI_ReadFlashData(int flash_addr, int len, uint8_t *rxbuf);

/**
  *@brief Read flash RDID.
  *@param rxbuf Receive RDID buffer.
  *@return None.
  */
void __attribute__((noinline,aligned(32)))  QSPI_ReadFlashRDID(uint8_t *rxbuf);

/**
  *@brief Read flash register.
  *@param rxbuf Receive register value buffer.
  *@return None.
  */
void __attribute__((noinline,aligned(32)))  QSPI_ReadFlashReg(uint8_t *rxbuf);

/**
  *@brief Read flash REMS.
  *@param rxbuf Receive REMS buffer. 
  *@return None.
  */
void  __attribute__((noinline,aligned(32)))  QSPI_ReadFlashREMS(uint8_t *rxbuf);

/**
  *@brief Read flash UID.
  *@param rxbuf Receive UID buffer. 
  *@return None.
  */
void __attribute__((noinline,aligned(32)))  QSPI_ReadFlashUID(uint8_t *rxbuf);

/**
  *@brief Write flash.
  *@param flash_addr Flash address where send command to.
  *@param len Write flash data length.
  *@param txdata Write flash data buffer.
  *@return None.
  */
void    __attribute__((noinline,aligned(32))) QSPI_WriteFlashData(int flash_addr, int len, uint8_t *txdata);

/**
  *@brief Write flash after earse per page, the interface writed for PURAN flash.
  *@param flash_addr Flash address where send command to.
  *@param len Write flash data length.
  *@param txdata Write flash data buffer.
  *@return None.
  */
void QSPI_WriteFlashDatabyPage(int flash_addr, int txlen, uint8_t *txdata);

/**
  *@brief Erasure by page ,256B at a time,size range 0~0xFFF.
  *@param flash_addr The flash address will be erasure.
  *@return None.
  */
void __attribute__((noinline,aligned(32))) QSPI_PageEraseFlash(int flash_addr);

/**
  *@brief Enable Quadruple SPI model.
  *@param on is a non-zero positive integer the model on , on is zero represent the model off.
  *@return None.
  */
void   __attribute__((noinline,aligned(32)))  QSPI_FlashQuadEnable(uint8_t on);

/**
  *@brief Enter DEEP POWER DOWN model, can not read and write.
  *@return None.
  */
void  __attribute__((noinline,aligned(32)))  QSPI_FlashDeepPowerDown();
#endif/*_BC62XX_FLASH_H_*/
 

