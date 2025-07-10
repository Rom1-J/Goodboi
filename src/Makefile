MODULE_NAME := kernelhooker

# SRC_FILES := $(shell find $(CURDIR) -type f -name '*.c')
# REL_SRC_FILES := $(patsubst $(CURDIR)/%, %, $(SRC_FILES))
# OBJ_FILES := $(REL_SRC_FILES:%.c=%.o)

obj-m += $(MODULE_NAME).o
$(MODULE_NAME)-y := entry.o \
	inc/ftrace/ftrace_helper.o \
	inc/logger/logger.o \
	inc/hooks/utils.o \
	inc/hooks/execve/execve.o

KBUILD_CFLAGS += -DDEBUG=1

JOBS := $(shell nproc)
KDIR := /lib/modules/$(shell uname -r)/build

.PHONY: all
all: build

.PHONY: rebuild
rebuild: clean build

.PHONY: build
build:
	$(MAKE) -j$(JOBS) -C $(KDIR) M=$(CURDIR) modules

.PHONY: clean
clean:
	$(MAKE) -j$(JOBS) -C $(KDIR) M=$(CURDIR) clean
