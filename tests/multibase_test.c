#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>
#include "multibase/multibase.h"

#pragma GCC diagnostic ignored "-Wunused-parameter"

void test_multibase_encode_decode_base16(void **state) {
    unsigned char *input = "hello world";
    int esize = multibase_encode_size(
        MULTIBASE_BASE16,
        input,
        12
    );
    assert(esize > 0);

    unsigned char *output = calloc(1, esize);
    size_t len;
    int rc = multibase_encode(
        MULTIBASE_BASE16, 
        input,
        strlen((char *)input),
        output,
        esize,
        &len
    );
    assert(rc == 1);
    printf("base16: %s\n", output);
    // assert that the first element is the base16 identifier
    assert(output[0] == MULTIBASE_BASE16);
    assert(
        strcmp((char *)output, "f68656c6c6f20776f726c64") == 0
    );

    int dsize = multibase_decode_size(MULTIBASE_BASE16, output, len);
    assert(dsize > 0);

    unsigned char *decoded_output = calloc(1, dsize);
    size_t written;
    rc = multibase_decode(output, len, decoded_output, dsize, &written);
    assert(rc == 1);
    assert(
        memcmp(
            input,
            decoded_output,
            written
        ) == 0
    );

    free(output);
    free(decoded_output);
}

void test_multibase_encode_decode_base32(void **state) {
    unsigned char *input = "hello world";
    int esize = multibase_encode_size(MULTIBASE_BASE32, input, 12);
    assert(esize > 0);
    
    unsigned char *output = calloc(1, esize);
    size_t len;
    int rc = multibase_encode(
        MULTIBASE_BASE32,
        input,
        strlen((char *)input),
        output,
        esize,
        &len
    );
    assert(rc == 1);
    assert(output[0] == MULTIBASE_BASE32);
    assert(
        strcmp(
            (char *)output,
            "bNBSWY3DPEB3W64TMMQ======"
        ) == 0
    );

    int dsize = multibase_decode_size(MULTIBASE_BASE16, output, len);
    assert(dsize > 0);

    unsigned char *decoded_output = calloc(1, dsize);
    size_t res;
    rc = multibase_decode(
        output,
        len,
        decoded_output,
        dsize,
        &res
    );
    assert(rc == 1);
    assert(
        memcmp(
            input,
            decoded_output,
            res
        ) == 0
    );

    free(output);
    free(decoded_output);
}

void test_multibase_encode_decode_base64(void **state) {
    unsigned char *input = "hello world";
    int esize = multibase_encode_size(MULTIBASE_BASE64, input, 12);
    assert(esize > 0);

    unsigned char *output = calloc(1, esize);
    size_t len;
    int rc = multibase_encode(
        MULTIBASE_BASE64,
        input,
        strlen((char *)input),
        output,
        esize,
        &len
    );
    assert(rc == 1);
    assert(output[0] == MULTIBASE_BASE64);
    assert(
        strcmp(
            (char *)output,
            "maGVsbG8gd29ybGQ="
        ) == 0
    );

    int dsize = multibase_encode_size(MULTIBASE_BASE64, output, len);
    assert(dsize > 0);

    unsigned char *decoded_output = calloc(1, dsize);
    size_t res;
    rc = multibase_decode(
        output,
        len,
        decoded_output,
        dsize,
        &res
    );
    assert(rc == 1);
    assert(
        memcmp(
            input,
            decoded_output,
            res
        ) == 0
    );

    free(output);
    free(decoded_output);
}


int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_multibase_encode_decode_base16),
        cmocka_unit_test(test_multibase_encode_decode_base32),
        cmocka_unit_test(test_multibase_encode_decode_base64)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}