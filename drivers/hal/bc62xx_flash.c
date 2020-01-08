#include "bc62xx_flash.h"

void __attribute__((noinline))   FillSendCMD(uint8_t *buf, uint8_t cmd, int flash_addr);
void __attribute__((noinline,aligned(32)))  ReadFlashSPI(uint8_t *txdata,int txlen,uint8_t *rxbuf,int rxlen);
void  __attribute__((noinline,aligned(32)))  FlashWriteEnable();
void __attribute__((noinline,aligned(32)))   WaitFlashFinish();

#ifdef RAM10000
//static byte qspi_txbuf[60]={0};
//static byte readreg[2] =  {0,0};
#else
  uint8_t *readreg = (uint8_t *)(RXBUF_ADDR); // 2 byte
  uint8_t *temp2 = (uint8_t *)(RXBUF_ADDR+2); 
  uint8_t *temp = (uint8_t *)(RXBUF_ADDR+6); 
#endif


void __attribute__((noinline)) Start()
{
	__ASM("nop");
}

/*
void HWRITE_T(int reg,byte val)
{
//	do {
	HREAD(reg) = val;
//	while(n--);
//	}
//	while(0)	
	__ASM("nop");
	__ASM("nop");
	__ASM("nop");
	__ASM("nop");
//	__ASM("nop");
}

void HWRITEW_T(int reg,int val)
{
//	do {
	HWRITE_T(reg, (byte)(val));
	HWRITE_T(reg + 1, (byte)((int)(val) >> 8));
//	}
//	while(0)
}

void   QSPI_Delay(volatile int cnt)
{
	while(cnt--);
}
*/

void __attribute__((noinline))   FillSendCMD(uint8_t *buf, uint8_t cmd, int flash_addr)
{
	//uint8_t j;
	buf[0] = cmd;
	//for(j = 0;j < 3;j++);
//	QSPI_Delay(1);
	buf[1] = flash_addr >> 16;
	//for(j = 0;j < 3;j++);
//	QSPI_Delay(1);
	buf[2] = flash_addr >> 8;
	//for(j = 0;j < 3;j++);
//	QSPI_Delay(1);
	buf[3] = flash_addr;
}

/*
void __attribute__((noinline,aligned(32))) QSPI_ReadFlashQSPI(int flash_addr, int rxlen, uint8_t *rxbuf)
{
//	int addr;
	volatile int i, j;
//	byte *cmd = (byte *)0x10000b90;
	static uint8_t rxbuf_t[300]={0};
	static uint8_t cmd[4]  = {0};
	uint8_t mode, delay;
	FillSendCMD(cmd, 0x6b, flash_addr);
	mode = HREAD(CORE_QSPI_CTRL);
	delay = HREAD(CORE_QSPI_DELAY);
//	for(i = 0;i < 2;i++) {
//		addr = i == 0 ? 0xc000 : 0;
//		addr =  0;
		HWRITE(CORE_QSPI_CTRL  , 0x46);//mode&0xb8 |0x05
		HWRITE(CORE_QSPI_DELAY, 8);
		HWRITEW(CORE_QSPI_TXADDR , (int)cmd);
		HWRITEW(CORE_QSPI_TXLEN , 4);
		HWRITEW(CORE_QSPI_RXADDR , (int)rxbuf_t);
		HWRITEW(CORE_QSPI_RXLEN , rxlen);
		HWRITE(CORE_DMA_START , 8);
		for(j = 0;j <20;j++);
//		QSPI_Delay(20);
//		while(!(HREAD(CORE_DMA_STATUS) & 8) && i == 1);
		while(!(HREAD(CORE_DMA_STATUS) & 8) );
		HWRITE(CORE_QSPI_CTRL, mode);
		HWRITE(CORE_QSPI_DELAY , delay);
//	}
	for(i=0;i<rxlen;i++)
		rxbuf[i] = rxbuf_t[i];
}

void __attribute__((noinline,aligned(32))) QSPI_ReadFlashFastQSPI(int flash_addr, int rxlen, uint8_t *rxbuf)
{
//	int addr;
	volatile int i, j;
//	byte *cmd = (byte *)0x10000b90;
//	byte cmd[4]  = {0};
	uint8_t mode, delay;
	FillSendCMD(qspi_txbuf, 0xeb, flash_addr);
	mode = HREAD(CORE_QSPI_CTRL);
	delay = HREAD(CORE_QSPI_DELAY);
//	for(i = 0;i < 2;i++) {
//		addr = i == 0 ? 0xc000 : 0;
//		addr =  0;
#ifdef RAM10000
		HWRITE(CORE_QSPI_CTRL  , 0x46);//mode&0xb8 |0x05
#else
		HWRITE(CORE_QSPI_CTRL  , 0x06);//mode&0xb8 |0x05
#endif
		HWRITE(CORE_QSPI_DELAY, 0x44);
		HWRITEW(CORE_QSPI_TXADDR , (int)qspi_txbuf);
		HWRITEW(CORE_QSPI_TXLEN , 4);
		HWRITEW(CORE_QSPI_RXADDR , (int)rxbuf);
		HWRITEW(CORE_QSPI_RXLEN , rxlen);
		HWRITE(CORE_DMA_START , 8);
		for(j = 0;j <20;j++);
//		QSPI_Delay(20);
//		while(!(HREAD(CORE_DMA_STATUS) & 8) && i == 1);
		while(!(HREAD(CORE_DMA_STATUS) & 8) );
		HWRITE(CORE_QSPI_CTRL, mode);
		HWRITE(CORE_QSPI_DELAY , delay);
//	}
}
*/

