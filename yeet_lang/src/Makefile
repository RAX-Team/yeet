CC      = gcc
CFLAGS  = -g -lwiringPi -lpthread -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99
LDFLAGS = -lwiringPi -lpthread

.SUFFIXES: .c .o

.PHONY: all src clean

all: yeet
	cp yeet ../
	cp yeet ../../

yeet: yeet.o \
	parser/parser.o \
	parser/lexeme.o \
	parser/lexemestream.o \
	parser/lexer.o \
	parser/syntaxtree.o \
	interpreter/data.o \
	interpreter/interpreter.o \
	interpreter/stack.o \
	pi_interface/7seg.o \
	pi_interface/led.o \
	pi_interface/switch.o \
	pi_interface/event_loop.o \
	interpreter/prelude.o \
	interpreter/io.o

clean:
	rm -f $(wildcard *.o)
	rm -f yeet
	rm -f ../yeet
	rm -f run
	rm -f pi_interface/run
	rm -f ../../run
	rm -f $(wildcard pi_interface/*.o)
	rm -f $(wildcard parser/*.o)
	rm -f $(wildcard interpreter/*.o)