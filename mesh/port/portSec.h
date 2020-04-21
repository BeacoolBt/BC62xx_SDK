#ifndef __PORTSEC_H__
#define __PORTSEC_H__

#include "m_al.h"
#include "port.h"
#include "co_com.h"
#include "flash_config.h"
#include "portBle.h"

#if (BLE_MESH)

/// Key length
#define GAP_KEY_LEN           (16)

/// P256 Key Len
#define GAP_P256_KEY_LEN      (0x20)


/// Parameters of the @ref GAPM_USE_ENC_BLOCK_IND message
/*@TRACE*/
struct gapm_use_enc_block_ind
{
    /// Result (16 bytes)
    uint8_t result[GAP_KEY_LEN];
}mesh_use_enc_block_ind;

/// Parameters of the @ref GAPM_GEN_DH_KEY_IND message
/*@TRACE*/
typedef struct gapm_gen_dh_key_ind
{
    /// Result (32 bytes)
    uint8_t result[GAP_P256_KEY_LEN];
}mesh_gen_dh_key_ind;


/// Parameters of the @ref GAPM_PUB_KEY_IND message
/*@TRACE*/
typedef struct gapm_pub_key_ind
{
    /// X coordinate
    uint8_t pub_key_x[GAP_P256_KEY_LEN];
    /// Y coordinate
    uint8_t pub_key_y[GAP_P256_KEY_LEN];
}mesh_pub_key_ind;


/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle result of an AES based algorithm
 *
 * @param[in] status       Execution status
 * @param[in] aes_res      16 bytes block result
 * @param[in] src_info     Information provided by requester
 ****************************************************************************************
 */
typedef void (*mesh_aes_func_result_cb) (uint8_t status, const uint8_t* aes_res, uint32_t src_info);

/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle result of AES-CCM Cipher/Decipher
 *
 * @param[in] mic_error  True if a MIC error detected when Decipher, False else
 *                       In case of MIC error output message is considered invalid
 * @param[in] src_info     Information provided by requester
 ****************************************************************************************
 */
typedef void (*mesh_aes_ccm_func_result_cb) (bool mic_error, uint32_t src_info);

/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle read public key value
 *
 * @param[in] status		 Status of the public key reading
 * @param[in] p_pub_key_x	 32 bytes (LSB) X coordinate of public key
 * @param[in] p_pub_key_y	 32 bytes (LSB) Y coordinate of public key
 ****************************************************************************************
 */
typedef void (*mesh_sec_pub_key_res_cb) (uint16_t status, const uint8_t* p_pub_key_x, const uint8_t* p_pub_key_y);

/**
 ****************************************************************************************
 * @brief Call back definition of the function that can handle result of ECDH Secret computation algorithm
 *
 * @param[in] status		 Status of the ECDH Secret computation
 * @param[in] p_ecdh_secret  32 bytes (LSB) ECDH Secret value
 ****************************************************************************************
 */
typedef void (*mesh_sec_ecdh_secret_res_cb) (uint16_t status, const uint8_t* p_ecdh_secret);


/**
 ****************************************************************************************
 * @brief Initialize AES function management
 *
 * @param reset True if a reset must be perform, False means boot initialization
 ****************************************************************************************
 */
void mesh_sec_aes_init(bool reset);

/**
 ****************************************************************************************
 * @brief Perform an AES encryption - result within callback
 * @param[in] key      Key used for the encryption
 * @param[in] val      Value to encrypt using AES
 * @param[in] copy     Copy parameters because source is destroyed
 * @param[in] res_cb   Function that will handle the AES based result (16 bytes)
 * @param[in] src_info Information used retrieve requester
 ****************************************************************************************
 */
extern void mesh_aes_encrypt(const uint8_t* key, const uint8_t *val, uint8_t* aes_res);
__INLINE_S__ void mesh_sec_aes_encrypt(const uint8_t* key, const uint8_t *val, 
								bool copy, mesh_aes_func_result_cb res_cb, uint32_t src_info)
{
	uint8_t aes_res[16];
	uint8_t tk[16];
	uint8_t tv[16];
	//m_printf_hex(L_AL, "aes key", key, 16);
	//m_printf_hex(L_AL, "aes val", val, 16);
	
	for(int i = 0; i < 16; ++i){
		tk[i] = key[15-i];
	}
	for(int i = 0; i < 16; ++i){
		tv[i] = val[15-i];
	}
	
	mesh_aes_encrypt((const uint8_t*)tk, (const uint8_t *)tv, aes_res);
	
	for(int i = 0; i < 8; ++i){
		aes_res[i] ^= aes_res[15-i];
		aes_res[15-i] ^= aes_res[i];
		aes_res[i] ^= aes_res[15-i];
	}
	//m_printf_hex(L_AL, "aes res", aes_res, 16);

	if(res_cb)	
		res_cb(0, aes_res, src_info);
}

