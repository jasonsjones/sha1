#================================================================
#
#     Filename: makefile
#  Description:
#
#        Usage: make            -- generate executable
#               make verbose    -- generate executable with 
#                                  debugging and verbose output
#               make tags       -- generate tags file
#               make install    -- copies exec to $HOME/bin
#               make uninstall  -- removes exec from $HOME/bin
#               make run-test   -- runs the test suite
#               make clean      -- remove objects, executable,
#                                  prerequisits
#               make clean-test -- same as clean, changes exec name
#               make print      -- prints the value of $SRC and $OBJ
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

TARGET = sha1

# ==============================================================
SRC_DIR   = src
OBJ_DIR   = obj
INCL_DIR  = include

# ==============================================================
INSTALL_DIR  = $(HOME)/bin
LIB_DIR      = $(HOME)/lib 
INCL_LIB_HDR = $(HOME)/lib/include
LIB          = util

CC         = gcc
CFLAGS     = -O1 -Wall -std=c99 -pedantic -I$(INCL_DIR) -I$(INCL_LIB_HDR)
DEBUGFLAGS = -ggdb -DDEBUG
LDFLAGS    = -L$(LIB_DIR) -l$(LIB)
CFLAGS    += $(LDFLAGS)

# ==============================================================
SRC  = $(wildcard $(SRC_DIR)/*.c)
OBJ  = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:%.c=%.o)))
EXEC = $(TARGET)

# ============  archive generation =============================
TARBALL_EXCLUDE1 = obj/*.o
TARBALL_EXCLUDE2 = tags
TARBALL_EXCLUDE3 = *.{gz,zip}
ZIP_EXCLUDE     = \*.o \*,gz \*,zip

# ==============================================================
prod:$(TARGET) tags 

verbose: debug tags

$(TARGET) debug: $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	$(CC) $(CFLAGS) -c -o $@ $< 


debug: CFLAGS += $(DEBUGFLAGS)
debug clean-test: EXEC = $(TARGET)-gdb

# ================= PHONY targets ===============================
.PHONY: tags install uninstall clean clean-test run-test

tags:
	ctags $(INCL_DIR)/*.h $(SRC_DIR)/*.c

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
    tar --exclude=$(TARBALL_EXCLUDE1)           \
        --exclude=$(TARBALL_EXCLUDE2)           \
        --exclude=$(TARBALL_EXCLUDE3)           \
        --create                                \
        --gzip                                  \
        --verbose                               \
        --file  $$lokaldir.tar.gz *

# ------------ zip --------------------
zip:
	@lokaldir=`pwd`; lokaldir=$${lokaldir##*/}; \
	zip -r  $$lokaldir.zip * -x $(ZIP_EXCLUDE)

