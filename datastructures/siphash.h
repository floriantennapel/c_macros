#ifndef SIPHASH_H
#define SIPHASH_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/***********************************************************************************************
 * Hash function source code
 *
 * source: <https://github.com/veorq/SipHash>
 * Some global scope names have been prefixed with _SIPHASH
 * The code was also defined as static and moved to a header file, to avoid needing to link it
 ***********************************************************************************************/

/*
   SipHash reference C implementation

   Copyright (c) 2012-2022 Jean-Philippe Aumasson
   <jeanphilippe.aumasson@gmail.com>
   Copyright (c) 2012-2014 Daniel J. Bernstein <djb@cr.yp.to>

   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along
   with
   this software. If not, see
   <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

/* default: SipHash-2-4 */
#ifndef _SIPHASH_cROUNDS
#define _SIPHASH_cROUNDS 2
#endif
#ifndef _SIPHASH_dROUNDS
#define _SIPHASH_dROUNDS 4
#endif

#define _SIPHASH_ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define _SIPHASH_U32TO8_LE(p, v)                                                        \
    (p)[0] = (uint8_t)((v));                                                   \
    (p)[1] = (uint8_t)((v) >> 8);                                              \
    (p)[2] = (uint8_t)((v) >> 16);                                             \
    (p)[3] = (uint8_t)((v) >> 24);

#define _SIPHASH_U64TO8_LE(p, v)                                                        \
    _SIPHASH_U32TO8_LE((p), (uint32_t)((v)));                                           \
    _SIPHASH_U32TO8_LE((p) + 4, (uint32_t)((v) >> 32));

#define _SIPHASH_U8TO64_LE(p)                                                           \
    (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) |                        \
     ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) |                 \
     ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) |                 \
     ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56))

#define _SIPHASH_SIPROUND                                                               \
    do {                                                                       \
        v0 += v1;                                                              \
        v1 = _SIPHASH_ROTL(v1, 13);                                                     \
        v1 ^= v0;                                                              \
        v0 = _SIPHASH_ROTL(v0, 32);                                                     \
        v2 += v3;                                                              \
        v3 = _SIPHASH_ROTL(v3, 16);                                                     \
        v3 ^= v2;                                                              \
        v0 += v3;                                                              \
        v3 = _SIPHASH_ROTL(v3, 21);                                                     \
        v3 ^= v0;                                                              \
        v2 += v1;                                                              \
        v1 = _SIPHASH_ROTL(v1, 17);                                                     \
        v1 ^= v2;                                                              \
        v2 = _SIPHASH_ROTL(v2, 32);                                                     \
    } while (0)

/*
    Computes a SipHash value
    *in: pointer to input data (read-only)
    inlen: input data length in bytes (any size_t value)
    *k: pointer to the key data (read-only), must be 16 bytes 
    *out: pointer to output data (write-only), outlen bytes must be allocated
    outlen: length of the output in bytes, must be 8 or 16
*/
static int _siphash_source_code(const void *in, const size_t inlen, const void *k, uint8_t *out,
            const size_t outlen) {

    const unsigned char *ni = (const unsigned char *)in;
    const unsigned char *kk = (const unsigned char *)k;

    assert((outlen == 8) || (outlen == 16));
    uint64_t v0 = UINT64_C(0x736f6d6570736575);
    uint64_t v1 = UINT64_C(0x646f72616e646f6d);
    uint64_t v2 = UINT64_C(0x6c7967656e657261);
    uint64_t v3 = UINT64_C(0x7465646279746573);
    uint64_t k0 = _SIPHASH_U8TO64_LE(kk);
    uint64_t k1 = _SIPHASH_U8TO64_LE(kk + 8);
    uint64_t m;
    int i;
    const unsigned char *end = ni + inlen - (inlen % sizeof(uint64_t));
    const int left = inlen & 7;
    uint64_t b = ((uint64_t)inlen) << 56;
    v3 ^= k1;
    v2 ^= k0;
    v1 ^= k1;
    v0 ^= k0;

    if (outlen == 16)
        v1 ^= 0xee;

    for (; ni != end; ni += 8) {
        m = _SIPHASH_U8TO64_LE(ni);
        v3 ^= m;

        for (i = 0; i < _SIPHASH_cROUNDS; ++i)
            _SIPHASH_SIPROUND;

        v0 ^= m;
    }

    switch (left) {
    case 7:
        b |= ((uint64_t)ni[6]) << 48;
        /* FALLTHRU */
    case 6:
        b |= ((uint64_t)ni[5]) << 40;
        /* FALLTHRU */
    case 5:
        b |= ((uint64_t)ni[4]) << 32;
        /* FALLTHRU */
    case 4:
        b |= ((uint64_t)ni[3]) << 24;
        /* FALLTHRU */
    case 3:
        b |= ((uint64_t)ni[2]) << 16;
        /* FALLTHRU */
    case 2:
        b |= ((uint64_t)ni[1]) << 8;
        /* FALLTHRU */
    case 1:
        b |= ((uint64_t)ni[0]);
        break;
    case 0:
        break;
    }

    v3 ^= b;

    for (i = 0; i < _SIPHASH_cROUNDS; ++i)
        _SIPHASH_SIPROUND;

    v0 ^= b;

    if (outlen == 16)
        v2 ^= 0xee;
    else
        v2 ^= 0xff;

    for (i = 0; i < _SIPHASH_dROUNDS; ++i)
        _SIPHASH_SIPROUND;

    b = v0 ^ v1 ^ v2 ^ v3;
    _SIPHASH_U64TO8_LE(out, b);

    if (outlen == 8)
        return 0;

    v1 ^= 0xdd;

    for (i = 0; i < _SIPHASH_dROUNDS; ++i)
        _SIPHASH_SIPROUND;

    b = v0 ^ v1 ^ v2 ^ v3;
    _SIPHASH_U64TO8_LE(out + 8, b);

    return 0;
}

#endif
