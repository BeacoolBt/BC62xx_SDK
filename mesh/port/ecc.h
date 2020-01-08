#include "math.h"

#include <stdbool.h>
#include <stdint.h>

#define AES_ENCRY_CBC 0x01
#define AES_ENCRY_ECB 0x02

#define AES_TYPE_SELECT(state)	HWRITE(mem_aes_type,state);
#define START_AES_ENCRY()		HREAD(mem_aes_state_map) |= 0x01 << 1;
#define	START_RAND()			HREAD(mem_aes_state_map) |= 0x01 << 2;


/* Create a public key from a private key.
 *
 * Outputs:
 *	private_key - Const private key
 *	public_key  - Will be filled in with the public key.
 *
 * Returns true if the public key was generated successfully, false
 * if an error occurred. The keys are with the LSB first.
 */
bool ecc_make_public_key(const uint8_t private_key[32], uint8_t public_key[64]);

/* Create a public/private key pair.
 *
 * Outputs:
 *	public_key  - Will be filled in with the public key.
 *	private_key - Will be filled in with the private key.
 *
 * Returns true if the key pair was generated successfully, false
 * if an error occurred. The keys are with the LSB first.
 */
bool ecc_make_key(uint8_t public_key[64], uint8_t private_key[32]);

/* Check to see if a public key is valid.
 *
 * Inputs:
 *	public_key - The public key to check.
 *
 * Returns true if the public key is valid, false if it is invalid.
*/
bool ecc_valid_public_key(const uint8_t public_key[64]);

/* Compute a shared secret given your secret key and someone else's
 * public key.
 * Note: It is recommended that you hash the result of ecdh_shared_secret
 * before using it for symmetric encryption or HMAC.
 *
 * Inputs:
 *	public_key  - The public key of the remote party.
 *	private_key - Your private key.
 *
 * Outputs:
 *	secret - Will be filled in with the shared secret value.
 *
 * Returns true if the shared secret was generated successfully, false
 * if an error occurred. Both input and output parameters are with the
 * LSB first.
 */
bool ecdh_shared_secret(const uint8_t public_key[64],
				const uint8_t private_key[32],
				uint8_t secret[32]);
