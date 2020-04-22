#ifndef __PROTOCOLRLN_H__
#define __PROTOCOLRLN_H__

#include "common.h"
/**
 * Resolution callback function
 */
typedef void (*prorocol_rln_cb)(const uint8_t* data, uint8_t len);

/**
 * Resolution callback function
 */
typedef void (*prorocol_send_cb)(const uint8_t* data, uint8_t len);


/**
 ****************************************************************************************
 * @brief protocol call back function
 *
 * @param[in] cb  Resolution callback function
 *
 * @return None
 ****************************************************************************************
 */
	void protocol_init(prorocol_rln_cb rcb, prorocol_send_cb scb);

/**
 ****************************************************************************************
 * @brief Encapsulated data frame
 *
 * @param[in] data  send payload data buff
 * @param[in] len  length of data
 *
 * @return None
 ****************************************************************************************
 */
void protocol_send(const uint8_t* data, uint8_t len);

/**
 ****************************************************************************************
 * @brief send data to protocol process
 *
 * @param[in] data  received data buff
 * @param[in] len  length of recv data
 *
 * @return None
 ****************************************************************************************
 */
void protocol_recv(uint8_t* data, uint8_t len);

#endif/*__PROTOCOLRLN_H__*/
