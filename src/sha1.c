/*
 * ==============================================================
 *       Filename:  sha1.c
 *
 *    Description:  Implementation of the Secure Hash Algorithm
 *                  (SHA-1).  SHA-1 produces a 160-bit message
 *                  digest regardless of the size of the input.
 *                  SHA-1 can be run on string 'abc' or on a 3MB
 *                  pdf file.
 *
 *        Version:  1.0
 *        Created:  11/13/2010
 *       Modified:  06/24/2011 07:26:43 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Jones (), jsjones96@gmail.com
 *        Company:
 *
 *  Reference Federal Information Processing Standard
 *  Publications (FIPS PUBS) 180-1 and FIPS PUB 180-2 for
 *  specification of the sha-1 hash function. FIPS PUBS are
 *  issued by the National Institute of Standards and Technology
 *  (NIST).
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

#include <stdio.h>
#include <stdlib.h>
#include "sha1.h"

/******************* FUNCTION PROTOTYPES ***********************/
static void
sha_init(struct sha_hash_s *hash);

static void
sha_pad(struct sha_hash_s *hash);

static void
sha_output(struct sha_hash_s *hash, char *filename);

static void
sha_process_file(struct sha_hash_s *hash, FILE *fd);

static void
sha_process_str(struct sha_hash_s *hash, char *str);

static void
sha_compute(struct sha_hash_s *hash);

static char *
sha_get_digest(struct sha_hash_s *hash);

/***************** END FUNCTION PROTOTYPES *********************/


/* Constants K sub t */
static uint32 k[ ] = { 0x5A827999,      /* K for  0 <= t <= 19 */
                       0x6ED9EBA1,      /* K for 20 <= t <= 39 */
                       0x8F1BBCDC,      /* K for 40 <= t <= 59 */
                       0xCA62C1D6       /* K for 60 <= t <= 79 */ 
};



/******************* UTILITY FUNCTIONS ***********************/

/* 
 * ===  FUNCTION  ===============================================
 *         Name:  hexdump_char
 *  Description:  Dumps out the hexadecimal equivalent of the 
 *                char c that is passed in as the first 
 *                parameter.  It assigns the hex value to the
 *                char pointer that is passed in as the second 
 *                parameter.  For example, if 'a' is passed, 
 *                the function will assign '61' to str.
 *                Therefore, str must be at least 2 bytes in
 *                length.
 *
 *  Note: I realize there is a %x format specifier for use with
 *  printf, but I wanted to implement my own function as an
 *  exercise of using a lookup table and pointer offsets.
 *
 * ==============================================================
 */
static void
hexdump_char(uint8 c, char *str)
{
    static char *hex_digits = "0123456789abcdef";

    uint8 hi_nibble = (c >> 4);
    uint8 lo_nibble = c & 0x0F;

    *(str + 0) = *(hex_digits + hi_nibble);
    *(str + 1) = *(hex_digits + lo_nibble);
}               /* -----  end of static function hexdump_char  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  hexdump_word
 *  Description:  Extension of hexdump_char function.  Instead
 *                of dumping the hex value of a single byte,
 *                this dumps the hex value of a 4-byte word.
 *
 *   By 'dump' I mean assigning the appropriate hex value to
 *   the corresponding position in str, the char array
 *   that is passed to hold the hex values of the 4-byte word.
 *   str needs to be at least 8 bytes in length, since each byte
 *   is represented by 2 hex digits, one for each nibble.
 *
 * ==============================================================
 */
