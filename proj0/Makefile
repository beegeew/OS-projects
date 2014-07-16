# Makefile for Beverly G Wertz
# project 0 - Extra Credit
# CS4352 Spring 2012
#
# comment next line if you want to see, in what order the commands are executed.
#.SILENT:

# Define variables. 
PROG   = Stage2
OBJS   = stage2.o output.o 
SRCS   = stage2.c output.c
CC     = /usr/local/bin/gcc

Stage2: $(OBJS)
	$(CC) $(OBJS) -o Stage2 -lc 
	@echo executable file is $(PROG)

stage2.o: my_header.h stage2.c
	$(CC) -c stage2.c

output.o: my_header.h output.c
	$(CC) -c output.c
	
# to install the program, invoke 'make install'. Installs in
# 'bin' directory of your HOME.

install: $(PROG)
	if [ -f $(HOME)/bin/$(PROG) ]  \
		then  \
		  echo moving $(HOME)/bin/$(PROG) to $(HOME)/bin/$(PROG).old; \
		  mv $(HOME)/bin/$(PROG) $(HOME)/bin/$(PROG).old;  \
		  echo copying $(PROG) $(HOME)/bin; \
		  cp $(PROG) $(HOME)/bin; \
		else \
		  echo copying $(PROG) $(HOME)/bin; \
		  cp $(PROG) $(HOME)/bin; \
	fi

clean:
		rm -f $(OBJS)