void  ReadFlashDSPI(uint8_t cammond,int flash_addr, int rxlen, uint8_t *rxbuf)
{
//	int addr;
//	volatile unsigned int i, j;
	uint8_t mode, delay;
#ifdef RAM10000
	uint8_t temp[4]={0,0,0,0};
#endif
	FillSendCMD(temp, cammond, flash_addr);
	mode = HREAD(CORE_QSPI_CTRL);
	delay = HREAD(CORE_QSPI_DELAY);
#ifdef RAM10000
		HWRITE(CORE_QSPI_CTRL  , 0x45);//mode&0xb8 |0x05
#else
		HWRITE(CORE_QSPI_CTRL  , 0x05);//mode&0xb8 |0x05
#endif
		HWRITE(CORE_QSPI_DELAY, 8);
		HWRITEW(CORE_QSPI_TXADDR , (int)temp);
		HWRITEW(CORE_QSPI_TXLEN , 4);
		HWRITEW(CORE_QSPI_RXADDR , (int)rxbuf);
		HWRITEW(CORE_QSPI_RXLEN , rxlen);
		
		HWRITE(CORE_DMA_START , 8);
		//for(j = 0;j < 20;j++);
		while(!(HREAD(CORE_DMA_STATUS) & 8) );
		HWRITE(CORE_QSPI_CTRL, mode);
		HWRITE(CORE_QSPI_DELAY , delay);
}

void __attribute__((noinline,aligned(32))) QSPI_ReadFlashData(int flash_addr, int len, uint8_t *rxbuf)
{
	OS_ENTER_CRITICAL();
	int i=0,j=0;

	for(i = len,j=0;i>0;i = i -0x40,j = j+0x40){
		if(i>0x40)
			ReadFlashDSPI(0x3b,flash_addr+j,0x40,rxbuf+j);
		else
			ReadFlashDSPI(0x3b,flash_addr+j,i,rxbuf+j);	
	}
	OS_EXIT_CRITICAL();
}

