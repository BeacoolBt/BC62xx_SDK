#include "gatt_ota.h"
#include "co_com.h"

#include "bc62xx_flash.h"
#include "bc62xx_timer.h"

#include "ali_config.h"

/*the period set Gatt Ota adv data set to 10S*/
//#define GATT_OTA_PERIOD_NUM		(1000)//

/*****************OTA DEFINE****************/
#define CMD_OTA_START	(0xc0)
#define CMD_OTA_DATA 	(0xc1)
#define CMD_OTA_END		(0xc2)
#define CMD_OTA_RESET	(0xff)

#define PACKETNUM 		(0x21c)

#define APP_ONE 	(1)
#define APP_TWO 	(2)

#define READ_UINT 256
/*****************OTA DEFINE****************/

uint8_t StartOtaFlag = OTANONE;
static uint16_t data_start_flag = 0;
static uint16_t data_retransmit_flag = 0;

static uint16_t cur_packet_id = 0xFFFF;

/*used to period set Gatt Ota adv data*/
//static SYS_TIMER_TYPE stGattOtaAdvTimer;//peroid timer	

void gatt_ota_start(void)
{
	//stop adv timer
//	SYS_ReleaseTimer(&stGattOtaAdvTimer);
	
	StartOtaFlag = NOSTART;
	IPC_TxControlCmd(IPC_CMD_MTU_EXCHANGE);
	HWRITE(mem_le_scan_enable,0x00);//stop scan
	HWRITE(mem_le_adv_enable,0x00);//stop adv

	IPC_TxControlCmd(IPC_CMD_UPDATA_CONN);
}

void gatt_ota_stop(void)
{
	StartOtaFlag = OTANONE;
	HWRITE(mem_le_scan_enable,0x01);//enable scan
	//start adv timer
	gatt_ota_init();
}

uint8_t gatt_ota_state_get(void)
{
	return StartOtaFlag;
}

/**
  *@breaf Get flash program start addr.
  *@param None.
  *@@return 1:running app one
			2:running app two
  */
uint8_t ota_prog_addr()
{
	uint8_t saddr[3]; 
	uint32_t uintStart;
	QSPI_Init();
	QSPI_ReadFlashData(0,3,saddr);//从flash的特定地址读出数据
	uintStart = ((saddr[0] << 16) + (saddr[1] << 8) + saddr[2]);
	//M_PRINTF(L_APP, "uintStart[%x]", uintStart);
	if(uintStart == CODE_UPGRADE_START_ADDR+PAGE_SIZE+3){
		return APP_TWO;
	}
	return APP_ONE;
}

int ota_crc16_ccitt(int crc, int c)
{
	crc  = (crc >> 8) | (crc << 8);
	crc ^= c & 0xff;	
	crc ^= (crc & 0xff) >> 4;	
	crc ^= crc << 12;	
	crc ^= (crc & 0xff) << 5;	
	crc &= 0xffff;	
	return crc;
}

uint16_t ota_calc_crc(uint32_t flashaddr,uint32_t len)
{
	int i,j,crc;
	uint8_t readbyte[256];
	uint32_t readlen = 0;
	uint32_t readtimes = (len+READ_UINT-1)/READ_UINT;
	crc=0xffff;
	for(i=0;i< readtimes; i++){
		readlen = len > READ_UINT ? READ_UINT : len;

		QSPI_Init();
		QSPI_ReadFlashData(flashaddr,readlen,readbyte);
		flashaddr += readlen;
		len -= readlen;
		for(j=0;j<readlen;j++) {
			crc = ota_crc16_ccitt(crc,*(readbyte+j));
		}
	}
	return crc;
}

