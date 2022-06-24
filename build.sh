#!/bin/bash

CC=gcc
CFLAGS="-Wall -Wextra -Werror -pedantic -ggdb"

$CC $CFLAGS -o example example.c