/**
 ****************************************************************************************
 * @brief Generate a random number using AES encryption - result within callback
 *
 * @param[in] res_cb   Function that will handle the AES based result (16 bytes)
 * @param[in] src_info Information used retrieve requester
 ****************************************************************************************
 */
__INLINE_S__ void mesh_sec_aes_rand(mesh_aes_func_result_cb res_cb, uint32_t src_info)
{
	uint8_t aes_res[16];//M_KEY_LEN
	extern void mesh_aes_rand(uint8_t* randbuf,uint8_t randlen);
	mesh_aes_rand(aes_res, sizeof(aes_res));
	//m_printf_hex(L_AL, "aes rand res", aes_res, sizeof(aes_res));
	if(res_cb)
		res_cb(0, aes_res, src_info);
}

/**
 ****************************************************************************************
 * @brief Start the AES CMAC crypto function. Allocate memory for the CMAC and
 *        begins the subkey generation
 *
 * @param[in] key          Pointer to the Key to be used
 * @param[in] message      Pointer to the block of data the data on which the CMAC is performed
 * @param[in] message_len  Length (in bytes) of the block of data M
 * @param[in] res_cb       Function that will handle the AES based result (16 bytes)
 * @param[in] src_info     Information used retrieve requester
 ****************************************************************************************
 */
extern void mesh_aes_cmac(const uint8_t* key, const uint8_t* message, uint16_t message_len,
			  mesh_aes_func_result_cb res_cb, uint32_t src_info);
__INLINE_S__ void mesh_sec_aes_cmac(const uint8_t* key, const uint8_t* message, uint16_t message_len,
              mesh_aes_func_result_cb res_cb, uint32_t src_info)
{
	uint8_t tk[16];
	uint8_t* tm = ke_malloc(message_len, 0);
	
    //m_printf_hex(L_AL, "cmac key val", key, 16);   
	//m_printf_hex(L_AL, "cmac message val", message, message_len);  

	for(int i = 0; i < 16; ++i){
		tk[i] = key[15-i];
	}
	for(int i = 0; i < message_len; ++i){
		tm[i] = message[message_len-1-i];
	}
	mesh_aes_cmac((const uint8_t*)tk,  (const uint8_t*)tm, message_len, res_cb, src_info);
	ke_free(tm);
}
			  
/**
****************************************************************************************
* @brief Start the AES S1 crypto function.
*
* @param[in] message 	 Message used to generate Salted key
* @param[in] message_len  Length (in bytes) of the block of data M
* @param[in] res_cb		 Function that will handle the AES based result (16 bytes)
* @param[in] src_info	 Information used retrieve requester
****************************************************************************************
*/
extern void mesh_aes_s1(const uint8_t* message, uint8_t message_len, uint8_t* aes_res);

__INLINE_S__ void mesh_sec_aes_s1(const uint8_t* message, uint8_t message_len, 
						mesh_aes_func_result_cb res_cb, uint32_t src_info)
{
	uint8_t aes_res[16] = {0};
	uint8_t* tm = ke_malloc(message_len, 0);
	//m_printf_hex(L_AL, "s1 message", message, message_len);
	
	for(int i = 0; i < message_len; ++i){
		tm[i] = message[message_len-1-i];
	}
	mesh_aes_s1((const uint8_t*)tm, message_len, aes_res);

	ke_free(tm);

	for(int i = 0; i < 8; ++i){
		aes_res[i] ^= aes_res[15-i];
		aes_res[15-i] ^= aes_res[i];
		aes_res[i] ^= aes_res[15-i];
	}
	//m_printf_hex(L_AL, "s1 res", aes_res, 16);
	if(res_cb)
		res_cb(0, aes_res, src_info);
	
}

/**
****************************************************************************************
* @brief Start the AES K1 crypto function.
*
* @param[in] salt		 Salted Key to use
* @param[in] n			 Value of N
* @param[in] n_len		 Length of N
* @param[in] p			 Value of P
* @param[in] p_len		 Length of P
* @param[in] res_cb		 Function that will handle the AES based result (16 bytes)
* @param[in] src_info	 Information used retrieve requester
****************************************************************************************
*/
extern void mesh_aes_k1(const uint8_t* salt, const uint8_t* n, uint8_t n_len, const uint8_t* p, 
						uint8_t p_len, uint8_t* aes_res);

