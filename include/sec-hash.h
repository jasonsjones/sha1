/*
 * ==============================================================
 *       Filename:  sec-hash.h
 *
 *    Description:  Implementation of the Secure Hash Algorithm
 *                  (SHA-1).  SHA-1 produces a 160-bit message
 *                  digest regardless of the size of the input.
 *                  SHA-1 can be run on string 'abc' or on a 3MB
 *                  pdf file.
 *
 *        Version:  1.0
 *        Created:  11/13/2010 
 *       Modified:  12/08/2010 06:36:53 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Jones (), jsjones96@gmail.com
 *        Company:  J2Labs
 *
 *  Reference Federal Information Processing Standard 
 *  Publications (FIPS PUBS) 180-1 and FIPS PUB 180-2
 *  for specification of the sha-1 hash function. FIPS
 *  PUBS are issued by the National Institute of Standards
 *  and Technology (NIST).
 *
 * ==============================================================
 */

#ifndef _SECHASH_H_
#define _SECHASH_H_


#define BLK_SIZE     64

typedef unsigned char uint8;
typedef unsigned int  uint32;

struct sha_hash_s {

    uint8   msg_block[BLK_SIZE];  /* current msg block  */
    uint8   msg_idx;              /* current idx or offset within msg block */

    uint32  hi_length;            /* high 32-bits of msg length */
    uint32  lo_length;            /* low 32-bits of msg length */

    uint32  msg_digest[5];        /* from spec: H[0], ..., H[4] */
};

extern void sha_init(struct sha_hash_s *);

extern void sha_process_input(struct sha_hash_s *, FILE *);

extern void sha_compute(struct sha_hash_s *);

extern void sha_output(struct sha_hash_s *, char *);

extern void sha_hash(char *);

#endif
