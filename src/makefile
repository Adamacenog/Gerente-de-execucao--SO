IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=./obj
LDIR =../lib

LIBS=-lm

_DEPS = delayedMulti-ProcessExecution.h messageQueue.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_DEPS2 = scheduler.h messageQueue.h job.h
DEPS2 = $(patsubst %,$(IDIR)/%,$(_DEPS2))

_DEPS3 = processManager.h messageQueue.h job.h
DEPS3 = $(patsubst %,$(IDIR)/%,$(_DEPS3))

_DEPS4 = killScheduler.h messageQueue.h
DEPS4 = $(patsubst %,$(IDIR)/%,$(_DEPS4))

_OBJ = delayedMulti-ProcessExecution.o messageQueue.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_OBJ2 = scheduler.o messageQueue.o job.o
OBJ2 = $(patsubst %,$(ODIR)/%,$(_OBJ2))

_OBJ3 = processManager.o messageQueue.o job.o
OBJ3 = $(patsubst %,$(ODIR)/%,$(_OBJ3))

_OBJ4 = killScheduler.o messageQueue.o
OBJ4 = $(patsubst %,$(ODIR)/%,$(_OBJ4))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS2)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS3)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.c $(DEPS4)
	$(CC) -c -o $@ $< $(CFLAGS)

all: clean executa_postergado escalonador gerente_execucao killScheduler

executa_postergado: $(OBJ)
	gcc -std=c99 -D_POSIX_C_SOURCE -o $@ $^ $(CFLAGS) $(LIBS)

escalonador: $(OBJ2)
	gcc -std=c99 -D_POSIX_C_SOURCE -o $@ $^ $(CFLAGS) $(LIBS)

gerente_execucao: $(OBJ3)
	gcc -std=c99 -D_POSIX_C_SOURCE -o $@ $^ $(CFLAGS) $(LIBS)

killScheduler: $(OBJ4)
	gcc -std=c99 -D_POSIX_C_SOURCE -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ executa_postergado escalonador gerente_execucao killScheduler
