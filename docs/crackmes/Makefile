CC = gcc
CFLAGS = -O1
CRACKMES = crack01 crack02 crack03 crack04 crack05 crack06

all: $(CRACKMES)

crack06: crack06.c
	$(CC) $(CFLAGS) -o crack06 crack06.c

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(CRACKMES)

.PHONY: all clean
