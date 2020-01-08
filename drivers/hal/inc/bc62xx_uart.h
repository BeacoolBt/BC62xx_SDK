/** 
  *@file uart.h
  *@brief uart support for application
  */
#ifndef _BC62XX_UART_H_
#define _BC62XX_UART_H_

#include "common.h"

#include "bc62xx_gpio.h"
#define UART_CLK_24M (24000000)
#define UART_CLK_48M (48000000)


/**  *@brief Baudrate definition.  */
#define BAUD1200        ((UART_CLK_24M/1200)|0x8000)//(0x804F) /*!< 1200 baud  */
#define BAUD2400        ((UART_CLK_24M/2400)|0x8000)//(0x809D) /*!< 2400 baud  */
#define BAUD4800        ((UART_CLK_24M/4800)|0x8000)//(0x813B) /*!< 4800 baud  */
#define BAUD9600        ((UART_CLK_24M/9600)|0x8000)//(0x8275) /*!< 9600 baud  */
#define BAUD14400       ((UART_CLK_24M/14400)|0x8000)//(0x83AF) /*!< 14400 baud */
#define BAUD19200       ((UART_CLK_24M/19200)|0x8000)//(0x84EA) /*!< 19200 baud */
#define BAUD28800       ((UART_CLK_24M/28800)|0x8000)//(0x875C) /*!< 28800 baud */
#define BAUD31250       ((UART_CLK_24M/31250)|0x8000)//(0x8800) /*!< 31250 baud */
#define BAUD38400       ((UART_CLK_24M/38400|0x8000))//(0x89D0) /*!< 38400 baud */
#define BAUD56000       ((UART_CLK_24M/56000|0x8000))//(0x8E50) /*!< 56000 baud */
#define BAUD57600       ((UART_CLK_24M/57600)|0x8000)//(0x8EB0) /*!< 57600 baud */
#define BAUD76800       (0x83A9) /*!< 76800 baud */
#define BAUD115200      ((UART_CLK_48M/115200)|0x8000)//(0x81A0) /*!< 115200 baud */
#define BAUD230400      (0x803B) /*!< 230400 baud */
#define BAUD250000      (0x8040) /*!< 250000 baud */
#define BAUD460800      ((UART_CLK_48M/460800)|0x8000)//(0x8074) /*!< 460800 baud */
#define BAUD921600      ((UART_CLK_48M/921600)|0x8000)//(0x8034) /*!< 921600 baud */
#define BAUD1M          (0x9000) /*!< 1Mega baud */


/**  *@brief USART_Word_Length.  */ 
#define DATA_BITS_8b                  (0<<2)
#define DATA_BITS_9b                  (1<<2)

/**  *@brief USART_Stop_Bits.   */
#define STOP_BITS_1                 (0<<3)
#define STOP_BITS_2                 (1<<3)
#define IS_USART_STOPBITS(STOPBITS) (((STOPBITS) == STOP_BITS_1) ||  \
									((STOPBITS) == STOP_BITS_2) )

/**  *@brief USART_Parity.  */ 
#define PARITY_EVEN					(0<<1)
#define PARITY_ODD					(1 << 1) 
#define IS_USART_PARITY(PARITY)		(((PARITY) == PARITY_EVEN) || \
                                 	((PARITY) == PARITY_ODD))

/**  *@brief USART_Mode.   */
#define MODE_SINGLE					(1<<6)
#define MODE_DUPLEX					(0<<6)
#define IS_USART_MODE(MODE) 		(((MODE) == MODE_SINGLE) ||\
									((MODE) == MODE_DUPLEX))

/**  *@brief USART_Hardware_Flow_Control.   */ 
#define FLOW_CTRL_NONE				(0<<4)
#define FLOW_CTRL_ENABLE			(1<<4)

/**   *@brief  USART channel define.  */ 
typedef enum{
	UARTA = 0,
	UARTB,
	UARTN,
}USART_TypeDef;

#define IS_USARTAB(USARTx)			((USARTx == UARTA)||\
									(USARTx == UARTB))

