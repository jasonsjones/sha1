#================================================================
#
#     Filename: makefile
#  Description:
#
#        Usage: make            -- generate executable
#               make test       --
#               make tags       -- generate tags file
#               make install    --
#               make uninstall  --
#               make run-test   --
#               make clean      -- remove objects, executable,
#                                  prerequisits
#               make clean-test -- same as clean, changes exec name
#               make print
#               make tarball    -- generate compressed archive
#               make zip        -- generate compressed archive
#
#      Version:
#      Created: 
#     Revision: ---
#
#       Author: Jason S. Jones
#      Company:
#        Email: jsjones96@gmail.com
#
#        Notes: This is a GNU make (make) makefile.
#               c extension   :  c
#================================================================

vpath %.h  include
vpath %.c  src

TARGET = sechash

# ==============================================================
BUILD_DIR = build
SRC_DIR   = src
OBJ_DIR   = obj

# ==============================================================
INSTALL_DIR  = $(HOME)/bin
INCL         = include
INCL_LIB_HDR = $(HOME)/lib/include
LIB_DIR      = $(HOME)/lib 
LIB          = util

CC         = gcc
CFLAGS     = -O2 -Wall -std=c99 -pedantic -I$(INCL) -I$(INCL_LIB_HDR)
DEBUGFLAGS = -ggdb -DDEBUG

# ==============================================================
SRC  = $(wildcard $(SRC_DIR)/*.c)
OBJ  = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:%.c=%.o)))
EXEC = $(TARGET)

LDFLAGS = -L$(LIB_DIR) -l$(LIB)
CFLAGS += $(LDFLAGS)

# ==============================================================


# ------------  archive generation ---------------------------------------------
TARBALL_EXCLUDE = *.{o,gz,zip}
ZIP_EXCLUDE     = *.{o,gz,zip}

prod:$(TARGET) tags 

test: debug tags run-test

$(TARGET) debug: $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	$(CC) $(CFLAGS) -c -o $@ $< 


debug: CFLAGS += $(DEBUGFLAGS)
debug clean-test run-test: EXEC = $(TARGET)-gdb


.PHONY: tags install uninstall clean clean-test dist run-test

tags:
	ctags $(INCL)/*.h $(SRC_DIR)/*.c

install:
	cp $(EXEC) $(INSTALL_DIR)/

uninstall:
	rm $(INSTALL_DIR)/$(EXEC)

clean:
	rm -rf $(OBJ_DIR)/*.o $(EXEC)

clean-test: clean

print:
	@echo "src files: " $(SRC)
	@echo "obj files: " $(OBJ)

run-test:
	tests/sha_test.sh


# ------------ tarball generation ---------------
tarball:
	@lokaldir=`pwd`; lokaldir=$${lokaldir##*/}; \
    rm --force $$lokaldir.tar.gz;               \
    tar --exclude=$(TARBALL_EXCLUDE)            \
        --create                                \
        --gzip                                  \
        --verbose                               \
        --file  $$lokaldir.tar.gz *

# ------------ zip --------------------
zip:
	@lokaldir=`pwd`; lokaldir=$${lokaldir##*/}; \
	zip -r  $$lokaldir.zip * -x $(ZIP_EXCLUDE)

