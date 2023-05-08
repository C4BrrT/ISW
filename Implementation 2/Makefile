CFLAGS?=-g -Wall -Wno-unused-value

all: metrorec

metrorec: metrorec-runner3.c metrorec.c
	$(CC) $(CFLAGS) -o metrorec metrorec-runner3.c -lpthread 

clean:
	$(RM) metrorec reaction
	$(RM) -r *.dSYM