__INLINE_S__ void mesh_sec_aes_k1(const uint8_t* salt,  const uint8_t* n, uint8_t n_len, const uint8_t* p, uint8_t p_len,
		  mesh_aes_func_result_cb res_cb, uint32_t src_info)
{
	uint8_t aes_res[16];
	uint8_t ts[16];
	uint8_t* tn = ke_malloc(n_len,0);
	uint8_t* tp = ke_malloc(p_len,0);
	//m_printf_hex(L_AL, "k1 salt", salt, 16);
	//m_printf_hex(L_AL, "k1 n", n, n_len);
	//m_printf_hex(L_AL, "k1 p", p, p_len);
	for(int i = 0; i < 16; ++i){
		ts[i] = salt[15-i];
	}
	for(int i = 0; i < n_len; ++i){
		tn[i] = n[n_len-1-i];
	}
	for(int i = 0; i < p_len; ++i){
		tp[i] = p[p_len-1-i];
	}
	
	mesh_aes_k1((const uint8_t*)ts, (const uint8_t*)tn, n_len, (const uint8_t*)tp, p_len, aes_res);
	
	ke_free(tn);
	ke_free(tp);
	
	for(int i = 0; i < 8; ++i){
		aes_res[i] ^= aes_res[15-i];
		aes_res[15-i] ^= aes_res[i];
		aes_res[i] ^= aes_res[15-i];
	}
	//m_printf_hex(L_AL, "k1 res", aes_res, 16);
	if(res_cb)
		res_cb(0, aes_res, src_info);
	
	
}

/**
****************************************************************************************
* @brief Start the AES K2 crypto function.
*
* @param[in] n			 Value of N - 128 bits
* @param[in] p			 Value of P
* @param[in] p_len		 Length of P
* @param[in] res_cb		 Function that will handle the AES based result (33 bytes)
* @param[in] src_info	 Information used retrieve requester
****************************************************************************************
*/
extern void mesh_aes_k2(const uint8_t* n, const uint8_t* p, uint8_t p_len, 
						uint8_t* aes_res);

__INLINE_S__ void mesh_sec_aes_k2(const uint8_t* n, const uint8_t* p, uint8_t p_len, 
						mesh_aes_func_result_cb res_cb, uint32_t src_info)
{
	uint8_t aes_res[33] = {0};
	uint8_t tn[16];
	uint8_t* tp = ke_malloc(p_len,0);
	//m_printf_hex(L_AL, "k2 n", n, 16);
	//m_printf_hex(L_AL, "k2 p", p, p_len);
	for(int i = 0; i < 16; ++i){
		tn[i] = n[15-i];
	}
	for(int i = 0; i < p_len; ++i){
		tp[i] = p[p_len-1-i];
	}

	mesh_aes_k2((const uint8_t*)tn, (const uint8_t*)tp, p_len, aes_res);

	ke_free(tp);

	for(int i = 0; i < sizeof(aes_res)/2; ++i){
		aes_res[i] ^= aes_res[32-i];
		aes_res[32-i] ^= aes_res[i];
		aes_res[i] ^= aes_res[32-i];
	}
	
	//m_printf_hex(L_AL, "k2 res", aes_res, 33);
	if(res_cb)
		res_cb(0, aes_res, src_info);
}


/**
****************************************************************************************
* @brief Start the AES K3 crypto function.
*
* @param[in] n			 Value of N - 128 bits
* @param[in] res_cb		 Function that will handle the AES based result (8 bytes)
* @param[in] src_info	 Information used retrieve requester
****************************************************************************************
*/
extern void mesh_aes_k3(const uint8_t* n, uint8_t* aes_res);
__INLINE_S__ void mesh_sec_aes_k3(const uint8_t* n, mesh_aes_func_result_cb res_cb, uint32_t src_info)
{
	uint8_t aes_res[8] = {0};
	uint8_t tn[16];
	//m_printf_hex(L_AL, "k3 n", n, 16);
	
	for(int i = 0; i < 16; ++i){
		tn[i] = n[15-i];
	}

	mesh_aes_k3((const uint8_t*)tn, aes_res);
	
	for(int i = 0; i < 4; ++i){
		aes_res[i] ^= aes_res[7-i];
		aes_res[7-i] ^= aes_res[i];
		aes_res[i] ^= aes_res[7-i];
	}
	
	//m_printf_hex(L_AL, "k3 res", aes_res, sizeof(aes_res));
	if(res_cb)
		res_cb(0, aes_res, src_info);
}

