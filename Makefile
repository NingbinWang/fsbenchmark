FSBENCH_VERSION = 1.0.0
# common
CURDIR = $(shell pwd)
export ROOT_PATH = $(CURDIR)

#misc
BUILD_COMPLETE_STRING ?= $(shell date "+%a, %d %b %Y %T %z")
UID := $(shell id -u)
GID := $(shell id -g)


OUTPUT_DIR= $(CURDIR)/output


TARGET_CROSS_HOST =$(CROSS_COMPILE)



export $(TARGET_CROSS_HOST)

# rules
include $(ROOT_PATH)/linux.mk



SRC_DIR = $(ROOT_PATH)/common
SRC_DIR += $(ROOT_PATH)/libc
SRC_DIR += $(ROOT_PATH)/fdisk
SRC_DIR += $(ROOT_PATH)/medium
SRC_DIR += $(ROOT_PATH)/posix
SRC_DIR += $(ROOT_PATH)/thread
SRC_DIR += $(ROOT_PATH)
INC_DIR = $(ROOT_PATH)/inc

#path
ifneq ($(SRC_DIR),"")
SRCS_CPP += $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp))  
SRCS_CC += $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cc))  
SRCS_C += $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.c))
INC_PATH += $(patsubst %,-I%,$(SRC_DIR))
endif

ifneq ($(INC_DIR), "")
INC_DIRS = $(shell find $(INC_DIR) -maxdepth 3 -type d)
INC_PATH += $(patsubst %,-I%,$(INC_DIRS))
endif


OBJS_CPP:= $(patsubst %.cpp, %.o, $(SRCS_CPP))
OBJS_CC := $(patsubst %.cc, %.o,  $(SRCS_CC))
OBJS_C  := $(patsubst %.c, %.o,  $(SRCS_C))


LD_C_FLAGS   +=  -ldl -lm -lpthread -lrt  -std=c99
LD_CPP_FLAGS +=  -ldl -lm -lpthread -lrt  -lstdc++  #C++参数


checkenv:
	@if [ ! -e $(OUTPUT_DIR)/ ]; then \
		mkdir $(OUTPUT_DIR); \
	fi

.PHONY: all

all: checkenv
	$(CC) -o $(OUTPUT_DIR)/fsbench  $(INC_PATH)  $(SRCS_C) $(LD_C_FLAGS)
	$(ECHO) "Finish generating images at $(BUILD_COMPLETE_STRING)"
	

clean:
	@$(RM)  *.o -rf
	@$(ECHO) "RM  $(OUTPUT_DIR)"
	@$(RM)  $(OUTPUT_DIR)
