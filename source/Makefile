#.....

ROOT_DIR = $(shell pwd)
INC_DIR = $(ROOT_DIR)/include
LIB_DIR = $(ROOT_DIR)/lib
SERVER_DIR = $(ROOT_DIR)/server
TEST_DIR = $(ROOT_DIR)/test
OUTPUT_DIR = $(ROOT_DIR)/output
BIN_DIR = $(OUTPUT_DIR)/bin

LOG_INC_DIR = $(SERVER_DIR)/log/sdk/include

BIN = jpgdecode

#编译器
CC = gcc
CFLAGS = -Wall -I$(INC_DIR) -I$(LOG_INC_DIR)

#导出路径
export ROOT_DIR INC_DIR LIB_DIR SERVER_DIR TEST_DIR OUTPUT_DIR CC CFLAGS

SRC = $(wildcard $(TEST_DIR)/*.c)
TARGET = $(OUTPUT_DIR)/$(basename $(SRC))

all:$(OUTPUT_DIR) $(BIN)

$(OUTPUT_DIR):CHACKDIR
	@echo "start make"
	make -C $(SERVER_DIR)
	make -C $(TEST_DIR)

.PHONY:CHACKDIR
CHACKDIR:
	mkdir -p $(OUTPUT_DIR) $(BIN_DIR)

$(BIN):$(OUTPUT_DIR)
	$(CC) -o $(BIN_DIR)/$@ $(OUTPUT_DIR)/*.o $(CFLAGS) 

.PHONY:clean
clean:
	make -C $(SERVER_DIR) clean
	make -C $(TEST_DIR) clean
	$(RM) -rf $(OUTPUT_DIR)/*