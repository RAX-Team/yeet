#include "lexer.h"

void show(char* text) {
    LexemeStream* stream = lex(text);
    puts(text);
    ls_print(stream);
    puts(".\n");
    ls_cleanup(stream);
}

void main(int arg_c, char** arg_v) {
    show("90 \"hello\"**");
    show("90 \"Hel\\\"lo\" xavier ()+ dowithit\n"
         "  world\n"
         "  do 5\n"
         "x\n"
         "  5\n"
         "  do with\n"
         "    5/5\n"
         "      xavier\n"
         "x");
    show("90 \"Hel\\\"lo\" xavier, ()+> dowithit(\n"
         "  world\n"
         "  do 5\n"
         ") x\n"
         "  5\n"
         "  do with\n"
         "    5/5\n"
         "      xavier\n"
         "x");
    show("90 \"Hel\\\"lo\" xavier, ()+> dowithit(\n"
         "  world -- hello world\n"
         "  do 5\n"
         ") x\n"
         "  5 -- hello\n"
         "  do with if (+) while whilst\n"
         "    5/5\n"
<<<<<<< HEAD:extension/yeet_lang/language_tests/lexer.c
         "      xavier \n");
=======
         "      xavier \n{- hwlllo -}\n");
>>>>>>> yeet-lang/master:test/lexer.c
}