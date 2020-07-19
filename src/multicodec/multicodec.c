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
// limitations under the Licens

/*! @file multicodec.h
  * @author Bonedaddy
  * @brief an implemention of multicodec
  * @details enables self describing data encoding methods, for the actual codec definitions see codecs.h
  * @note for more information on multicodecs see https://github.com/multiformats/multicodec
*/

#include "multicodec/multicodec.h"
#include "multicodec/codecs.h"
#include <string.h>
#include <stdio.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

const char *CODECS_LIST[CODECS_COUNT] = {IDENTITY, IP4, TCP, SHA1, SHA2_256, SHA2_512, SHA3_512, SHA3_384, SHA3_256, SHA3_224, SHAKE_128, SHAKE_256, KECCAK_224, KECCAK_256, KECCAK_384, KECCAK_512, BLAKE3, DCCP, MURMUR3_128, MURMUR3_32, IP6, IP6ZONE, PATH, MULTICODEC, MULTIHASH, MULTIADDR, MULTIBASE, DNS, DNS4, DNS6, DNSADDR, PROTOBUF, CBOR, RAW, DBL_SHA2_256, RLP, BENCODE, DAG_PB, DAG_CBOR, LIBP2P_KEY, GIT_RAW, TORRENT_INFO, TORRENT_FILE, LEOFCOIN_BLOCK, LEOFCOIN_TX, LEOFCOIN_PR, SCTP, DAG_JOSE, DAG_COSE, ETH_BLOCK, ETH_BLOCK_LIST, ETH_TX_TRIE, ETH_TX, ETH_TX_RECEIPT_TRIE, ETH_TX_RECEIPT, ETH_STATE_TRIE, ETH_ACCOUNT_SNAPSHOT, ETH_STORAGE_TRIE, BITCOIN_BLOCK, BITCOIN_TX, BITCOIN_WITNESS_COMMITMENT, ZCASH_BLOCK, ZCASH_TX, STELLAR_BLOCK, STELLAR_TX, MD4, MD5, BMT, DECRED_BLOCK, DECRED_TX, IPLD_NS, IPFS_NS, SWARM_NS, IPNS_NS, ZERONET, SECP256K1_PUB, BLS12_381_G1_PUB, BLS12_381_G2_PUB, X25519_PUB, ED25519_PUB, DASH_BLOCK, DASH_TX, SWARM_MANIFEST, SWARM_FEED, UDP, P2P_WEBRTC_STAR, P2P_WEBRTC_DIRECT, P2P_STARDUST, P2P_CIRCUIT, DAG_JSON, UDT, UTP, UNIX, P2P, IPFS, HTTPS, ONION, ONION3, GARLIC64, GARLIC32, TLS, QUIC, WS, WSS, P2P_WEBSOCKET_STAR, HTTP, JSON, MESSAGEPACK, LIBP2P_PEER_RECORD, SHA2_256_TRUNC254_PADDED, RIPEMD_128, RIPEMD_160, RIPEMD_256, RIPEMD_320, X11, SM3_256, BLAKE2B_8, BLAKE2B_16, BLAKE2B_24, BLAKE2B_32, BLAKE2B_40, BLAKE2B_48, BLAKE2B_56, BLAKE2B_64, BLAKE2B_72, BLAKE2B_80, BLAKE2B_88, BLAKE2B_96, BLAKE2B_104, BLAKE2B_112, BLAKE2B_120, BLAKE2B_128, BLAKE2B_136, BLAKE2B_144, BLAKE2B_152, BLAKE2B_160, BLAKE2B_168, BLAKE2B_176, BLAKE2B_184, BLAKE2B_192, BLAKE2B_200, BLAKE2B_208, BLAKE2B_216, BLAKE2B_224, BLAKE2B_232, BLAKE2B_240, BLAKE2B_248, BLAKE2B_256, BLAKE2B_264, BLAKE2B_272, BLAKE2B_280, BLAKE2B_288, BLAKE2B_296, BLAKE2B_304, BLAKE2B_312, BLAKE2B_320, BLAKE2B_328, BLAKE2B_336, BLAKE2B_344, BLAKE2B_352, BLAKE2B_360, BLAKE2B_368, BLAKE2B_376, BLAKE2B_384, BLAKE2B_392, BLAKE2B_400, BLAKE2B_408, BLAKE2B_416, BLAKE2B_424, BLAKE2B_432, BLAKE2B_440, BLAKE2B_448, BLAKE2B_456, BLAKE2B_464, BLAKE2B_472, BLAKE2B_480, BLAKE2B_488, BLAKE2B_496, BLAKE2B_504, BLAKE2B_512, BLAKE2S_8, BLAKE2S_16, BLAKE2S_24, BLAKE2S_32, BLAKE2S_40, BLAKE2S_48, BLAKE2S_56, BLAKE2S_64, BLAKE2S_72, BLAKE2S_80, BLAKE2S_88, BLAKE2S_96, BLAKE2S_104, BLAKE2S_112, BLAKE2S_120, BLAKE2S_128, BLAKE2S_136, BLAKE2S_144, BLAKE2S_152, BLAKE2S_160, BLAKE2S_168, BLAKE2S_176, BLAKE2S_184, BLAKE2S_192, BLAKE2S_200, BLAKE2S_208, BLAKE2S_216, BLAKE2S_224, BLAKE2S_232, BLAKE2S_240, BLAKE2S_248, BLAKE2S_256, SKEIN256_8, SKEIN256_16, SKEIN256_24, SKEIN256_32, SKEIN256_40, SKEIN256_48, SKEIN256_56, SKEIN256_64, SKEIN256_72, SKEIN256_80, SKEIN256_88, SKEIN256_96, SKEIN256_104, SKEIN256_112, SKEIN256_120, SKEIN256_128, SKEIN256_136, SKEIN256_144, SKEIN256_152, SKEIN256_160, SKEIN256_168, SKEIN256_176, SKEIN256_184, SKEIN256_192, SKEIN256_200, SKEIN256_208, SKEIN256_216, SKEIN256_224, SKEIN256_232, SKEIN256_240, SKEIN256_248, SKEIN256_256, SKEIN512_8, SKEIN512_16, SKEIN512_24, SKEIN512_32, SKEIN512_40, SKEIN512_48, SKEIN512_56, SKEIN512_64, SKEIN512_72, SKEIN512_80, SKEIN512_88, SKEIN512_96, SKEIN512_104, SKEIN512_112, SKEIN512_120, SKEIN512_128, SKEIN512_136, SKEIN512_144, SKEIN512_152, SKEIN512_160, SKEIN512_168, SKEIN512_176, SKEIN512_184, SKEIN512_192, SKEIN512_200, SKEIN512_208, SKEIN512_216, SKEIN512_224, SKEIN512_232, SKEIN512_240, SKEIN512_248, SKEIN512_256, SKEIN512_264, SKEIN512_272, SKEIN512_280, SKEIN512_288, SKEIN512_296, SKEIN512_304, SKEIN512_312, SKEIN512_320, SKEIN512_328, SKEIN512_336, SKEIN512_344, SKEIN512_352, SKEIN512_360, SKEIN512_368, SKEIN512_376, SKEIN512_384, SKEIN512_392, SKEIN512_400, SKEIN512_408, SKEIN512_416, SKEIN512_424, SKEIN512_432, SKEIN512_440, SKEIN512_448, SKEIN512_456, SKEIN512_464, SKEIN512_472, SKEIN512_480, SKEIN512_488, SKEIN512_496, SKEIN512_504, SKEIN512_512, SKEIN1024_8, SKEIN1024_16, SKEIN1024_24, SKEIN1024_32, SKEIN1024_40, SKEIN1024_48, SKEIN1024_56, SKEIN1024_64, SKEIN1024_72, SKEIN1024_80, SKEIN1024_88, SKEIN1024_96, SKEIN1024_104, SKEIN1024_112, SKEIN1024_120, SKEIN1024_128, SKEIN1024_136, SKEIN1024_144, SKEIN1024_152, SKEIN1024_160, SKEIN1024_168, SKEIN1024_176, SKEIN1024_184, SKEIN1024_192, SKEIN1024_200, SKEIN1024_208, SKEIN1024_216, SKEIN1024_224, SKEIN1024_232, SKEIN1024_240, SKEIN1024_248, SKEIN1024_256, SKEIN1024_264, SKEIN1024_272, SKEIN1024_280, SKEIN1024_288, SKEIN1024_296, SKEIN1024_304, SKEIN1024_312, SKEIN1024_320, SKEIN1024_328, SKEIN1024_336, SKEIN1024_344, SKEIN1024_352, SKEIN1024_360, SKEIN1024_368, SKEIN1024_376, SKEIN1024_384, SKEIN1024_392, SKEIN1024_400, SKEIN1024_408, SKEIN1024_416, SKEIN1024_424, SKEIN1024_432, SKEIN1024_440, SKEIN1024_448, SKEIN1024_456, SKEIN1024_464, SKEIN1024_472, SKEIN1024_480, SKEIN1024_488, SKEIN1024_496, SKEIN1024_504, SKEIN1024_512, SKEIN1024_520, SKEIN1024_528, SKEIN1024_536, SKEIN1024_544, SKEIN1024_552, SKEIN1024_560, SKEIN1024_568, SKEIN1024_576, SKEIN1024_584, SKEIN1024_592, SKEIN1024_600, SKEIN1024_608, SKEIN1024_616, SKEIN1024_624, SKEIN1024_632, SKEIN1024_640, SKEIN1024_648, SKEIN1024_656, SKEIN1024_664, SKEIN1024_672, SKEIN1024_680, SKEIN1024_688, SKEIN1024_696, SKEIN1024_704, SKEIN1024_712, SKEIN1024_720, SKEIN1024_728, SKEIN1024_736, SKEIN1024_744, SKEIN1024_752, SKEIN1024_760, SKEIN1024_768, SKEIN1024_776, SKEIN1024_784, SKEIN1024_792, SKEIN1024_800, SKEIN1024_808, SKEIN1024_816, SKEIN1024_824, SKEIN1024_832, SKEIN1024_840, SKEIN1024_848, SKEIN1024_856, SKEIN1024_864, SKEIN1024_872, SKEIN1024_880, SKEIN1024_888, SKEIN1024_896, SKEIN1024_904, SKEIN1024_912, SKEIN1024_920, SKEIN1024_928, SKEIN1024_936, SKEIN1024_944, SKEIN1024_952, SKEIN1024_960, SKEIN1024_968, SKEIN1024_976, SKEIN1024_984, SKEIN1024_992, SKEIN1024_1000, SKEIN1024_1008, SKEIN1024_1016, SKEIN1024_1024, POSEIDON_BLS12_381_A2_FC1, POSEIDON_BLS12_381_A2_FC1_SC, FIL_COMMITMENT_UNSEALED, FIL_COMMITMENT_SEALED, HOLOCHAIN_ADR_V0, HOLOCHAIN_ADR_V1, HOLOCHAIN_KEY_V0, HOLOCHAIN_KEY_V1, HOLOCHAIN_SIG_V0, HOLOCHAIN_SIG_V1};


