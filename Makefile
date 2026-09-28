.PHONY: all clean run

CC ?= gcc

TUYA_SDK ?= $(STAGING_DIR)/usr
SDK_BUILD_DIR ?= $(TUYA_SDK)

CFLAGS ?= -Wall -Wextra -g -std=c11 -D_POSIX_C_SOURCE=200809L

CPPFLAGS := \
    -Isystem_info/include \
    -Ituya-agent/include \
    -I$(TUYA_SDK)/include \
    -I$(TUYA_SDK)/interface \
    -I$(TUYA_SDK)/utils \
    -I$(TUYA_SDK)/examples/custom_protocol_basic_demo

SYSTEM_INFO_SRC := $(wildcard system_info/src/*.c)
TUYA_AGENT_SRC := $(wildcard tuya-agent/src/*.c)

SRC := \
    $(SYSTEM_INFO_SRC) \
    $(TUYA_AGENT_SRC)

OBJ := $(patsubst %.c,build/%.o,$(SRC))

TARGET := build/tuya-monitor-daemon

all: $(TARGET)

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(OBJ) \
		-L$(SDK_BUILD_DIR)/lib \
		-llink_core \
		-lmiddleware_implementation \
		-lplatform_port \
		-lutils_modules \
		-lpthread \
		-lubus \
		-lubox \
		-luci \
		-lblobmsg_json \
		-lcjson \
		-lm \
		-o $@

run: all
	LD_LIBRARY_PATH=$(SDK_BUILD_DIR)/lib:$$LD_LIBRARY_PATH \
	./$(TARGET) \
		--device-id DEVICEID\
		--device-secret DEVICESECRET \
		--product-id PRODUCTID \
		--daemon

clean:
	rm -rf build