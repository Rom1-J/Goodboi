BUILD_DIR := build

SRC_FILES := $(shell find $(CURDIR) -type f -name '*.c')
REL_SRC_FILES := $(patsubst $(CURDIR)/%, %, $(SRC_FILES))
OBJ_FILES := $(REL_SRC_FILES:.c=.o)

obj-m += main.o
main-objs := $(OBJ_FILES)

KBUILD_CFLAGS += -DDEBUG=1

JOBS := $(shell nproc)
KDIR := /lib/modules/$(shell uname -r)/build

.PHONY: all
all: prepare build

.PHONY: rebuild
rebuild: clean prepare build

.PHONY: prepare
prepare:
	mkdir -p $(BUILD_DIR)
	cp -r $(CURDIR)/src $(BUILD_DIR)
	cp -r Makefile $(BUILD_DIR)/Makefile

.PHONY: build
build:
	$(MAKE) -j$(JOBS) -C $(KDIR) M=$(CURDIR)/$(BUILD_DIR) modules

.PHONY: clean
clean:
	$(MAKE) -j$(JOBS) -C $(KDIR) M=$(CURDIR)/$(BUILD_DIR) clean || true
	rm -rf $(BUILD_DIR)
