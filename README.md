 # SHA1
 #### C implementation of the [secure hash algorithm (SHA1)][wiki].

#### Motivation

This project started as an academic exercise.  Basically, the intent was to understand, on a much
deeper level, a few aspects of cryptography that I explored while studying for the CISSP exam.

Also, I wanted to find an intermediate (yet practical) programming project in order to understand
some of the more advanced aspects of c.

For reference, see the [Federal Information Processing Standard Publication (FIPS PUB) 180-4][180-4]
for the specification of the SHA-1 hash function. FIPS PUBS are issued by the National Institute of
Standards and Technology (NIST).

#### BUILD

To build the basic executable (without verbose output enabled),
run:

```bash
$ make
```

_NOTE: Without any modification to the included makefile, this will produce an executable
named `sha1`._


To build the executable with verbose output enabled, run:

```bash
$ make verbose
```

_NOTE: Without modification to the makefile, this will produce an executable named `sha1-gdb`.
This will also define the DEBUG preprocessor directive._

Running this program will produce a verbose output which prints the components of the hash
algorithm for each round. This is good to see how things are working and to verify the output
against the examples listed in the appendix of the specification.  Included in the `tests`
directory are two files, `sha_spec_example{1,2}.txt`.  These files represent the two of the three
example cases located in the specification for the sha-1 algorithm.

To build and run the test suite located in the `tests` directory, run:

```bash
$ make
$ make run-test
```

`make run-test` will execute the script in the tests directory which will loop over all the `.txt`
files located in the directory and run the actual `sha1sum` program and then run `sha1`.  The
message digests will be side-by-side (so to speak), so you will be able to visually inspect the
digest for correctness.


#### INSTALL

To install the executable in `$(HOME)/bin`, run:

```bash
$ make install
```

If you don't have a `$(HOME)/bin` directory or do not want to install it, it can safely be run from
the project directory where it was built.

To uninstall the executable, run:

```bash
$ make uninstall
```

#### USAGE

To obtain a sha1 message digest for a particular file, run:

```bash
$ sha1 <filename>
```

To obtain a sha1 message digest for several files, run:

```bash
$ sha1 <filename1> <filename2> <filename3>
```

For now, the output is sent to `stdout`.  Maybe a future update will include the option to send
the msg digest to another output file.

#### COPYING/LICENSE

This program is licensed under the Gnu General Public License v3 (GPLv3).  See the file 'LICENSE'
included with the source code for more details.

[wiki]: https://en.wikipedia.org/wiki/SHA-1
[180-4]: http://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
