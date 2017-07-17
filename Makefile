#
# Makefile for simulator
# Note: Requires 64-bit x86-64 system
#
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -m64 -g

all: csim.c cachelab.c cachelab.h
  $(CC) $(CFLAGS) -o csim csim.c cachelab.c -lm
#
# Clean the src directory
#
clean:
  rm -rf *.o
  rm -f csim
  rm -f .csim_results .marker
