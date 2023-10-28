CC=mpicc
CCFLAGS=-O3 -Wall 
LDFLAGS=
LIBS=-lpng -lm

EXE=heat_mpi
OBJS=core.o setup.o utilities.o io.o main.o
OBJS_PNG=pngwriter.o


all: $(EXE)

pngwriter.o: pngwriter.c pngwriter.h
core.o: core.c heat.h
utilities.o: utilities.c heat.h
setup.o: setup.c heat.h
io.o: io.c heat.h
main.o: main.c heat.h

$(OBJS_PNG): C_COMPILER := $(CC)
$(OBJS): C_COMPILER := $(CC)

$(EXE): $(OBJS) $(OBJS_PNG)
	$(CC) $(CCFLAGS) $(OBJS) $(OBJS_PNG) -o $@ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(C_COMPILER) $(CCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	-/bin/rm -f $(EXE) a.out *.o *.png *~
