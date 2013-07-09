#!/bin/sh

BIN_PATH="bin/"
LOG_PATH="log/"
DEFAULT_PORT="8080"
DEFAULT_SERVER="127.0.0.1"
SERVER_COMMAND=${BIN_PATH}"server"
CLIENT_COMMAND=${BIN_PATH}"client"
startMode=""
serverAddr=${DEFAULT_SERVER}
serverPort=${DEFAULT_PORT}
repeatCount=10000
procCount=1
needLog=0

function help() {
    echo "\
$0 <mode> [option]

Mode: 
    -s		    start server
    -c		    start client

Option:
    -a		    server address(client mode only)
    --count <count> repeat count(client mode only)
    --proc <count>  create how many process
    -p		    server port
    -l		    record log

Arguments:
    -h|--help	    print this page"
    exit $1
}

function make_log() {
    (($1 3>&1 1>&2 2>&3- | tee /dev/fd/2) 3>&1 1>&2 2>&3- ) | cat >> $2 &
}

if [ $# -lt 1 ]; then
    echo "Not enough arguments"
    help 0
else
    while [ $# -gt 0 ]; do
	case $1 in
	-s)
	    startMode="s"
	;;
	-c)
	    startMode="c"
	;;
	-a)
	    serverAddr=$1
	;;
	-p)
	    serverPort=$1
	;;
	-l)
	    needLog=1
	;;
	--count)
	    repeatCount=$2
	    shift
	;;
	--proc)
	    procCount=$2
	    shift
	;;
	*)
	    echo "Worng argument"
	    help 1
	;;
	esac
	shift
    done

    if [ "x$startMode" = "xs" ]; then
	if [ ${needLog} -gt 0 ]; then
	    mkdir -p ${LOG_PATH}
	    make_log "${SERVER_COMMAND} ${serverPort}" "${LOG_PATH}/server.log"
#	    ${SERVER_COMMAND} ${serverPort} >> ${LOG_PATH}/server.log 2>&1
	else 
	    ${SERVER_COMMAND} ${serverPort}
	fi
    elif [ "x$startMode" = "xc" ]; then
	if [ ${needLog} -gt 0 ]; then
	    mkdir -p ${LOG_PATH}
	    for i in $(seq 1 ${procCount}); do
		make_log "${CLIENT_COMMAND} ${serverAddr} ${serverPort} ${repeatCount}" "${LOG_PATH}/client.log"
	    done
	else
	    for i in $(seq 1 ${procCount}); do
		${CLIENT_COMMAND} ${serverAddr} ${serverPort} ${repeatCount} &
	    done
	fi
    else
	echo "Worng mode"
	help 1
    fi
fi
