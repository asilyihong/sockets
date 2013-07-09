#!/bin/sh

BIN_PATH="bin/"
LOG_PATH="log/"
SERVER_COMMAND=${BIN_PATH}"server"
CLIENT_COMMAND=${BIN_PATH}"client"
startMode=""
repeatCount=1000000
procCount=1
needLog=0

function help() {
    echo "\
$0 <mode> [option]

Mode: 
    -s		    start server
    -c		    start client

Option:
    --count <count> repeat count(client mode only)
    --proc <count>  create how many process
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
    help 1
else
    while [ $# -gt 0 ]; do
	case $1 in
	-s)
	    startMode="s"
	;;
	-c)
	    startMode="c"
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
	"-h"|"--help")
	    help 0
	;;
	*)
	    echo "Worng argument " $1
	    help 1
	;;
	esac
	shift
    done

    if [ "x$startMode" = "xs" ]; then
	if [ ${needLog} -gt 0 ]; then
	    mkdir -p ${LOG_PATH}
	    make_log "${SERVER_COMMAND}" "${LOG_PATH}/server.log"
	else 
	    ${SERVER_COMMAND}
	fi
    elif [ "x$startMode" = "xc" ]; then
	if [ ${needLog} -gt 0 ]; then
	    mkdir -p ${LOG_PATH}
	    for i in $(seq 1 ${procCount}); do
		make_log "${CLIENT_COMMAND} ${repeatCount}" "${LOG_PATH}/client.log"
	    done
	else
	    for i in $(seq 1 ${procCount}); do
		${CLIENT_COMMAND} &
	    done
	fi
    else
	echo "Worng mode"
	help 1
    fi
fi