void __attribute__((noinline,aligned(32))) ReadFlashSPI(uint8_t *txdata,int txlen,uint8_t *rxbuf,int rxlen)
{
//	volatile int i, j;
	uint8_t mode, delay;
		mode = HREAD(CORE_QSPI_CTRL);
		delay = HREAD(CORE_QSPI_DELAY);
#ifdef RAM10000
		HWRITE(CORE_QSPI_CTRL , 0x44);// 0x40  mode&0xb8 |0x00
#else
		HWRITE(CORE_QSPI_CTRL , 0x04);// 0x40  mode&0xb8 |0x00
#endif
		HWRITE(CORE_QSPI_DELAY , 0x0);
		HWRITEW(CORE_QSPI_TXADDR ,(int)txdata );
		HWRITEW(CORE_QSPI_TXLEN , txlen);
		HWRITEW(CORE_QSPI_RXADDR , (int)rxbuf);
		HWRITEW(CORE_QSPI_RXLEN , rxlen);
		HWRITE(CORE_DMA_START , 8);
		//for(j = 0;j < 20;j++);
		while(!(HREAD(CORE_DMA_STATUS) & 8) );
		HWRITE(CORE_QSPI_CTRL , mode);
		HWRITE(CORE_QSPI_DELAY , delay);
}

void __attribute__((noinline,aligned(32))) QSPI_ReadFlashRDID(uint8_t *rxbuf)
{
#ifdef RAM10000
	uint8_t temp[1]={0};
#endif
	temp[0] = 0x9f;
	ReadFlashSPI(temp,1,rxbuf,3);
}

void __attribute__((noinline,aligned(32))) QSPI_ReadFlashReg(uint8_t *rxbuf)
{
#ifdef RAM10000
	uint8_t temp2[1]={0};
#endif
	temp2[0] = 0x05;
	ReadFlashSPI(temp2,1,rxbuf,1); //s7-s0
	temp2[0] = 0x35;
	ReadFlashSPI(temp2,1,rxbuf+1,1);	//s15-s8
}

void __attribute__((noinline,aligned(32))) QSPI_ReadFlashREMS(uint8_t *rxbuf)
{
#ifdef RAM10000
	uint8_t temp[4]={0,0,0,0};
#endif
	temp[0] = 0x90;
	ReadFlashSPI(temp,4,rxbuf,2); //s7-s0
}

void __attribute__((noinline,aligned(32))) QSPI_ReadFlashUID(uint8_t *rxbuf)
{
#ifdef RAM10000
	uint8_t temp[5]={0,0,0,0,0};
#endif
	temp[0] = 0x4b;
	ReadFlashSPI(temp,5,rxbuf,8); //s7-s0
}

void  WaitFlashFinish()
{
//	uint8_t j;
#ifdef RAM10000
	uint8_t readreg[2] =  {0,0};
#endif
	do{
		//for(j = 0;j < 40;j++);
		QSPI_ReadFlashReg(readreg);
	}	while((readreg[0]&0x01));
}

void WriteFlashData( int txlen, uint8_t *txdata)
{
	volatile int j;
	uint8_t mode, delay;
	FlashWriteEnable();
	mode = HREAD(CORE_QSPI_CTRL);
	delay = HREAD(CORE_QSPI_DELAY);
#ifdef RAM10000
		HWRITE(CORE_QSPI_CTRL ,0x44);//mode&0xf8 |0x40
#else
		HWRITE(CORE_QSPI_CTRL ,0x04);//mode&0xf8 |0x40
#endif
		HWRITE(CORE_QSPI_DELAY , 0x0);
		HWRITEW(CORE_QSPI_TXADDR , (int)txdata);
		HWRITEW(CORE_QSPI_TXLEN , txlen );
		HWRITEW(CORE_QSPI_RXLEN , 0);
		HWRITE(CORE_DMA_START , 8);
		for(j = 0;j <(txlen*10);j++);
		while(!(HREAD(CORE_DMA_STATUS) & 8) );
		HWRITE(CORE_QSPI_CTRL , mode);
		HWRITE(CORE_QSPI_DELAY , delay);
		WaitFlashFinish();
}

