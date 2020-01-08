#include "ble_adv.h"


void BLE_SetAdvData(uint8_t* Dt, uint16_t Size)
{
	IPC_TxCommon(IPC_MESH_ADV_DATA, Dt, Size);
}

void BLE_SetScanData(uint8_t* Dt, uint16_t Size)
{
	IPC_TxCommon(IPC_MESH_RSP_DATA, Dt, Size);
}

void BLE_StopAdv()
{
	IPC_TxControlCmd(IPC_CMD_STOP_ADV);
}

void BLE_StartAdv()
{
	IPC_TxControlCmd(IPC_CMD_START_ADV);
}

void BLE_StartScan()
{
	IPC_TxControlCmd(IPC_CMD_START_SCAN);
}

void BLE_StopScan()
{
	IPC_TxControlCmd(IPC_CMD_STOP_SCAN);
}


void BLE_SetMacAddr(tBTADDR Addr)
{
	for (uint8_t i=0; i< BT_ADDR_SIZE; i++) {
		HWRITE(mem_le_lap+i, Addr[BT_ADDR_SIZE-1-i]);
	}
}

void BLE_SetAdvType(enum advType Type)
{
	HWRITE(mem_le_adv_type, Type);
}

void BLE_SetScanRspType(enum scanType Type)
{
	HWRITE(mem_le_scan_type, Type);
}

void BLE_SetAdvInterval(uint16_t Intv)
{
	HW_REG_16BIT(reg_map(mem_le_adv_interval_max), Intv);
}

void BLE_SetScanInterval(uint16_t Intv)
{
	HW_REG_16BIT(reg_map(mem_le_scan_interval), Intv);
}
void BLE_SetScanParameter(uint16_t Intv, uint16_t uIntWindow)
{
	HW_REG_16BIT(reg_map(mem_le_scan_interval), Intv);
	
	HW_REG_16BIT(reg_map(mem_le_scan_window), uIntWindow);
}

