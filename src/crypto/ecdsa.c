// Copyright 2020 RTrade Technologies Ltd
//
// licensed under GNU AFFERO GENERAL PUBLIC LICENSE;
// you may not use this file except in compliance with the License;
// You may obtain the license via the LICENSE file in the repository root;
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/ecdsa.h"
// mbedtls stuff
#include "crypto/ecdsa.h"
#include "crypto/peerutils.h"
#include "crypto/util.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/bignum.h"
#include "mbedtls/config.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/oid.h"
#include "mbedtls/pk.h"
#include "mbedtls/platform.h"
#include "mbedtls/x509.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*!
 * @brief frees up resources allocated for the private key
 */
int libp2p_crypto_ecdsa_free(ecdsa_private_key_t *pk) {
    mbedtls_pk_free(&pk->pk_ctx);
    mbedtls_ecdsa_free(&pk->ecdsa_ctx);
    pthread_mutex_destroy(&pk->mutex);
    free(pk);
    return 0;
}

/*! @brief returns a peer_id_t struct for the given private key
 * @details this is useful for exchanging peer identifier information with
 * @details anyone who connects to our host
 * @warning caller must free returned pointer when no longer needed
 * @details to get the peerid we take a sha256 hash of the public key file in PEM
 * format
 * @details we then generate a multihash of that sha256, and base58 encode it
 * @todo this currently relies on hard coded assumptions about hashing algorithm
 * @todo in the future this will change
 * @param pk a loaded ecdsa_private_key_t instance
 * @return Success: pointer to an instance of peer_id_t
 * @return Failure: NULL pointer
 */
peer_id_t *libp2p_crypto_ecdsa_keypair_peerid(ecdsa_private_key_t *pk) {
    public_key_t *pub_key = libp2p_crypto_ecdsa_keypair_public(pk);
    if (pub_key == NULL) {
        return NULL;
    }
    unsigned char *public_key_hash = calloc(1, 32);
    if (public_key_hash == NULL) {
        return NULL;
    }
    int rc = libp2p_crypto_hashing_sha256(
        pub_key->data, strlen((char *)pub_key->data), public_key_hash);
    if (rc != 1) {
        libp2p_crypto_public_key_free(pub_key);
        free(public_key_hash);
        print_mbedtls_error(rc);
        return NULL;
    }

    unsigned char temp_peer_id[1024];
    size_t len = (size_t)1024;
    peer_id_t *pid = libp2p_new_peer_id(temp_peer_id, &len, public_key_hash, 32);

    libp2p_crypto_public_key_free(pub_key);
    free(public_key_hash);

    return pid;
}

/*!
 * @brief returns the public key associated with the private key in PEM format
 * @details the returned struct is suitable for encoding into CBOR and sending to
 * peers
 * @warning caller must free returned data when no longer
 * @todo this currently relies in usage of `str..` we should use `mem...` instead
 * @return Success: an instance of public_key_t with the corresponding information
 * @return Failure: NULL pointer
 */
public_key_t *libp2p_crypto_ecdsa_keypair_public(ecdsa_private_key_t *pk) {
    // we use `strcpy` and `strlen` as mbedtls_pk_write_pubkey_pem includes null
    // terminating byte
    unsigned char output_buf[1024];
    int rc = mbedtls_pk_write_pubkey_pem(&pk->pk_ctx, output_buf, 1024);
    if (rc != 0) {
        print_mbedtls_error(rc);
        return NULL;
    }
    unsigned char *public_key =
        malloc(sizeof(unsigned char) * strlen((char *)output_buf) + 1);
    if (public_key == NULL) {
        return NULL;
    }
    strcpy((char *)public_key, (char *)output_buf);

    public_key_t *pub_key = libp2p_crypto_public_key_new();
    if (pub_key == NULL) {
        free(public_key);
        return NULL;
    }

    pub_key->data = public_key;
    pub_key->data_size = strlen((char *)output_buf);
    pub_key->type = KEYTYPE_ECDSA;

    return pub_key;
}

/*!
 * @brief parses a PEM encoded private key and returns a struct for use
 * @details the returned mbedtls_*_context in the struct are not suitable for
 * concurrent use, please access through mutex locks
 * @param pem_input the PEM encoded ECDSA private key
 * @return an initialize and populated ecdsa_private_key_t
 */
ecdsa_private_key_t *
libp2p_crypto_ecdsa_pem_to_private_key(unsigned char *pem_input) {
    mbedtls_pk_context pk_context;
    mbedtls_ecdsa_context ecdsa_context;

    mbedtls_pk_init(&pk_context);
    mbedtls_ecdsa_init(&ecdsa_context);

    int rc = mbedtls_pk_parse_key(&pk_context, pem_input,
                                  strlen((char *)pem_input) + 1, NULL, 0);
    if (rc != 0) {
        print_mbedtls_error(rc);
        return NULL;
    }

    rc = mbedtls_ecdsa_from_keypair(&ecdsa_context, mbedtls_pk_ec(pk_context));
    if (rc != 0) {
        print_mbedtls_error(rc);
        return NULL;
    }
    ecdsa_private_key_t *pk = malloc(sizeof(ecdsa_private_key_t) +
                                     sizeof(pk_context) + sizeof(ecdsa_context));
    if (pk == NULL) {
        mbedtls_pk_free(&pk_context);
        mbedtls_ecdsa_free(&ecdsa_context);
        return NULL;
    }
    pk->ecdsa_ctx = ecdsa_context;
    pk->pk_ctx = pk_context;
    pthread_mutex_init(&pk->mutex, NULL);

    return pk;
}