static void
hexdump_word(uint32 word, char *str)
{
    hexdump_char(( word  >> 24),        &str[0]);
    hexdump_char(((word >> 16) & 0xFF), &str[2]);
    hexdump_char(((word >>  8) & 0xFF), &str[4]);
    hexdump_char(( word & 0xFF),        &str[6]);
}           /* -----  end of static function hexdump_word  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  get_uint32
 *  Description:  Combines four one-byte values pointed to by
 *                buf (and idx) into one 4-byte word.  Returns
 *                the resulting 4-byte word.
 *
 *  Consider to following example:
 *        n  = 00000000000000000000000000000000
 *     byte 0: 01100011    bitwise or'd with
 *             byte 1; 00111110   bitwise or'd with
 *                     byte 2: 11010010   bitwise or'd with
 *                             byte 3: 00000101  
 *        n =  01100011001111101101001000000101
 * ==============================================================
 */
static uint32
get_uint32(uint8 *buf, int idx)
{
    uint32 n = 0;

    n = ( buf[idx    ] << 24 )
      | ( buf[idx + 1] << 16 )
      | ( buf[idx + 2] <<  8 )
      | ( buf[idx + 3]       );

    return n;
}		/* -----  end of static function get_uint32  ----- */



/* 
 * ===  FUNCTION  ==========================================
 *         Name:  get_uint8
 *  Description:  Returns the byte identified by byte_num from
 *                the 4-byte word.  This is a way to decompose a
 *                4-byte word (could be an int) into its
 *                individual byte components.
 *
 *                The least significant byte is byte 0 and the
 *                most significant byte is byte 3.
 * =========================================================
 */
static uint8
get_uint8(uint32 word, int byte_num)
{
    uint8 ret_val = 0;

    switch (byte_num) {
        case 0:	   /* get the least sig byte (byte 0) */
            ret_val = word & 0xFF;
            break;

        case 1:	   /* get byte 1 */
            ret_val = (word >> 8) & 0xFF;
            break;

        case 2:	   /* get byte 2 */
            ret_val = (word >> 16) & 0xFF;
            break;

        case 3:    /* get the most sig byte (byte 3) */
            ret_val = word >> 24;
            break;

        default:	
            break;
    }				/* -----  end switch  ----- */

    return ret_val;
}		/* -----  end of static function get_uint8  ----- */


/* 
 * ===  FUNCTION  ===============================================
 *         Name:  print_block
 *  Description:  Prints a msg block in hexadecimal notation to
 *                verify the contents of the block and to verify
 *                the padding if applicable.  To make the output
 *                more readable, the function prints the block in
 *                4-byte blocks.  This function is used only in
 *                DEBUG mode.
 *
 *  Conditional compilation directive--only compiles code
 *  wrapped in the #ifdef DEBUG ... #endif when DEBUG is
 *  defined, which is done with 'make test'.  All code wrapped
 *  with the conditional compilation directive allows for the
 *  verbose output.
 * ==============================================================
 */
#ifdef DEBUG
static void
print_block(struct sha_hash_s *hash)
{
    char hex_str[] = "00";
    puts("Output of a 64 byte (512 bits) msg block in HEX:\n");
    printf("length of orig msg (so far) is %d bits\n", hash->lo_length);

    int i;
    for(i = 0; i < (hash->msg_idx); i++) {
        hexdump_char(hash->msg_block[i], hex_str);
        printf("%s", hex_str);

        /* make output readable in 4 byte chunks */
        if( (i+1) % 4 == 0 )
            printf(" ");

        if( (i+1) % 16 == 0)
            printf("\n");
    }
    printf("\n");
}
#endif



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  reset_block
 *  Description:  Resets a block array of the prescribed length
 *                to null values.
 * ==============================================================
 */
static void
reset_block(uint8 *block, int length)
{
    int i;
    for(i = 0; i < length; i++)
        block[i] = 0x00;
}		/* -----  end of static function reset_block  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  rotl
 *  Description:  Returns the value of word after the circular
 *                left rotation of n bits.  The most significant
 *                bits that are shifted left off the uint32 are
 *                wrapped around to be the least significant
 *                bits.
 * ==============================================================
 */
static uint32
rotl(uint32 word, int n) 
{
    return (word << n) | (word >> (32 - n));
}		/* -----  end of static function rotl  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  ch
 *  Description:  Logical function defined in the specification.
 *                This function will be used to compute the hash
 *                in rounds 0 thru 19.
 * ==============================================================
 */