/*! @brief free up resources allocated for multicodec_encoded_t
  * @param encoded the isntance of multicodec_encoded_t we are freeing
*/
void multicodec_free_encoded(multicodec_encoded_t *encoded) {
    /*!
      * @note we probably dont need to free up encoded->codec
    */
    free(encoded->data);
    free(encoded);
}

/**
 * Encode a char array using the specified encoding
 * @param codecName a standard name of the codec
 * @param inData the incoming data to be encoded
 * @param inDataLength the length of the inData
 * @param outData a place to store the outgoing data that is at least
 * maxOutDataLength in size
 * @param maxOutDataLength the max size for the output
 * @returns Success: pointer to an instance of multicodec_encoded_t 
 * @returns Failure: NULL pointer
 */
multicodec_encoded_t *multicodec_encode(char *codec, char *inData, size_t inDataLength,
                      char *outData, size_t maxOutDataLength) {
    
    // make sure the codec is supported
    if (multicodec_is_valid(codec) == false) {
        return NULL;   
    }

    multicodec_encoded_t *encoded = calloc(
        sizeof(multicodec_encoded_t),
        sizeof(multicodec_encoded_t)  
    );
    if (encoded == NULL) {
        return NULL;
    }
    memcpy(
        outData,
        codec,
        strlen(codec) // these is no guarantee tehse include a null terminating zero
    );
    memcpy(
        outData + strlen(codec), // copy inData to outData offset my strlen(codec)
        inData,
        inDataLength
    );
    encoded->codec = codec;
    encoded->data = outData;
    return encoded;
}

/**
 * Decode a multicodec encoded data array
 * @param encoded an instance of multicodec_encoded_t holding the encoded data and codec used
 * @param outData the array to put the results in
 * @param maxOutDataLength the maximum size of the data
 * @returns an error code or 0 if all is well
 */
int multicodec_decode(multicodec_encoded_t *encoded, char *outData,
                      size_t maxOutDataLength) {
    
    // make sure the codec is supported
    if (multicodec_is_valid(encoded->codec) == false) {
        return 1;   
    }

    memcpy(
        outData,
        /*!
          * @note copy from inData starting at offset, do `+1` to account for array index starting at 0,
          * @note and thus the next I value is the start of the actual data, whereas if we simply did 
          * @note `inData + i` we would be starting at the end of the codec part
        */
        encoded->data + strlen(encoded->codec),
        maxOutDataLength
    );
    return 0;
}

/*!
  * @brief used to check if the provided codec is a valid codec
  * @param codec the codec value to check
  * @returns Success: true
  * @returns Failure: false
*/
bool multicodec_is_valid(char *codec) {
    for (int i = 0; i < CODECS_COUNT; i++) {
        if (
            strcmp(codec, CODECS_LIST[i]) == 0
        ) {
            return true;
        }
    }
    return false;
}