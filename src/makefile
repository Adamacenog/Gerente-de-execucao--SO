IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./obj
LDIR =../lib

LIBS=-lm

_DEPS = delayedMulti-ProcessExecution.h processManager.h shm.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_DEPS2 = scheduler.h messageQueue.h
DEPS2 = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = delayedMulti-ProcessExecution.o processManager.o shm.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_OBJ2 = scheduler.o messageQueue.o
OBJ2 = $(patsubst %,$(ODIR)/%,$(_OBJ2))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS2)
	$(CC) -c -o $@ $< $(CFLAGS)

all: clean executa_postergado escalonador

executa_postergado: $(OBJ)
	gcc -std=c99  -o $@ $^ $(CFLAGS) $(LIBS)

escalonador: $(OBJ2)
	gcc -std=c99  -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ executa_postergado escalonador