/** 
  *@brief USART initialized Structure definition.  
  */ 
typedef struct
{
  uint32_t USART_BaudRate;            	/*!< This member configures the USART communication baud rate.
                                           The baud rate is computed using the following formula:
                                            -USART_BaudRate = (48M/baudrate)|(1<<31) */

  uint16_t USART_WordLength;          	/*!< Specifies the number of data bits transmitted or received in a frame.
                                           This parameter can be a value of @ref USART_Word_Length */

  uint16_t USART_StopBits;            	/*!< Specifies the number of stop bits transmitted.
                                           This parameter can be a value of @ref USART_Stop_Bits */

  uint16_t USART_Parity;              	/*!< Specifies the parity mode.
                                           This parameter can be a value of @ref USART_Parity
                                           @note When parity is enabled, the computed parity is inserted
                                                 at the MSB position of the transmitted data (9th bit when
                                                 the word length is set to 9 data bits; 8th bit when the
                                                 word length is set to 8 data bits). */
 
  uint16_t USART_Mode;                	/*!< Specifies wether the Receive or Transmit mode is enabled or disabled.
                                           This parameter can be a value of @ref USART_Mode */

  uint16_t USART_HardwareFlowControl; 	/*!< Specifies wether the hardware flow control mode is enabled
                                           or disabled.
                                           This parameter can be a value of @ref USART_Hardware_Flow_Control */
                                           
  uint16_t USART_TXLen; 				/*!< Specifies Tx DMA buff length */
  
  uint16_t USART_RXLen; 				/*!< Specifies Rx DMA buff length */
} USART_InitTypeDef;

/**
  *@brief Uart initialization.
  *@param USARTx USART channel select.
  *@param USART_InitStruct Pointer of initializing uart structure.
  *@return None.
  */
void USART_Init(USART_TypeDef USARTx, USART_InitTypeDef* USART_InitStruct);

/**
  *@brief Cancle uart initialization.
  *@param USARTx USART channel select.
  *@return None.
  */
void USART_DeInit(USART_TypeDef USARTx);

/**
  *@brief Sending a byte data by UART.
  *@param USARTx USART channel select.
  *@param Data The data to be send.
  *@return None.
  */
void USART_SendData(USART_TypeDef USARTx, uint16_t Data);

/**
  *@brief Receive a byte data from UART.
  *@param USARTx USART channel select.
  *@returns A 16-bit data what read from UART . 
  */
uint16_t USART_ReceiveData(USART_TypeDef USARTx);

/**
  *@brief Gets the position of the read pointer of UART.
  *@param USARTx USART channel select.
  *@returns The current read pointer position of the UART.
  */
uint16_t USART_GetRxCount(USART_TypeDef USARTx);

/**
  *@brief Receive data from UART.
  *@param USARTx USART channel select.
  *@param RxBuff The pointer that stores data read from the UART.
  *@param RxSize The number of byte read from UART.
  *@retval 0 The length of the read data is not enought of does not read the data;
  *@retval non-zero The actual number of byte read from UART.
  */
uint16_t USART_ReadDatatoBuff(USART_TypeDef USARTx, uint8_t* RxBuff, uint16_t RxSize);

/**
  *@brief Sending data by UART.
  *@param USARTx USART channel select.
  *@param TxBuff The pointer of the data to be send.
  *@param TxLen The number of byte of the data to be send.
  *@return The actual number of byte send to UART.
  */
uint16_t USART_SendDataFromBuff(USART_TypeDef USARTx, uint8_t* TxBuff, uint16_t TxLen);


/**
  *@brief Remap Uart To Printf.
  *Default uart parameter :BAUD921600, DATA_BITS_8b,STOP_BITS_1,
  *PARITY_EVEN,MODE_DUPLEX,FLOW_CTRL_NONE,TX_LEN[128], RX_LEN[128]
  *@return 0 Success, 1 Failed.
  */
int UART_RemapToPrintf(USART_TypeDef USARTx, uint8_t txPin, uint8_t rxPin);

void UART_PutChar(char c);

#endif/*_BC62XX_UART_H_*/