/*! @brief used to generate an ECDSA keypair
 * @returns Success: pointer to an ecdsa_private_key_t instance
 * @returns NUULL pointer
 */
ecdsa_private_key_t *
libp2p_crypto_ecdsa_keypair_generation(mbedtls_ecp_group_id curve) {
    mbedtls_pk_context ecdsa_key_pair;
    mbedtls_entropy_context entropy_context;
    mbedtls_ctr_drbg_context ctr_drb_context;
    /*! * @todo figure out if we need this
     */
    const char *pers = "ecdsa";

    // initialize mbedtls context(s)
    mbedtls_ctr_drbg_init(&ctr_drb_context);
    mbedtls_entropy_init(&entropy_context);
    mbedtls_pk_init(&ecdsa_key_pair);
    mbedtls_pk_setup(&ecdsa_key_pair, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));

    // seed entropy generation
    int rc = mbedtls_ctr_drbg_seed(&ctr_drb_context, mbedtls_entropy_func,
                                   &entropy_context, (const unsigned char *)pers,
                                   strlen(pers) != 0);
    if (rc != 0) {
        print_mbedtls_error(rc);
        // TODO(bonedaddy): free up memory
        return NULL;
    }

    // generate the actual ecdsa keypair
    rc = mbedtls_ecdsa_genkey(mbedtls_pk_ec(ecdsa_key_pair), curve,
                              mbedtls_ctr_drbg_random, &ctr_drb_context

    );
    if (rc != 0) {
        print_mbedtls_error(rc);
        return NULL;
    }

    unsigned char output[1024];
    memset(output, 0, 1024);

    // write the private key in PEM format to output
    rc = mbedtls_pk_write_key_pem(&ecdsa_key_pair, output, 1024);
    if (rc != 0) {
        print_mbedtls_error(rc);
        return NULL;
    }

    // free up allocated resources
    mbedtls_pk_free(&ecdsa_key_pair);
    mbedtls_ctr_drbg_free(&ctr_drb_context);
    mbedtls_entropy_free(&entropy_context);

    return libp2p_crypto_ecdsa_pem_to_private_key(output);
}

/*!
 * @brief used to save a private key at the given path
 * @param pk an instance of ecdsa_private_key_t
 * @param path the location on disk to save file at
 * @return Success: 0
 * @return Failure: 1
 */
int libp2p_crypto_ecdsa_private_key_save(ecdsa_private_key_t *pk, char *path) {
    unsigned char pem_buffer[1024];

    int rc = mbedtls_pk_write_key_pem(&pk->pk_ctx, pem_buffer, 1024);
    if (rc != 0) {
        print_mbedtls_error(rc);
        return 1;
    }

    int fd = open(path, O_WRONLY | O_CREAT | O_SYNC | O_TRUNC, 0640);
    if (fd <= 0) {
        printf("failed to open: %s\n", strerror(errno));
        return 1;
    }

    size_t stat = write(fd, (char *)pem_buffer, strlen((char *)pem_buffer));
    if ((int)stat == -1) {
        printf("failed to write: %s\n", strerror(errno));
        rc = 1;
    } else {
        rc = 0;
    }

    close(fd);
    return rc;
}

/*!
 * @brief loads an ECDSA private key from a file containing a PEM key
 * @param path the path on disk to a PEM file containing an ECDSA private key
 * @return Success: pointer to an instance of an ecdsa_private_key_t type
 * @return Failure: NULL pointer
 */
ecdsa_private_key_t *libp2p_crypto_ecdsa_private_key_from_file(char *path) {
    int fd = open(path, O_RDONLY, 0640);
    if (fd <= 0) {
        printf("failed to open: %s\n", strerror(errno));
        return NULL;
    }
    char pem_buffer[1024];
    memset(pem_buffer, 0, 1024);
    int rc = read(fd, pem_buffer, 1024);
    if (rc == -1) {
        printf("failed to read from file: %s\n", strerror(errno));
        return NULL;
    }
    close(fd);

    return libp2p_crypto_ecdsa_pem_to_private_key((unsigned char *)pem_buffer);
}

/*!
 * @brief takes a private key and returns its corresponding PEM format
 */
unsigned char *libp2p_crypto_ecdsa_private_key_to_pem(ecdsa_private_key_t *pk) {
    unsigned char buffer[1024];
    memset(buffer, 0, 1024);

    int rc = mbedtls_pk_write_key_pem(&pk->pk_ctx, buffer, 1024);
    if (rc != 0) {
        print_mbedtls_error(rc);
        return NULL;
    }

    // this works as the previous function includes null terminating byte
    unsigned char *ret_buf = calloc(1, strlen((char *)buffer) + 1);
    if (ret_buf == NULL) {
        return NULL;
    }

    memcpy(ret_buf, buffer, strlen((char *)buffer));

    return ret_buf;
}