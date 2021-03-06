#include "crypto/peerutils.h"
#include "crypto/sha256.h"
#include "multibase/multibase.h"
#include "multihash/hashes.h"
#include "multihash/multihash.h"
#include <stdlib.h>
#include <string.h>

/*!
 * @brief generates a peerID for the public key using SHA256 as the hashing algorithm
 * @details is a wrapper around libp2p_new_peer_id that abstracts away hashing and
 * memory allocs and stuff like that
 * @param output where to store the resulting peerID
 * @param output_len the max lenght of the output buffer, but will also be used to
 * store size of buffer
 * @param public_key the public key in PEM format to hash
 * @param public_key_len the length of the public key
 * @return Success: pointer to an instance of peer_id_t
 * @return Failure: NULL pointer
 */
peer_id_t *libp2p_new_peer_id_sha256(unsigned char *output, size_t *output_len,
                                     unsigned char *public_key,
                                     size_t public_key_len) {

    unsigned char temp_hash_output[32];
    memset(temp_hash_output, 0, 32);

    int rc =
        libp2p_crypto_hashing_sha256(public_key, public_key_len, temp_hash_output);
    if (rc != 1) {
        return 0;
    }

    return libp2p_new_peer_id(output, output_len, temp_hash_output, 32);
}

/**
 * @brief constructs a peer identifier from the given public key
 * @details this is responsible for taking the hash of a public key
 * @details turning it into a multihash, and the base encoding it
 * @note is currently hard coded for the input hash being SHA256
 * @note if you allocated memory for output param you can free it up after this
 * function returns
 * @note this is because we copy the data placed there into the returned struct
 * @param output where to store the resulting peerID
 * @param output_len the max lenght of the output buffer, but will also be used to
 * store size of buffer
 * @param input_hash the input public key hash, usually sha256
 * @param input_size the input size, usually 32 bytes
 * @return Success: pointer to an instance of peer_id_t
 * @return Failure: NULL pointer
 */
peer_id_t *libp2p_new_peer_id(unsigned char *output, size_t *output_len,
                              unsigned char *input_hash, size_t input_size) {
    unsigned char temp_buffer[*output_len];
    memset(temp_buffer, 0, *output_len);

    // wrap the base58 into a multihash
    int retVal = mh_new(temp_buffer, MH_H_SHA2_256, input_hash, input_size);
    if (retVal < 0)
        return 0;

    retVal = multibase_encode(MULTIBASE_BASE32, temp_buffer,
                              input_size + 2, // 2 (1 for code, 1 for digest_len)
                              output, *output_len, output_len);

    if (retVal != 1) {
        return 0;
    }

    peer_id_t *pid = calloc(1, sizeof(peer_id_t));
    if (pid == NULL) {
        return NULL;
    }

    // increase output_len by 1 otherwise valgrind reports invalid read errors
    *output_len += 1;

    pid->data = calloc(1, *output_len);
    if (pid->data == NULL) {
        free(pid);
        return NULL;
    }

    memcpy(pid->data, output, *output_len);

    pid->len = *output_len;

    return pid;
}

/*!
 * @brief free up resources allocated for an instance of peer_id_t
 */
void libp2p_peer_id_free(peer_id_t *pid) {
    free(pid->data);
    free(pid);
}