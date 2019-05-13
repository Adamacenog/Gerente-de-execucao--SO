IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./obj
LDIR =../lib

LIBS=-lm

_DEPS = processManager.h scheduler.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = delayedMulti-ProcessExecution.o processManager.o scheduler.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: clean delayedMulti-ProcessExecution

delayedMulti-ProcessExecution: $(OBJ)
	gcc -std=c99  -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ delayedMulti-ProcessExecution
