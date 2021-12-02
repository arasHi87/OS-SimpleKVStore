SERVER_DIR := server
CLIENT_DIR := client
UTIL_DIR := util
BUILD_DIR ?= build
SERVER_PORT = 8787
SERVER_HOST = localhost

SERVER_TARGET := ${BUILD_DIR}/server
CLIENT_TARGET := ${BUILD_DIR}/client

COMMON_INCLUDES := common
SERVER_SOURCES := $(wildcard ${SERVER_DIR}/*.c)
CLIENT_SOURCES := $(wildcard ${CLIENT_DIR}/*.c)
UTIL_SOURCES := $(wildcard ${UTIL_DIR}/*.c)

GIT_HOOKS := .git/hooks/applied

__dirs := $(shell mkdir -p ${BUILD_DIR})

DEBUG = -g
CFLAGS = -Wall -lpthread -I${COMMON_INCLUDES} $(DEBUG)

all: $(GIT_HOOKS) ${SERVER_TARGET} ${CLIENT_TARGET}

$(GIT_HOOKS):
	@.githooks/install-git-hooks
	@echo

${SERVER_TARGET}: ${SERVER_SOURCES}
	$(CC) -o $@ ${SERVER_SOURCES} ${UTIL_SOURCES} ${CFLAGS}

${CLIENT_TARGET}: ${CLIENT_SOURCES}
	$(CC) -o $@ ${CLIENT_SOURCES} ${UTIL_SOURCES} ${CFLAGS}

server_up:
	./${SERVER_TARGET} -p ${SERVER_PORT}

client_up:
	./${CLIENT_TARGET} -h ${SERVER_HOST} -p ${SERVER_PORT}

clean:
	${RM} -r ${BUILD_DIR}
