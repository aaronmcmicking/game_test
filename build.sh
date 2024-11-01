#!/bin/sh

RAYLIB="-lraylib -lGL -lm -lpthread -ldl -lrt -lX11"

CFLAGS="-Wall -Wextra -fsanitize=undefined -O0 -ggdb -std=c++20 -Wno-overflow -Wno-format-security"

g++ $CFLAGS -o game game.cpp $RAYLIB