void ota_prog_switch()
{
	uint8_t bootaddr[3]; 
	uint32_t startAddr;
	if(ota_prog_addr()==APP_TWO){
		startAddr = CODE_START_ADDR+PAGE_SIZE+3;
		
	}else{
		startAddr = CODE_UPGRADE_START_ADDR+PAGE_SIZE+3;
	}
	bootaddr[0] = (startAddr>>16)&0xff;
	bootaddr[1] = (startAddr>> 8)&0xff;
	bootaddr[2] = (startAddr>> 0)&0xff;
	//m_printf_hex(L_APP, "ota_prog_switch", bootaddr, 3);
	QSPI_Init();
	QSPI_PageEraseFlash(0x00);
	//QSPI_SectorEraseFlash(0x00);
	QSPI_WriteFlashData(0x00,3,bootaddr);
	HW_REG_8BIT(reg_map(CORE_RESET),0x03);
}

/**
  *@breaf check out flash.dat code crc.
  *@param None.
  *@return !0: fail.
			0: pass.
*/
int8_t ota_code_crc(uint32_t len)
{
	uint32_t flashaddr,flashaddrS,varaddr;
	uint8_t readbuf[4];
	uint32_t length = 0;
	uint16_t crc;
	//step1:get app flashaddr	
	if(ota_prog_addr() == APP_TWO){ //check out addr 0x1000 code
		flashaddrS = CODE_START_ADDR+PAGE_SIZE+3;//0x2003;
	}
	else{
		flashaddrS = CODE_UPGRADE_START_ADDR+PAGE_SIZE+3;//0x40003;
	}
	flashaddr = flashaddrS;

	//step2:get 0xaa55 and length	
	QSPI_Init();
	QSPI_ReadFlashData(flashaddr,4,readbuf);
	if(!(*readbuf == 0xaa && *(readbuf+1) == 0x55))
		return -1;
	length |= *(readbuf+2);
	length |= *(readbuf+3)<<8;
	flashaddr += 4;

	//step3:get bt code
	crc = ota_calc_crc(flashaddr,length-2);	
	flashaddr += length;

	//step4:compare to bt code crc
	QSPI_Init();
	QSPI_ReadFlashData(flashaddr-2,2,readbuf);

	if(!(*(readbuf+1) == (crc&0xff) && *readbuf == ((crc>>8)&0xff)))
		return -2;

	//step5:skip variable data
	while(1){
		length = 0;
		QSPI_Init();
		QSPI_ReadFlashData(flashaddr,4,readbuf);
		if(!(*readbuf == 0xaa && *(readbuf+1) == 0x55))
			return -3;
		length |= *(readbuf+2);
		length |= *(readbuf+3)<<8;

		flashaddr = flashaddr+length+6;
		QSPI_Init();
		QSPI_ReadFlashData(flashaddr,1,readbuf);
		if(*readbuf == 0xff)
		{	
			QSPI_ReadFlashData(flashaddr-3,3,readbuf);
			varaddr  = *(readbuf);
			varaddr |= *(readbuf+1) << 8;
			varaddr |= *(readbuf+2) << 24;
			break;	
		}
	}
	flashaddr++;
	//step6:get m0 code
	QSPI_Init();
	QSPI_ReadFlashData(flashaddr,4,readbuf);
	if(!(*readbuf == 0xaa && *(readbuf+1) == 0x55))
		return -5;
	length = 0;
	length |= *(readbuf+2);
	length |= *(readbuf+3)<<8;
	flashaddr += 4;
	
	if(((len - varaddr - 2)&0x00ffff) != length){
		return -6;
	}
	crc = ota_calc_crc(flashaddr,(len-varaddr-4));
	//flashaddr += length;
	flashaddr += (len-varaddr-2);
	//step7: compare m0 code crc
	QSPI_Init();
	QSPI_ReadFlashData(flashaddr-2,2,readbuf);

	if(!(*(readbuf+1) == (crc&0xff) && *readbuf == ((crc>>8)&0xff)))
		return -7;

	//step8: calc all crc
	length = flashaddr - flashaddrS; 
	flashaddr = flashaddrS;

	crc = ota_calc_crc( flashaddr, length);
	flashaddr += length;
	
	//step9: compare all code crc
	printf("last crc addr= %lx\n",flashaddr);
	QSPI_Init();
	QSPI_ReadFlashData(flashaddr,2,readbuf);
	
	if(!(*(readbuf+1) == (crc&0xff) && *readbuf == ((crc>>8)&0xff)))
		return -8;

	return 0;
}


