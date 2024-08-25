#!/usr/bin/env bash

set -e

[[ -d ./build ]] && rm -rf ./build

clang -std=c2x -pedantic -Werror -Os -Wall \
	-g3 \
	-ggdb \
	-I ../mongoose \
	-I/opt/homebrew/Cellar/raylib/5.0/include \
	../mongoose/mongoose.c \
	-L/opt/homebrew/Cellar/raylib/5.0/lib \
	-lraylib \
	gui.c \
	shared_data.c \
	imu.c \
	imu_ws.c \
	main.c \
	-o beagle-imu-client

[[ -d ./build ]] || mkdir build
mv beagle-imu-client ./build