void  __attribute__((noinline,aligned(32))) QSPI_WriteFlashData(int flash_addr, int txlen, uint8_t *txdata)
{
	OS_ENTER_CRITICAL();
	int i=0,j=0,x=0, offset=0;
#ifdef RAM10000
	uint8_t temp[0x10+4]={0};	
#endif
	for(i = txlen,j=0;i>0;i = i -0x10,j = j+0x10){
		
		if(i>0x10){
			offset = (flash_addr+j +0x10)%0x100;
		 	if(offset < 0x10 && offset !=0)
		 	{
				FillSendCMD(temp, 0x02, flash_addr+j);
				for(x = 0;x < 0x10-offset;x++)  
					temp[x+4] = txdata[j+x];
				WriteFlashData(0x10-offset +4,temp);
				
				FillSendCMD(temp, 0x02, flash_addr+j+0x10-offset);
				for(;x < 0x10;x++)  
					temp[x+4-0x10 + offset] = txdata[j+x];
				WriteFlashData(offset+4,temp);
		 	}else
		 		{	
				FillSendCMD(temp, 0x02, flash_addr+j);
				for(x = 0;x < 0x10;x++)  
					temp[x+4] = txdata[j+x];
				WriteFlashData(0x10+4,temp);
				}
		}
		else
		{
			offset = (flash_addr+j +i)%0x100;
		 	if(offset <  i && offset != 0 )
		 	{
				FillSendCMD(temp, 0x02, flash_addr+j);
				for(x = 0;x <i- offset;x++)  
					temp[x+4] = txdata[j+x];
				WriteFlashData(i- offset+4,temp);
				
				FillSendCMD(temp, 0x02, flash_addr+j+i-offset);
				for(;x < i;x++)  
					temp[x+4-i + offset] = txdata[j+x];
				WriteFlashData(offset+4,temp);
		 	}else
			 	{
			 	FillSendCMD(temp, 0x02, flash_addr+j);
				for(x = 0;x < i;x++)  
					temp[x+4] = txdata[j+x];
				WriteFlashData(i+4,temp);
				}
		}
	}
	OS_EXIT_CRITICAL();
}
void __attribute__((noinline,aligned(32))) SendFlashWithoutRx(uint8_t *txdata,int txlen)
{
//	volatile int i, j;
	uint8_t mode, delay;
	mode = HREAD(CORE_QSPI_CTRL);
	delay = HREAD(CORE_QSPI_DELAY);
#ifdef RAM10000
		HWRITE(CORE_QSPI_CTRL,0x44 );// 0x40
#else
		HWRITE(CORE_QSPI_CTRL,0x04 );// 0x40
#endif
		HWRITE(CORE_QSPI_DELAY , 0x0);
		HWRITEW(CORE_QSPI_TXADDR , (int)txdata);
		HWRITEW(CORE_QSPI_TXLEN, txlen);
		HWRITEW(CORE_QSPI_RXLEN , 0);
		HWRITE(CORE_DMA_START , 8);
		//for(j = 0;j < 20;j++);
		while(!(HREAD(CORE_DMA_STATUS) & 8) );
		HWRITE(CORE_QSPI_CTRL , mode);
		HWRITE(CORE_QSPI_DELAY , delay);
		WaitFlashFinish();

}


void  __attribute__((noinline,aligned(32)))  QSPI_FlashDeepPowerDown()
{
#ifdef RAM10000
	uint8_t temp[1]={0};
#endif
	int i;
	temp[0] = 0xb9;
	SendFlashWithoutRx(temp,1);
	for(i=0;i<5000;i++);
}

void  __attribute__((noinline,aligned(32)))  FlashWriteEnable()
{
#ifdef RAM10000
	uint8_t temp2[1]={0};
#endif
	temp2[0] = 0x06;
	SendFlashWithoutRx(temp2,1);
}

void  __attribute__((noinline,aligned(32)))  QSPI_FlashWriteDisable()
{
#ifdef RAM10000
	uint8_t temp[1]={0};
#endif
	temp[0] = 0x04;
	SendFlashWithoutRx(temp,1);
}

