# libcp2p

A libp2p-like protocol written in C and designed to support embedded systems. High level language bindings will use FFI. Heavy WIP

# difference between libcp2p and libp2p

* Written in C
* Uses mbedtls 
* Removal of built-in identify protocol
  * LibP2P is heavily reliant on the identify protocol and it is not possible to remove it
  * Redesigned so that identify is optional
* Protobuf replaced with cbor


# roadmap

* Get basic multiformats working in C
  * Multiaddr
  * Multihash
  * Protobuf
* Get basic networking done
* ....

# protobuf

* The `src/protobuf` code is being left here for the time being, but protobuf is being deprecated for cbor

# installation

## dependencies

* CMake >= 3.0
* GCC with support for C17
  * If you want to run static analysis, GCC-10 installed
* CMocka (testing dependency)
* Valgrind (dynamic analysis)
* clang-format (code formatting)
* doxygen (documentation generation)
* pthreads


## compiling mbedtls

```shell
$> make -j6 CFLAGS="-fPIC" # reduce -jX to suit your environment appropriately
$> sudo make install CFLAGS="-fPIC"
$> sudo ldconfig
```

# development

* Code submitted in PRs must be formatted using `make format`
* All code must be tested with CMocka
* All code must be documented with doxygen compatible comments
* All code must be tested with valgrind

## testing

If starting a new test file, make sure the last part of the file name is `_test.c`, and that your actual tests have the following function signature:
```C
void test_your_test_name(void **state)
```
Use the following template to copy and paste when starting a new test file

```C
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
  write your tests here
*/

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(/* your test here */)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
```

# TODOs

* Look into using glib for stuff like base64 decoding
  * https://developer.gnome.org/glib/stable/glib-Base64-Encoding.html#g-base64-encode