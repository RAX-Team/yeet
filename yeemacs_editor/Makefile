CC   = gcc
LIBS = gtk+-3.0 gtksourceview-3.0
CFLAGS = -lwiringPi -lpthread -D_POSIX_SOURCE=200809L -D_XOPEN_SOURCE -std=c99 `pkg-config --cflags $(LIBS)` -Wall -Werror
LDLIBS = `pkg-config --libs $(LIBS)`
EXE = main
LDFLAGS = -lpthread -lwiringPi

.SUFFIXES: .c .o

.PHONY: all clean

all: yeemacs
	cp yeemacs ../

yeemacs: yeemacs.o actions.o editor_state.o \
	../yeet_lang/src/yeet.h \
	../yeet_lang/src/parser/parser.o \
	../yeet_lang/src/parser/lexeme.o \
	../yeet_lang/src/parser/lexemestream.o \
	../yeet_lang/src/parser/lexer.o \
	../yeet_lang/src/parser/syntaxtree.o \
	../yeet_lang/src/interpreter/data.o \
	../yeet_lang/src/interpreter/interpreter.o \
	../yeet_lang/src/interpreter/stack.o \
	../yeet_lang/src/pi_interface/7seg.o \
	../yeet_lang/src/pi_interface/led.o \
	../yeet_lang/src/pi_interface/switch.o \
	../yeet_lang/src/pi_interface/event_loop.o \
	../yeet_lang/src/interpreter/prelude.o \
	../yeet_lang/src/interpreter/io.o


clean:
	$(MAKE) clean -C ../yeet_lang/src
	rm -f $(wildcard *.o)
	rm -f yeemacs
