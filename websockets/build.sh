#!/usr/bin/env bash

set -e

[[ -d ./build ]] && rm -rf ./build

clang -Wall -pedantic -Werror -Os \
	-I ../mongoose \
	../mongoose/mongoose.c \
	client.c -o beagle-imu-client

clang -Wall -pedantic -Werror -Os \
	-I ../mongoose \
	../mongoose/mongoose.c \
	server.c -o beagle-imu-server

[[ -d ./build ]] || mkdir build
mv beagle-imu-server beagle-imu-client ./build
