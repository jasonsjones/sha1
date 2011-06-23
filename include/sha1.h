/*
 * ==============================================================
 *       Filename:  sha1.h
 *
 *    Description:  Implementation of the Secure Hash Algorithm
 *                  (SHA-1).  SHA-1 produces a 160-bit message
 *                  digest regardless of the size of the input.
 *                  SHA-1 can be run on string 'abc' or on a 3MB
 *                  pdf file.
 *
 *        Version:  1.0
 *        Created:  11/13/2010 
 *       Modified:  06/22/2011 09:41:10 PM
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

void sha_init(struct sha_hash_s *);

void sha_process_file(struct sha_hash_s *, FILE *fd);
void sha_process_str(struct sha_hash_s *, char *str);

void sha_compute(struct sha_hash_s *);

void sha_output(struct sha_hash_s *, char *);

void sha_hash_file(char *file);
void sha_hash_str(char *str);

#endif
