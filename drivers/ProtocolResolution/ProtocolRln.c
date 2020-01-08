#include "ProtocolRln.h"
#include "systick.h"
#include "bc_sys.h"
#include "bc62xx_timer.h"

#ifndef RELEASE
#define RELEASE
#endif


#define SIZE_MASK	(0x7F)
#define BUFF_SIZE	(128)
#define FRAME_MIN_TIME	(10)//100ms
#define FRAME_MIN_SIZE	(4)
#define FRAME_MAX_SIZE	(BUFF_SIZE)

#define FRAME_LEN_POS	(2)
#define FRAME_DAT_POS	(3)


typedef struct{
	uint8_t buff[BUFF_SIZE];
	uint16_t head:7;
	uint16_t tail:7;
	uint16_t rsv:2;
	prorocol_rln_cb rCb;
	prorocol_send_cb sCb;
	SYS_TIMER_TYPE dTmr;//delay timer
}proto_t;


const uint8_t frmHead[] = {0x3a, 0xff};
static proto_t proto;

static __INLINE uint8_t _get_recved_len(void)
{
	return proto.head>=proto.tail?
			proto.head-proto.tail:
			BUFF_SIZE - proto.tail + proto.head;
}
static __INLINE void _clear_buff()
{
	proto.tail = 0;
	proto.head = 0; 
}

static __INLINE uint8_t _get_byte()
{
	return proto.buff[proto.tail++];
}
static __INLINE void _set_byte(uint8_t data)
{
	proto.buff[proto.head] = data;
	if(++proto.head == proto.tail){
		proto.tail++;
	}
}

static __INLINE void _updata_pos(uint8_t idx)
{
	proto.tail+=idx;
}

static __INLINE uint8_t _show_byte(uint8_t idx)
{
	return proto.buff[(proto.tail+idx)&SIZE_MASK];
}

static void _delay_timer_cb(int params)
{
#ifndef RELEASE
	printf("clear ring buff");
#endif
	_clear_buff();
}

static void protocol_do(void)
{
	uint8_t _rLen = _get_recved_len();
#ifndef RELEASE
	printf("RECV LEN[%x]\r\n", _rLen);
#endif

	bool _found = true;
	//find head
	for(int j = 0; j < _rLen; ++j){
		for(int i = 0; i < sizeof(frmHead) && i+j<_rLen; ++i){
			if(frmHead[i] != _show_byte(i)){
				_found = false;
			}
		}
		if(true)
			break;
		_get_byte();
	}
#ifndef RELEASE
	printf("find head[%x]\r\n", _found);
#endif
	if(!_found)
		return;

	//check min len
	if(_get_recved_len() < FRAME_MIN_SIZE){
#ifndef RELEASE
		printf("little than FRAME_MIN_SIZE\r\n");
#endif
		return;
	}
	//check all len
	if(_get_recved_len() < FRAME_MIN_SIZE+_show_byte(FRAME_LEN_POS)){
#ifndef RELEASE
		printf("little than FRAME SIZE\r\n");
#endif
		return;
	}
	uint8_t _len = _show_byte(FRAME_LEN_POS);
	uint8_t* _data = NULL;
	if(_len){
		_data = malloc(_len);
		if(!_data){
#ifndef RELEASE
			printf("malloc error\r\n");
#endif
			return;
		}
	}
	_updata_pos(FRAME_DAT_POS);
	for(int i=0; i<_len; ++i){
		_data[i] = _get_byte();
	}
	//check sum
	if(_get_byte() != bc_sys_check_sum(_data, _len)+_len){
#ifndef RELEASE
		printf("check sum error\r\n");
#endif
		return;
	}
	proto.rCb(_data, _len);
	if(_data){
		free(_data);
	}
	if(_get_recved_len() >= FRAME_MIN_SIZE){
#ifndef RELEASE
		printf("redo\r\n");
#endif
		protocol_do();
	}
	
}

void protocol_init(prorocol_rln_cb rcb, prorocol_send_cb scb)
{
	proto.rCb = rcb;
	proto.sCb = scb;
	_clear_buff();
}

void protocol_send(const uint8_t* data, uint8_t len)
{
	if(proto.sCb == NULL)
		return;
	//calc check sum
	uint8_t _sum = bc_sys_check_sum((uint8_t *)data, len)+len;
	//send head
	proto.sCb(frmHead, sizeof(frmHead));
	//send len
	proto.sCb(&len, sizeof(len));
	//send data
	if(data != NULL&& len != 0)
		proto.sCb(data, len);
	//send check sum
	proto.sCb(&_sum, sizeof(_sum));
}

void protocol_recv(uint8_t* data, uint8_t len)
{
	if(proto.rCb == NULL || data == NULL || len == 0)
		return;
	SYS_SetTimer(&proto.dTmr, FRAME_MIN_TIME, TIMER_SINGLE, _delay_timer_cb);
	for(int i = 0; i < len; ++i){
		_set_byte(data[i]);
	}
	if(_get_recved_len() < FRAME_MIN_SIZE){
#ifndef RELEASE
		printf("start timer\r\n");
#endif
		return;
	}
#ifndef RELEASE
	printf("recv and do\r\n");
#endif
	protocol_do();
}