void Ble_SendDataSimple(uint16_t handle, uint8_t* data, uint16_t len)//eg handle = 0x0008
{	
	int cnt;
	if(len<0 || len>20)
		return;
	uint8_t BleSendData[30]={0};
	IPC_DATA_FORMAT* temp = (IPC_DATA_FORMAT*)BleSendData;
	temp->ipctype =IPC_BLE_DATA;
	temp->len = len+2;
	temp->ipcUnion.uBleData.mhandle = handle;
	unsigned char *dataAy = (unsigned char *)&temp->ipcUnion.uBleData.data;

	//	memcpy(dataAy,data,len);
	for(cnt=0; cnt<len+2; cnt++)
		*(dataAy+cnt) = *(data+cnt);
		
	IPC_TxPacket(temp);
}

/*********************************************************************
 *************************** OTA *************************************
 *********************************************************************/
/**
 **@breaf: wrtie flash with check out
 **@param: falshaddr 
 ** param: len
 **@return 0:write success.
 ** return !0:write failed.
**/
static uint32_t otaErasePage;
static uint32_t writeAddr;
uint8_t OTA_flash_init(uint32_t startAdd)
{
	otaErasePage = startAdd&PAGE_MASK;
	writeAddr = startAdd;
	QSPI_Init();
	QSPI_PageEraseFlash(otaErasePage);
	return 0;
}

uint8_t OTA_WriteFlash(uint32_t flashaddr, uint32_t len, uint8_t* data)
{
	uint8_t tbuf[PACKETNUM];
	//QSPI_WriteFlashDatabyPage(flashaddr,len,data);
	//QSPI_ReadFlashData(flashaddr,len,tbuf);
	//for(int i =0;i<len;i++){
	//	if(data[i]!=tbuf[i])
	//		return -1;
	//}
	//erase reserve byte of page
	//M_PRINTF(L_APP, "flashaddr[%x] otaErasePage[%x] writeAddr[%x]", flashaddr, otaErasePage, writeAddr);
	if(writeAddr > flashaddr){//write back
		otaErasePage = flashaddr&PAGE_MASK;
		int tlen = flashaddr&(PAGE_SIZE-1);
		QSPI_Init();
		QSPI_ReadFlashData(otaErasePage, tlen,tbuf);
		QSPI_PageEraseFlash(otaErasePage);
		QSPI_WriteFlashData(otaErasePage,tlen,tbuf);
		m_printf(L_APP, "flashaddr[%x] otaErasePage[%x] tlen[%x]", flashaddr, otaErasePage, tlen);
	}
	if(otaErasePage+PAGE_SIZE < flashaddr+len){//forward
		//erase new page
		otaErasePage = (flashaddr+len)&PAGE_MASK;
		QSPI_Init();
		QSPI_PageEraseFlash(otaErasePage);
		//M_PRINTF(L_APP, "2");
	}else 
	//M_PRINTF(L_APP, "3");

	QSPI_Init();
	//write data
	QSPI_WriteFlashData(flashaddr,len,data);
	//read and compare
	QSPI_ReadFlashData(flashaddr,len,tbuf);
	writeAddr = flashaddr+len;
	for(int i=0; i<len; i++){														//将需要修改数据copy到pagebuf中
		if(data[i] != tbuf[i])
			return 1;
	}
	
	return 0;
}

void ota_cmd_to_app(uint8_t cType, uint16_t pTemp, uint8_t CheckResult)
{
	uint8_t cbuf[5];
	cbuf[0] = 0xab;
	cbuf[1] = cType;
	cbuf[2] = pTemp & 0xff; 	//pTemp:00 01-->01 00
	cbuf[3] = (pTemp>>8) & 0xff;
	switch(cType){
		case CMD_OTA_START:
			Ble_SendDataSimple(OTA_INDICATE_HANDLE,cbuf,4);
			break;
		case CMD_OTA_DATA:
			Ble_SendDataSimple(OTA_INDICATE_HANDLE,cbuf,4);
			break;
		case CMD_OTA_END:
			cbuf[4] = CheckResult;
			Ble_SendDataSimple(OTA_INDICATE_HANDLE,cbuf,5);
			break;
	}
}

