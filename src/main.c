/*
 * ==============================================================
 *       Filename:  main.c
 *
 *    Description:  Main function to run my implementation of
 *                  secure hash algorithm (sha1). 
 *
 *        Version:  1.0
 *        Created:  11/29/2010 09:05:37 PM
 *       Modified:  06/24/2011 07:25:54 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Jones (), jsjones96@gmail.com
 *        Company:
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
 * ==============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "sha1.h"


/* 
 * ===  FUNCTION  ===============================================
 *         Name:  main
 *  Description:  Starts execution of the secure hash algorithm.
 *                Designed to take 0 or more command line
 *                arguments, which are the files to return the
 *                message digest for.  If no comand line
 *                arguments are provided, the algorithm reads
 *                from stdin.
 * ==============================================================
 */
int
main(int argc, char *argv[])
{
/*     if (argc > 1 ) {
 *         while (argc > 1) {
 *             sha_hash_file(argv[1]);
 *             argc--;
 *             argv++;
 *         }
 *     } else {
 *         sha_hash_file(NULL);
 *     }
 */

    printf("%s\n", sha_hash_str("jason jones"));

    return EXIT_SUCCESS;
}