static uint32
ch(uint32 x, uint32 y, uint32 z)
{
    return (x & y) | ((~x) & z);
}		/* -----  end of static function ch  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  parity
 *  Description:  Logical function defined in the specification.
 *                This function will be used to compute the hash
 *                in rounds 20 thru 39, then again in rounds 60
 *                thru 79.
 * ==============================================================
 */
static uint32
parity(uint32 x, uint32 y, uint32 z)
{
    return (x ^ y ^ z);
}		/* -----  end of static function parity  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  maj
 *  Description:  Logical function defined in the specification.
 *                This function will be used to compute the hash
 *                in rounds 40 thru 59.
 * ==============================================================
 */
static uint32
maj(uint32 x, uint32 y, uint32 z)
{
    return (x & y) | (x & z) | (y & z);
}		/* -----  end of static function maj  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  sha_pad
 *  Description:  Pads a msg block to ensure that it is contains
 *                512 bits.  A msg block is first padded with a
 *                '1' bit, then followed by k '0' bits, for k >
 *                0.  The msg block is padded with '0' bits until
 *                the last 64 bits of the block.  The length of
 *                the orig msg (in bits, and w/o paddding) is
 *                added as the last 64 bits of the msg block.
 * ==============================================================
 */
static void
sha_pad(struct sha_hash_s *hash)
{
    /* if the msg_idx is less than 56 (0-55), then there is
     * enough room to append a '1' bit, k '0' bits for k > 0,
     * and finally the the length of the orig msg in the last
     * 64 bits (8 bytes) of the block */
    if (hash->msg_idx < (BLK_SIZE - 8)) {

        hash->msg_block[hash->msg_idx++] = 0x80;

        while (hash->msg_idx < (BLK_SIZE - 8)) {
            hash->msg_block[hash->msg_idx++] = 0x00;
        }

    /* if 55 < msg_idx < 64 (56-63), then there is NOT enough room 
     * to pad the current msg block with a '1' bit and 64 bits for 
     * length.  We need to append a '1' bit then append k '0' bits 
     * to fill the current block.  Once the current block is filled,
     * we need to pad an additional block with k '0' bits, then append
     * the length of the orig msg as the last 64 bits of the additional
     * (and final) msg block. */
    } else if (hash->msg_idx > (BLK_SIZE - 9) && hash->msg_idx < BLK_SIZE) {

        hash->msg_block[hash->msg_idx++] = 0x80;

        while (hash->msg_idx < BLK_SIZE) {
            hash->msg_block[hash->msg_idx++] = 0x00;
        }

        /* the msg block is full, so process the block */
        sha_compute(hash);

        /* reset the msg block  */
        reset_block(hash->msg_block, BLK_SIZE);
        hash->msg_idx = 0;

        /* fill with '0's up to the last 64 bits--need to leave room
         * for the length */
        while (hash->msg_idx < (BLK_SIZE - 8)) {
            hash->msg_block[hash->msg_idx++] = 0x00;
        }


    /* even if the msg block is a multiple of 512 bits in length, we 
     * need to add an additional block that is padded with with a '1' 
     * then the a bunch of '0's and finally the length of the the 
     * orig msg */
    } else if (hash->msg_idx == BLK_SIZE) {

        /* reset the msg block  */
        reset_block(hash->msg_block, BLK_SIZE);
        hash->msg_idx = 0;

        hash->msg_block[hash->msg_idx++] = 0x80;

        while (hash->msg_idx < (BLK_SIZE - 8)) {
            hash->msg_block[hash->msg_idx++] = 0x00;
        }
    }

    /* append the final 64 bits (8 bytes) with the length of the 
     * orig msg (in bits) */

    /* A 4 byte word for the length (in bits)  if > 2^32
     * --most like this will always be 00000000 */
    hash->msg_block[BLK_SIZE - 8] = get_uint8(hash->hi_length, 3);   /* idx = 56 */
    hash->msg_block[BLK_SIZE - 7] = get_uint8(hash->hi_length, 2);   /* idx = 57 */
    hash->msg_block[BLK_SIZE - 6] = get_uint8(hash->hi_length, 1);   /* idx = 58 */
    hash->msg_block[BLK_SIZE - 5] = get_uint8(hash->hi_length, 0);   /* idx = 59 */
    hash->msg_idx+=4;

    /* the last 4 byte word for the length (in bits) */
    hash->msg_block[BLK_SIZE - 4] = get_uint8(hash->lo_length, 3);   /* idx = 60 */
    hash->msg_block[BLK_SIZE - 3] = get_uint8(hash->lo_length, 2);   /* idx = 61 */
    hash->msg_block[BLK_SIZE - 2] = get_uint8(hash->lo_length, 1);   /* idx = 62 */
    hash->msg_block[BLK_SIZE - 1] = get_uint8(hash->lo_length, 0);   /* idx = 63 */
    hash->msg_idx+=4;

    /* the last (and final) block is now full, so process the block */
    sha_compute(hash);

}		/* -----  end of static function sha_pad  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  sha_init
 *  Description:  Initializes all members of the sha_hash_s
 *                structure to sane values. It also initializes
 *                the values of the msg digest to the values
 *                specified in the sha-1 specification.
 * ==============================================================
 */
