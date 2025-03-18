CC := gcc
CFLAGS := -std=c99 -I ./include/ -fPIC
CFLAGS_RELEASE := -O2
CFLAGS_DEBUG := -O0 -g2 -fsanitize=leak -fsanitize=undefined -fstack-protector-all -Wall -Wextra -Wmismatched-dealloc -D__DEBUG__

SRC := ./src
BIN := ./bin
LIB := /home/atpan/.atlib

C_SRC := $(wildcard $(SRC)/*.c)
C_OBJ_RLS := ${C_SRC:%.c=%.o} 
C_OBJ_DBG := ${C_SRC:%.c=%_debug.o}

VERSION := 1.0.0
TARGET_RELEASE_NAME := libatlib.so.$(VERSION)
TARGET_RELEASE := $(BIN)/$(TARGET_RELEASE_NAME)
TARGET_DEBUG_NAME := libatlib_debug.so.$(VERSION)
TARGET_DEBUG := $(BIN)/$(TARGET_DEBUG_NAME)

.PHONY: all clean install

all: $(TARGET_RELEASE) $(TARGET_DEBUG)

$(TARGET_RELEASE): $(C_OBJ_RLS)
	$(CC) -shared $^ -o $@

$(TARGET_DEBUG): $(C_OBJ_DBG)
	$(CC) -shared $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_RELEASE) -c $< -o $@

%_debug.o: %.c
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG)  -c $< -o $@

install:
	@ln -sf $(subst /./,/,$(shell pwd)/$(TARGET_RELEASE)) $(LIB)/$(TARGET_RELEASE_NAME)
	@ln -sf $(subst /./,/,$(shell pwd)/$(TARGET_DEBUG)) $(LIB)/$(TARGET_DEBUG_NAME)
	@sudo ln -sf $(LIB)/$(TARGET_RELEASE_NAME) /usr/lib/libat.so
	@sudo ln -sf $(LIB)/$(TARGET_DEBUG_NAME) /usr/lib/libat_debug.so

clean:
	@rm -rf $(TARGET_RELEASE) $(TARGET_DEBUG) $(C_OBJ_RLS) $(C_OBJ_DBG)
