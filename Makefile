MODULE_NAME := kernelhooker.ko

SRC_DIR := src
BUILD_DIR := build
DIST_DIR := dist

.PHONY: all
all: prepare build serve

.PHONY: prepare
prepare: clean
	cp -r $(SRC_DIR) $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DIST_DIR)

.PHONY: build
build:
	(cd ${BUILD_DIR} && make)

.PHONY: serve
serve:
	mkdir $(DIST_DIR)
	cp $(BUILD_DIR)/$(MODULE_NAME) $(DIST_DIR)/$(MODULE_NAME)
