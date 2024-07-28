#!/usr/bin/env bash

set -e

[[ -d ./build ]] && rm -rf ./build
echo "building mpu9250 demo..."
gcc -std=c17 -Wall -Werror -Wpedantic -c mpu9250.c
gcc -std=c17 -Wall -Werror -Wpedantic -c main.c
mkdir ./build
mv *.o ./build
gcc ./build/mpu9250.o ./build/main.o -o mpu9250

echo "done."
[[ -f ./mpu9250 ]] && ./mpu9250
