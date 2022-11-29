#
#	Aidan Grieve
#

CC = gcc
DEBUG = -g
DEFINES =
#DEFINES +=

CFLAGS = $(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls\
		 -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Werror\
		 -Wdeclaration-after-statement -Wunsafe-loop-optimizations -Wuninitialized $(DEFINES)

PROG1 = primesMT

PROGS = $(PROG1)

all: $(PROGS)

$(PROG1): $(PROG1).o
	$(CC) $(CFLAGS) -o $@ $^ -lm -pthread

$(PROG1).o: $(PROG1).c
	$(CC) $(CFLAGS) -c $<

tar: clean
	tar cvfz $(PROG1).tar.gz *.[ch] ?akefile

clean cls:
	rm -f $(PROG1) *.o *~ \#*
