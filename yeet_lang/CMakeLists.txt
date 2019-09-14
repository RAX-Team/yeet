cmake_minimum_required(VERSION 3.12)
project(yeet_lang C)

set(CMAKE_C_STANDARD 99)

include_directories(src)
include_directories(src/parser)
include_directories(src/interpreter)
include_directories(src/pi_interface)

add_executable(run
        src/pi_interface/7seg.h
        src/pi_interface/7seg.c
        src/pi_interface/switch.c
        src/pi_interface/switch.h
        src/pi_interface/led.c
        src/pi_interface/led.h
        src/pi_interface/yeet_exec.c
        src/pi_interface/event_loop.c
        src/pi_interface/event_loop.h
        src/yeet.c
        src/yeet.h)

add_executable(yeet
        src/parser/lexer.c
        src/parser/lexer.h
        src/parser/lexeme.c
        src/parser/lexeme.h
        src/parser/lexemestream.c
        src/parser/lexemestream.h
        src/yeet.c
        src/yeet.h
        src/parser/syntaxtree.c
        src/parser/syntaxtree.h
        src/parser/parser.c
        src/parser/parser.h
        src/interpreter/interpreter.c
        src/interpreter/interpreter.h
        src/interpreter/stack.c
        src/interpreter/stack.h
        src/interpreter/data.c
        src/interpreter/data.h
        src/pi_interface/event_loop.h
        src/pi_interface/event_loop.c
        src/pi_interface/7seg.c
        src/pi_interface/7seg.h
        src/pi_interface/led.h
        src/pi_interface/led.c src/pi_interface/switch.c src/pi_interface/switch.h)

add_executable(test_lexer
        src/parser/lexer.c
        src/parser/lexer.h
        src/parser/lexeme.c
        src/parser/lexeme.h
        src/parser/lexemestream.c
        src/parser/lexemestream.h
        language_tests/lexer.c
        language_tests/lexer.h
        src/pi_interface/event_loop.h
        src/pi_interface/event_loop.c)

add_executable(test_parser
        src/parser/lexer.c
        src/parser/lexer.h
        src/parser/lexeme.c
        src/parser/lexeme.h
        src/parser/lexemestream.c
        src/parser/lexemestream.h
        language_tests/parser.c
        language_tests/parser.h
        src/parser/syntaxtree.c
        src/parser/syntaxtree.h
        src/parser/parser.c
        src/parser/parser.h
        src/pi_interface/event_loop.h
        src/pi_interface/event_loop.c)

add_executable(test_interpreter
        src/parser/lexer.c
        src/parser/lexer.h
        src/parser/lexeme.c
        src/parser/lexeme.h
        src/parser/lexemestream.c
        src/parser/lexemestream.h
        language_tests/interpreter.c
        language_tests/interpreter.h
        src/parser/syntaxtree.c
        src/parser/syntaxtree.h
        src/parser/parser.c
        src/parser/parser.h
        src/interpreter/interpreter.c
        src/interpreter/interpreter.h
        src/interpreter/stack.c
        src/interpreter/stack.h
        src/interpreter/data.c
        src/interpreter/data.h
        src/pi_interface/event_loop.h
        src/pi_interface/event_loop.c)