static void
sha_init(struct sha_hash_s *hash)
{
    reset_block(hash->msg_block, BLK_SIZE);
    hash->msg_idx = 0;

    hash->hi_length = 0;
    hash->lo_length = 0;

    /* intialize msg digest array to the values specified in
     * sha-1 specification */
    hash->msg_digest[0] = 0x67452301;
    hash->msg_digest[1] = 0xEFCDAB89;
    hash->msg_digest[2] = 0x98BADCFE;
    hash->msg_digest[3] = 0x10325476;
    hash->msg_digest[4] = 0xC3D2E1F0;

}		/* -----  end of static function sha_init  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  sha_process_file
 *  Description:  Processes the input and populates the
 *                corresponding members of the sha_hash_s struct.
 *                The input is specified by the FILE pointer, fd,
 *                which will either be stdin or a previously
 *                opened file descriptor.
 *
 *                It populates the msg block and keeps track of
 *                the file size (in bits).  Once a msg block is
 *                full, the function passes the block (or more
 *                specifically, a pointer to the struct) off to
 *                the sha_compute function.
 * ==============================================================
 */
static void
sha_process_file(struct sha_hash_s *hash, FILE *fd)
{

    int ch = 0;

    /* loop over file descriptor, fd, reading a single character
     * at a time until we hit the end of file */
    while( (ch = fgetc(fd)) != EOF  ) {

        /* add character to the current msg block as long as there is room */
        if (hash->msg_idx < BLK_SIZE) {
            hash->msg_block[hash->msg_idx++] = ch;


        /* msg block if full, so pass off the current block and start new one */
        } else { 
            sha_compute(hash);

            /* reset the block */
            reset_block(hash->msg_block, BLK_SIZE);
            hash->msg_idx = 0;

            /* can't lose the character we just grabbed on this pass */
            hash->msg_block[hash->msg_idx++] = ch;
        }

        /* increment the length of the msg; if the lo 4 bytes are full
         * that is == 2^32, then reset lo 4 bytes and increment the hi
         * 4 bytes, otherwise just add the 8 bits to the length  */
        if (hash->lo_length == 0xFFFFFFFF) {
            hash->lo_length = 0;
            hash->hi_length++;

        } else {
            hash->lo_length += 8;
        }
    }

    /* need to handle the end case when EOF is encountered at a
     * multiple of 512 bits (or 64 bytes).  without this,
     * sha_compute would never be called. */
    if (hash->msg_idx == 64)
        sha_compute(hash);

    /* always pad the msg */
    sha_pad(hash);

}		/* -----  end of static function sha_process_file  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  sha_process_str
 *  Description:  
 * ==============================================================
 */
