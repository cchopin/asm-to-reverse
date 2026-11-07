# Capstone - defis CTF. Compilation facon crackme (symboles, sans PIE).
CC = gcc
CFLAGS = -O1 -fno-stack-protector -no-pie
all: serrure machine oignon
serrure: serrure.c ; $(CC) $(CFLAGS) $< -o $@
machine: machine.c ; $(CC) $(CFLAGS) $< -o $@
oignon:  oignon.c  ; $(CC) $(CFLAGS) $< -o $@
clean: ; rm -f serrure machine oignon
.PHONY: all clean
