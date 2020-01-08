#ifndef __MESH_TIMER_H__
#define __MESH_TIMER_H__

#include "co_com.h"
#include "m_al.h"

uint8_t mesh_timer_init(void);

void mesh_timer_clear(uint16_t type);

void mesh_timer_set(uint16_t type, uint32_t delay);

void mesh_timer_polling();

uint32_t mesh_timer_get_time(void);

void ke_timer_clear(uint16_t type, uint32_t delay);

void ke_timer_set(uint16_t type, uint16_t taskid, uint32_t delay);

uint32_t ke_time(void);

#endif/*__MESH_TIMER_H__*/