void __attribute__((noinline,aligned(32))) QSPI_PageEraseFlash(int flash_addr)
{
	OS_ENTER_CRITICAL();
#ifdef RAM10000
	uint8_t temp[4]={0};
#endif
	FillSendCMD(temp, 0x81, flash_addr);
	FlashWriteEnable();	
	SendFlashWithoutRx(temp,4);
	OS_EXIT_CRITICAL();
}

void   __attribute__((noinline,aligned(32)))  QSPI_WriteFlashReg(uint8_t *txbuf)
{
#ifdef RAM10000
	uint8_t temp[3]={0};
#endif
	temp[0] = 0x01;
	temp[1] = txbuf[0];
	temp[2] = txbuf[1];
	FlashWriteEnable();
	SendFlashWithoutRx(temp,3);	//s15-s8
}

void   __attribute__((noinline,aligned(32)))   QSPI_FlashQuadEnable(uint8_t on)
{
#ifdef RAM10000
	uint8_t temp[2]={0};
#endif
	QSPI_ReadFlashReg(temp);
	if(on)
		temp[1] = temp[1] |0x02;	
	else
		temp[1] = temp[1] &0xfd;
	QSPI_WriteFlashReg(temp);
}


void __attribute__((noinline,aligned(32)))    End()
{
	__ASM("nop");
}

void __attribute__((noinline,aligned(32)))   prefetch(int start_addr, int end_addr)
{
	int addr= 0;
	for(addr = (int)start_addr & (~0x1f);addr < (int)end_addr + 32;addr += 32)
	{
		__ASM("ldr r0,[%0]": : "r"(addr) :"%r0");
//			__ASM("ldr r0,[%0]": : "r"(addr) :);
	}
}

void QSPI_Init()
{
	int a,b;
	a = (int)Start;
	b = (int)End;
	if(a<b)
		prefetch(a,b);
	else
		prefetch(b,a);
}

void _Qspi_WritebyEarsePage(uint32_t flash_addr,uint32_t len,uint8_t* tbuf)
{		
	uint32_t i=0;
	uint8_t pagebuf[PAGE_SIZE] = {0};
	
	memset(pagebuf,0xff,PAGE_SIZE);												//将每个字节都置为ff													

	QSPI_Init();
	QSPI_ReadFlashData((flash_addr/PAGE_SIZE)*PAGE_SIZE,PAGE_SIZE,pagebuf);//从flash中读出不更改的数据到pagebuf

	for(i=0; i<len; i++)														//将需要修改数据copy到pagebuf中
		pagebuf[flash_addr%256 +i] = *(tbuf + i);

	QSPI_Init();
	QSPI_PageEraseFlash(flash_addr);											//将page擦除

	QSPI_Init();
	QSPI_WriteFlashData((flash_addr/PAGE_SIZE)*PAGE_SIZE,PAGE_SIZE,pagebuf);	//将组好的新的pagebuf重新写回flash
}


void QSPI_WriteFlashDatabyPage(int flash_addr, int txlen, uint8_t *txdata)
{
	uint32_t i=0;
//	uint8_t pagebuf[PAGE_SIZE] = {0};
	uint32_t packnum = 0;
	uint32_t ilen = (PAGE_SIZE - flash_addr % PAGE_SIZE);
		
	ilen = ilen>txlen?txlen:ilen;						//确定第一页写的长度

	_Qspi_WritebyEarsePage(flash_addr,ilen,txdata);		//写需要写的第一个page

	flash_addr += ilen;									//写完第一页之后参量偏移
	txdata += ilen;
	txlen -= ilen;

	packnum = (txlen + 255)/PAGE_SIZE; 					//剩余数据还要写的页数

	for(i=0; i<packnum; i++){							//循环写剩余的数据
	
		ilen = txlen>PAGE_SIZE?PAGE_SIZE:txlen;
		_Qspi_WritebyEarsePage(flash_addr,ilen,txdata);

		flash_addr += ilen;
		txdata += ilen;
		txlen -= ilen;
	}
	
}





