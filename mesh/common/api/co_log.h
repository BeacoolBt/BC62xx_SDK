#ifndef __CO_LOG_H__
#define __CO_LOG_H__

#include "stdio.h"
#include "string.h"
#include "co_com.h"

uint32_t L_LEV;

#define L_AL		(0x01<<0)
#define L_TB		(0x01<<1)
#define L_BEAR		(0x01<<2)
#define L_PROV		(0x01<<3)
#define L_NET		(0x01<<4)
#define L_BCN		(0x01<<5)
#define L_LTARNS 	(0x01<<6)
#define L_LPN		(0x01<<7)
#define L_UTARNS 	(0x01<<8)
#define L_HB 		(0x01<<9)
#define L_ACCESS 	(0x01<<10)
#define L_FND		(0x01<<11)
#define L_MODEL		(0x01<<12)
#define L_APP 		(0x01<<13)

static const unsigned char lgs[] = "[MLG]";
static const unsigned char lge[] = "\r\n";

#define FILE_NAME (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)

int m_print(const char *fmt, ...);

int m_printf(uint32_t lev, const char* string,...);

int m_printf_hex(uint32_t lev, const char* str, const uint8_t* data, int len);

#ifdef MESH_RELEASE
#define M_PRINTF(lev, ...)
#else
#define M_PRINTF(lev, ...)	do{\
								if(!(lev & L_LEV))break;\
								m_print("%s[%10s:%d]%s:", lgs, FILE_NAME,__LINE__, __FUNCTION__);\
								m_print(__VA_ARGS__);\
								m_print("%s",lge);\
							}while(0)	
#endif

void m_set_log_lev(uint8_t l);

void m_log_disable(void);

void m_log_enable(void);
		
#endif/* __CO_LOG_H__ */