static void
sha_process_str(struct sha_hash_s *hash, char *str)
{
    while (*str) {

        /* add character to the current msg block as long as there is room */
        if (hash->msg_idx < BLK_SIZE) {
            hash->msg_block[hash->msg_idx++] = *str;


        /* msg block if full, so pass off the current block and start new one */
        } else { 
            sha_compute(hash);

            /* reset the block */
            reset_block(hash->msg_block, BLK_SIZE);
            hash->msg_idx = 0;

            /* can't lose the character we just grabbed on this pass */
            hash->msg_block[hash->msg_idx++] = *str;
        }

        /* increment the length of the msg; if the lo 4 bytes are full
         * that is == 2^32, then reset lo 4 bytes and increment the hi
         * 4 bytes, otherwise just add the 8 bits to the length  */
        if (hash->lo_length == 0xFFFFFFFF) {
            hash->lo_length = 0;
            hash->hi_length++;

        } else {
            hash->lo_length += 8;
        }

        str++;
    }

    /* need to handle the end case when EOF is encountered at a
     * multiple of 512 bits (or 64 bytes).  without this,
     * sha_compute would never be called. */
    if (hash->msg_idx == 64)
        sha_compute(hash);

    /* always pad the msg */
    sha_pad(hash);

}		/* -----  end of static function sha_process_str  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  sha_compute
 *  Description:  This is the heavy lifting function of the sha-1
 *                algorithm. Conducts 80 rounds of computations
 *                and updates the msg digest arrary each time.
 *                When complete, the five 32-bit words that make
 *                up the msg digest array contain the
 *                intermediate or final (if it is the last block
 *                that it processed) value of the hash.  Simply
 *                concatenate the five 32-bit words and the
 *                results is the 160-bit msg digest, or hash.
 * ==============================================================
 */
static void
sha_compute(struct sha_hash_s *hash)
{

#ifdef DEBUG
    char hex_word[] = "00000000";
    print_block(hash);
#endif

    uint32 w[80] = { 0 };

    int t;          /* loop idx; consistent with the specification */

    uint32 a, b, c, d, e, temp, constant;

    /* function pointer to point to the appropriate function to calculate
     * temp.  The function which it will point to is dependent on the
     * loop idx, t. */
    uint32 (*func)(uint32 x, uint32 y, uint32 z);

    a = hash->msg_digest[0];
    b = hash->msg_digest[1];
    c = hash->msg_digest[2];
    d = hash->msg_digest[3];
    e = hash->msg_digest[4];

    /* loop to initialize the w[80] array.  w[0] thru w[15]
     * is initialized to the contents of the msg_block.
     * w[16] thru w[79] is initialized with rotl */
    for(t = 0; t < 80; t++) {
        if (t < 16) {

            w[t] = get_uint32(hash->msg_block, t*4);

        } else {
            w[t] = rotl((w[t-3] ^ w[t-8] ^ w[t-14] ^ w[t-16]), 1);
        }

#ifdef DEBUG
        printf("W[%2d] = ", t);
        hexdump_word(w[t], hex_word);
        printf("%s\n", hex_word);
#endif
    }

    for (t = 0; t < 80; t++) {

        if (t < 20) { 
            func = ch;
            constant = k[0];
        } else if (t > 19 && t < 40) {
            func = parity;
            constant = k[1];
        } else if (t > 39 && t < 60) {
            func = maj;
            constant = k[2];
        } else {
            func = parity;
            constant = k[3];
        }

        temp = rotl(a, 5) + func(b, c, d) + e + w[t] + constant;

        e = d;
        d = c;
        c = rotl(b, 30);
        b = a;
        a = temp;

#ifdef DEBUG
        if (t == 0)
            printf("           A           B          C" \
                   "          D          E\n\n");

        printf("t = %2d: ", t);
        hexdump_word(a, hex_word);
        printf("%s   ", hex_word);
        hexdump_word(b, hex_word);
        printf("%s   ", hex_word);
        hexdump_word(c, hex_word);
        printf("%s   ", hex_word);
        hexdump_word(d, hex_word);
        printf("%s   ", hex_word);
        hexdump_word(e, hex_word);
        printf("%s\n", hex_word);
#endif

    }

    hash->msg_digest[0] += a;
    hash->msg_digest[1] += b;
    hash->msg_digest[2] += c;
    hash->msg_digest[3] += d;
    hash->msg_digest[4] += e;

#ifdef DEBUG
    int i;
    for(i = 0; i < 5; i++) {
        hexdump_word(hash->msg_digest[i], hex_word);
        printf("H[%d] = %s\n", i,  hex_word);
    }
#endif

}		/* -----  end of static function sha_compute  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  sha_output
 *  Description:  Prints the final msg digest and filename or
 *                string name.  The output is identical to the
 *                output of sha1sum.  This is essentially a
 *                wrapper for 'sha_get_digest( )', which only
 *                returns the digest of the file or str.  This
 *                function appends the name of the file to the
 *                end to match the output of sha1sum.
 *
 *                Currently the output is printed to stdout, but
 *                will soon add the ability to print to an output
 *                file.  If that is desired, just need to
 *                redirect the output to a file.
 * ==============================================================
 */
