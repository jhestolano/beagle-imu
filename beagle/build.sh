#!/usr/bin/env bash

set -e

[[ -d ./build ]] && rm -rf ./build
mkdir ./build

echo "building mpu9250 demo..."
gcc -Wall -Werror -Wpedantic -c mpu9250.c
gcc -Wall -Werror -Wpedantic -c test_mpu9250.c
mv *.o ./build
gcc ./build/mpu9250.o ./build/test_mpu9250.o -o mpu9250_test
echo "done."

echo "building websockets app..."
gcc -Wall -Werror -Wpedantic -c server.c
mv *.o ./build
gcc ./build/mpu9250.o ./build/server.o -o mpu9250_ws
echo "done."