void gatt_ota_data_deal(uint8_t* dat, uint8_t length)
{
	static uint32_t OTAStartAddr;
	static uint32_t flashaddrsaved;
	static uint16_t PacketId = 0;
	static uint32_t cflashaddr = 0;
	static uint32_t cSum=0;
	static uint32_t packetSum=0;
	static uint16_t PacketLength = 0;
	static uint16_t CurPacketLen = 0;
	uint8_t* dataTmp = dat;
	//m_printf_hex(L_APP, "ota data recv", dat, length);
	if(dat[0] == 0xba && dat[1] == CMD_OTA_DATA 
		&& dat[2] == (cur_packet_id&0xFF) 
		&& dat[3] == (cur_packet_id >> 8&0xFF)){
		data_retransmit_flag = 1;		
	}
	
	if(*dat++ == 0xba){
		switch(*(dat++)){
			case CMD_OTA_START://oxba,oxc0
				if(StartOtaFlag == NOSTART){	//收到start的包后，确定OTA地址和置标志位。				
					StartOtaFlag = OTASTART;
					
					uint8_t sts = ota_prog_addr();
					if(sts == APP_TWO){ //check out addr 0x1000 code
						OTAStartAddr = CODE_START_ADDR+PAGE_SIZE;//0x2000;
					}
					else{
						OTAStartAddr = CODE_UPGRADE_START_ADDR+PAGE_SIZE;//0x40000;
					}
					cflashaddr = OTAStartAddr;
					cSum = 0;
					packetSum = 0;
					//M_PRINTF(L_APP, "cflashaddr[%x]", cflashaddr);
					IPC_TxControlCmd(IPC_CMD_UPDATA_CONN);
					ota_cmd_to_app(CMD_OTA_START,PACKETNUM,0);//CMD_OTA_START_H
					OTA_flash_init(OTAStartAddr);
				} 
				break;
			case CMD_OTA_DATA:
				if(StartOtaFlag == OTASTART){
					StartOtaFlag = OTAPROCESSING;
				}
				if(StartOtaFlag != OTAPROCESSING)
					return;
				PacketId = *(dat++);
				PacketId |= *(dat++)<<8;
				PacketLength = *(dat++);
				PacketLength |= *(dat++)<<8;

				if(data_retransmit_flag == 1){
					data_retransmit_flag = 0;
					cflashaddr = flashaddrsaved;
					packetSum = 0;
				}
				else{
					flashaddrsaved = cflashaddr;
					
				}
				cSum += packetSum;
				//M_PRINTF(L_APP, "PacketId[%x] PacketLength[%x] cflashaddr[%x]", PacketId, PacketLength, cflashaddr);
				cur_packet_id = PacketId; 
				CurPacketLen = 0;
				data_start_flag = 1;
				dataTmp = dat;
				length = length - 6;
				packetSum = 0;
				break;
			case CMD_OTA_END:
				cSum += packetSum;
				if(StartOtaFlag != OTAPROCESSING)
					return;
				uint32_t mCheckSum;
				uint32_t CheckSum;
				PacketId = *(dat++);
				PacketId |= *(dat++)<<8;
				CheckSum = *(dat++);
				CheckSum |= *(dat++)<<8;
				CheckSum |= *(dat++)<<16;
				mCheckSum = cflashaddr - OTAStartAddr;
				
				m_printf(L_APP, "CheckSum[%x] cSum[%x]", CheckSum, cSum);
				if(CheckSum == cSum){
					int8_t status = ota_code_crc(mCheckSum);
					m_printf(L_APP, "code crc status[%d]", status);
					if(!status){
						ota_cmd_to_app(CMD_OTA_END,PacketId,0x55);
						uint32_t count = 100000;
						while(count--)
							IPC_DealSingleStep();
						ota_prog_switch();
					}
					else{
						StartOtaFlag = NOSTART;
						ota_cmd_to_app(CMD_OTA_END,PacketId,0);
					}	
				}else{
					StartOtaFlag = NOSTART;
					ota_cmd_to_app(CMD_OTA_END,PacketId,0);
				}
		
				break;
			case CMD_OTA_RESET:
				if(StartOtaFlag != OTAPROCESSING)
					return;
				ota_prog_switch();
				StartOtaFlag = NOSTART;
				break; 
			default:
				break;
		
		}
	}
	if(data_start_flag == 1 && StartOtaFlag == OTAPROCESSING){
		//calc check sum
		for(int i = 0; i < length;  i++){	
			packetSum += dataTmp[i];
		}
		//write flash data
		uint8_t writeStatus = 0;
		writeStatus = OTA_WriteFlash(cflashaddr,(uint32_t)length, dataTmp);
		//m_printf_hex(L_APP, "ota data crc", dataTmp, length);
		//M_PRINTF(L_APP, "writeStatus[%x]", writeStatus);
		cflashaddr += length;
		CurPacketLen += length;
		if(writeStatus == 0){
			if(CurPacketLen == PacketLength){
				data_start_flag = 0;
				ota_cmd_to_app(CMD_OTA_DATA,PacketId,0);//CMD_OTA_DATA_H
			}
		}
		else{
			//indicate failed
			//ota_cmd_to_app(CMD_OTA_DATA,PacketId,0);//CMD_OTA_DATA_H
		}
		
		//M_PRINTF(L_APP, "writeStatus[%d] CurPacketLen[%x] PacketLength[%x]", writeStatus, CurPacketLen, PacketLength);
		return;
	}
}