static void
sha_output(struct sha_hash_s *hash, char *name)
{
    printf("%s  %s\n", sha_get_digest(hash), name);
}		/* -----  end of static function sha_output  ----- */


/* 
 * ===  FUNCTION  ===============================================
 *         Name:  sha_get_digest
 *  Description:  Returns a pointer to the generated sha1 message
 *                digest.
 * ==============================================================
 */
static char *
sha_get_digest(struct sha_hash_s *hash)
{
    char *hash_str = malloc((unsigned) (40));

    int i;
    for(i = 0; i < 5; i++) {
        hexdump_word(hash->msg_digest[i], (hash_str + (i*8)));
    }

    return hash_str;
}		/* -----  end of static function sha_get_digest  ----- */

/***************** END UTILITY FUNCTIONS *********************/



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  sha_hash_file
 *  Description:  Main entry function to generate a msg digest
 *                for an input file, given by filename.
 *
 *                If filename is not NULL, the function will open
 *                the filename for reading then pass it off to
 *                sha_process_file.  If the filename is NULL,
 *                then it will read input from stdin.
 *
 *                Once the input is processed and the msg digest
 *                is output, the file is closed.
 * ==============================================================
 */
void
sha_hash_file(char *filename)
{
    FILE  *input_file;

    struct sha_hash_s hash;
    sha_init(&hash);

    /*  if filename is not NULL, so open for reading and
     *  subsequent processing */
    if (filename) {

        input_file	= fopen(filename, "r");

        if (input_file == NULL) {
            fprintf(stderr, "couldn't open file '%s'\n", filename);
            exit(EXIT_FAILURE);
        }

    /* filename == NULL, we read from stdin */
    } else {
        input_file = stdin;
    }

    sha_process_file(&hash, input_file);

    sha_output(&hash, filename);

    /* close input file   */
    if (fclose(input_file) == EOF) {		
        fprintf(stderr, "couldn't close file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

}		/* -----  end of function sha_hash_file  ----- */



/* 
 * ===  FUNCTION  ===============================================
 *         Name:  sha_hash_str
 *  Description:  Main entry function to generate a msg digest
 *                for a string, given by str.
 * ==============================================================
 */
char *
sha_hash_str(char *str)
{
    struct sha_hash_s hash;
    sha_init(&hash);

    sha_process_str(&hash, str);

    return sha_get_digest(&hash);
}		/* -----  end of function sha_hash_str  ----- */
