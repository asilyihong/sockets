CC = gcc
CFLAGS = -Wall -g
OBJ_PATH = obj
BIN_PATH = bin
SRC_PATH = src
SERVER_OBJ = ${OBJ_PATH}/server.o
SERVER_BIN = server
CLIENT_OBJ = ${OBJ_PATH}/client.o
CLIENT_BIN = client
UTILS_OBJ = ${OBJ_PATH}/utils.o
BINS = ${patsubst %, ${BIN_PATH}/%, ${SERVER_BIN} ${CLIENT_BIN}}
.PHONY: clean

all: 
	${MAKE} ${BINS}
${BIN_PATH}/${SERVER_BIN}: ${SERVER_OBJ} ${UTILS_OBJ}
	@if [ ! -d ${BIN_PATH} ]; then \
	mkdir -p ${BIN_PATH}; \
	fi
	${CC} -o $@ $^ ${CFLAGS}
${BIN_PATH}/${CLIENT_BIN}: ${CLIENT_OBJ} ${UTILS_OBJ}
	@if [ ! -d ${BIN_PATH} ]; then \
	mkdir -p ${BIN_PATH}; \
	fi
	${CC} -o $@ $^ ${CFLAGS}
${OBJ_PATH}/%.o: ${SRC_PATH}/%.c 
	@if [ ! -d ${OBJ_PATH} ]; then \
	mkdir -p ${OBJ_PATH}; \
	fi
	${CC} -c ${INC} -o $@ $< ${CFLAGS}
clean:
	rm -fr ${OBJ_PATH} ${BIN_PATH}
