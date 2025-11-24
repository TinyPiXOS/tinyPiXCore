#!/bin/bash


# Dependencies
export LD_LIBRARY_PATH=./lib:/System/lib
# Startup Parameters
ARG=--fullscreen --server 

# Execute Application
APP_PATH ="./bin/
if [ ! -f "$APP_PATH" ]; then
	echo "应用程序不存在，无法启动。"
	exit 1
fi
./$APP_PATH "$ARG"

PID=$!
echo "应用程序PID: $PID"
