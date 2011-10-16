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
 *       Modified:  10/12/2011 09:42:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Jones (), jsjones96@gmail.com
 *        Company:
 *
 *  Reference Federal Information Processing Standard 
 *  Publications (FIPS PUBS) 180-1 and FIPS PUB 180-2
 *  for specification of the sha-1 hash function. FIPS
 *  PUBS are issued by the National Institute of Standards
 *  and Technology (NIST).
 *
 *  Copyright (C) 2010-2011  Jason Jones
 *
 *  This program is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this program.  If not, see
 *  <http://www.gnu.org/licenses/>.
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

    uint32  h_sub[5];             /* from spec: H[0], ..., H[4] */

};



char * sha_hash_file(char *file);
char * sha_hash_str(char *str);

void sha_hash_file_output(char *file);

#endif
