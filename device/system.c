#include <stdio.h>
#include <sys/stat.h>
#include "bc62xx_ble.h"
//#include "Drv_debug.h"

int _fstat (int fd, struct stat *pStat)
{
	pStat->st_mode = S_IFCHR;
	return 0;
}

int _close(int fd)
{
	return -1;
}

extern void UART_PutChar(char c);
int _write (int fd, char *ptr, int len)
{
	int i = 0;

	if (fd > 2)
		return -1;

	while(*ptr && (i < len))\
	{
		UART_PutChar(*ptr);
		i++;
		ptr++;
	}
   
 	return len;
}

int _isatty (int fd)
{
	return 1;
}

int _lseek(int fd, int offset, int fromwhere)
{
	return 0;
}

int _read (int fd, char *pBuffer, int size)
{
	return 0;
}

caddr_t _sbrk(int increment)
{
	extern char end asm("end");
	register char *pStack asm("sp");
	static char *s_pHeapEnd;
	if (!s_pHeapEnd)
		s_pHeapEnd = &end;

	if (s_pHeapEnd + increment > pStack)
		return (caddr_t)-1;

	char *pOldHeapEnd = s_pHeapEnd;
	s_pHeapEnd += increment;
	return (caddr_t)pOldHeapEnd;
}

void _exit(int status)
{
	while(1);
}


void Default_Handler(void)
{
	printf("\r\n\r\nDefault_Handler\r\n\r\n");
#ifdef RELEASE
	HWRITE(CORE_RESET,0x03);//reset cpu
#else
	while(1);//block cpu
#endif
}
#if 0
cpu_first_task_start:
    @set PendSV prority to the lowest
    LDR     R0, =SHPR3_PRI_14_15                   
    LDR     R2, [R0]
    LDR     R1, =SHPR3_PRI_LVL
    ORRS    R2, R1
    STR     R2, [R0]

    @indicate PendSV_Handler branch to _pendsv_handler_nosave
    MOVS    R0, #0
    MSR     PSP, R0

    @make PendSV exception pending
    LDR     R0, =SCB_ICSR
    LDR     R1, =ICSR_PENDSVSET
    STR     R1, [R0]

    @goto PendSV_Handler
    CPSIE   I
    B       .
#endif