//static void _gatt_ota_adv_period_set(void *t)
//{
//	M_PRINTF(L_APP, "");
//	
//	/*Assemble adv packet with adv struct*/
//	/*adv data 02 01 05 07 09 5a 58 2d 4f 54 41 ff BC6218 mac ver*/
//	uint8_t data[13+6+6+5] = {0x02, 0x01, 0x05, 0x07, 0x09, 0x5a, 0x58, 0x2d, 0x4f, 0x54, 0x41, 0x0c+6, 0xff, 'B', 'C', '6', '2', '1', '8'};
//	ali_config_mac_read(data+19, 0);
//	memcpy(data+25, MESH_VER, strlen(MESH_VER));
//
//	HWRITE(mem_le_adv_type, ADV_TYPE_NOMAL);
//	HW_REG_16BIT(reg_map(mem_le_adv_interval_max), 1600);	
//	memcpy((unsigned char*)reg_map(mem_le_adv_data),data,sizeof(data));
//	HWRITE(mem_le_adv_data_len,sizeof(data));
//	if(!HREAD(mem_le_adv_enable)){
//		HWRITE(mem_le_adv_enable, 0x01);
//	}
//	
//	/*send adv data*/
//	//BLE_SendAdvData(ADV_TYPE_NOMAL, 1600, 4, sizeof(data), data);
//}

void gatt_ota_init(void)
{
	//set adv resp data
	uint8_t _data[] = {0x07, 0x09, 0x5a, 0x58, 0x2d, 0x4f, 0x54, 0x41, 0x0c+6, 0xff, 'B', 'C', '6', '2', '1', '8'};
	memcpy((unsigned char*)reg_map(mem_le_scan_data),_data,sizeof(_data));
	HWRITE(mem_le_scan_data_len,sizeof(_data));
	//start timer for clear
//	SYS_SetTimer(&stGattOtaAdvTimer, GATT_OTA_PERIOD_NUM, 
//					TIMER_CYCLE, (Timer_Expire_CB)_gatt_ota_adv_period_set);
}
void gatt_ota_deinit(void)
{
//	SYS_ReleaseTimer(&stGattOtaAdvTimer);
	HWRITE(mem_le_scan_data_len,0);
}