/**
****************************************************************************************
* @brief Start the AES K4 crypto function.
*
* @param[in] n			 Value of N - 128 bits
* @param[in] res_cb		 Function that will handle the AES based result (1 byte)
* @param[in] src_info	 Information used retrieve requester
****************************************************************************************
*/
extern void mesh_aes_k4(const uint8_t* n, uint8_t* aes_res);
__INLINE_S__ void mesh_sec_aes_k4(const uint8_t* n, mesh_aes_func_result_cb res_cb, uint32_t src_info)
{
	uint8_t aes_res[1] = {0};
	uint8_t tn[16];
	//m_printf_hex(L_AL, "k4 n", n, 16);
	
	for(int i = 0; i < 16; ++i){
		tn[i] = n[15-i];
	}
	
	mesh_aes_k4((const uint8_t*)tn, aes_res);
	//m_printf_hex(L_AL, "k4 res", aes_res, sizeof(aes_res));

	if(res_cb)
		res_cb(0, aes_res, src_info);
}

/**
****************************************************************************************
* @brief Start the AES CCM crypto function. Allocate memory for the CCM and start processing it
*		Execute result callback at end of function execution
*
* @param[in]  key			   Pointer to the Key to be used
* @param[in]  nonce			   13 Bytes Nonce to use for cipher/decipher
* @param[in]  in_message 	   Input message for AES-CCM exectuion
* @param[out] out_message	   Output message that will contain cipher+mic or decipher data
* @param[in]  message_len	   Length of Input/Output message without mic
* @param[in]  mic_len		   Length of the mic to use (2, 4, 6, 8, 10, 12, 14, 16 valid)
* @param[in]  cipher 		   True to encrypt message, False to decrypt it.
* @param[in]  add_auth_data	   Additional Authentication data used for computation of MIC
* @param[in]  add_auth_data_len Length of Additional Authentication data
* @param[in]  res_cb 		   Function that will handle the AES CCM result
* @param[in]  src_info		   Information used retrieve requester
****************************************************************************************
*/
extern void mesh_aes_ccm(const uint8_t* key, const uint8_t* nonce, const uint8_t* in_message,
		   uint8_t* out_message, uint16_t message_len, uint8_t mic_len, bool cipher,
		   const uint8_t* add_auth_data, uint8_t add_auth_data_len, mesh_aes_ccm_func_result_cb res_cb,
		   uint32_t src_info);
__INLINE_S__ void mesh_sec_aes_ccm(const uint8_t* key, const uint8_t* nonce, const uint8_t* in_message,
		   uint8_t* out_message, uint16_t message_len, uint8_t mic_len, bool cipher,
		   const uint8_t* add_auth_data, uint8_t add_auth_data_len, mesh_aes_ccm_func_result_cb res_cb, uint32_t src_info)
{
	uint8_t tk[16];
	//m_printf_hex(L_AL, "ccm key", key, 16);
	//m_printf_hex(L_AL, "ccm nonce",nonce, 13);
	//m_printf_hex(L_AL, "ccm in message",in_message, message_len);
	//M_PRINTF(L_AL, "ccm mic_len = %d", mic_len);
	//M_PRINTF(L_AL, "ccm cipher = %d", cipher);
	//if(add_auth_data)
	//	m_printf_hex(L_AL, "ccm auth data",add_auth_data, add_auth_data_len);
	for(int i = 0; i < 16; ++i){
		tk[i] = key[15-i];
	}
	mesh_aes_ccm((const uint8_t*)tk, nonce, in_message,
		   out_message, message_len, mic_len, cipher,
		   add_auth_data, add_auth_data_len, res_cb, src_info);
}

/**
****************************************************************************************
* @brief Read the Public key used for ECDH algorithm
*
* @param[in] renew	 Renew the Private/Public Key Pair
* @param[in] res_cb  Function that will provide result of Public Key Value
****************************************************************************************
*/
extern void mesh_pub_key_read(bool renew, mesh_sec_pub_key_res_cb res_cb);

__INLINE_S__ void mesh_sec_pub_key_read(bool renew, mesh_sec_pub_key_res_cb res_cb)
{
	mesh_pub_key_read(renew, res_cb);
}

/**
 ****************************************************************************************
 * @brief Perform computation of the ECDH Secret computation using peer device key
 *
 * @param[in] p_pub_key_x    32 bytes (LSB) X coordinate of public key
 * @param[in] p_pub_key_y    32 bytes (LSB) Y coordinate of public key
 * @param[in] res_cb         Function that will provide result of ECDH Secret computation
 ****************************************************************************************
 */
__INLINE_S__ void mesh_sec_ecdh_secret(const uint8_t* p_pub_key_x, const uint8_t* p_pub_key_y, mesh_sec_ecdh_secret_res_cb res_cb)
{
	extern void mesh_ecdh_secret(const uint8_t* p_pub_key_x, const uint8_t* p_pub_key_y, mesh_sec_ecdh_secret_res_cb res_cb);
	mesh_ecdh_secret(p_pub_key_x, p_pub_key_y, res_cb);
}

#endif // (BLE_MESH)

#endif/*__PORTSEC_H__*/

