//
// Created by mrr18 on 11/06/19.
//

#include "parser.h"

#define test(name, type, expected, test) ({\
printf(name ": " type " (expected: " type ")\n", test, expected);\
})

#define st_test(name, expected, test) ({\
ParseResult result = (test);\
printf(name ": %s %d ", result.success ? "parsed" : "not parsed", result.consumed);\
if(result.tree) st_print(result.tree);\
printf(" (expected: %s)\n", expected);\
})

#define prog_test(name, expected, test) ({\
Programme* result = (test);\
printf(name ": %s ", result ? "parsed" : "not parsed");\
if(result) programme_print(result);\
printf(" (expected: %s)\n", expected);\
})

#define with_stream(string, code) ({\
LexemeStream* stream = lex(string);\
printf("Lexemes: "); ls_print(stream); puts("");\
code \
/*ls_cleanup(stream); FIXME more work required */ \
})

void header(const char* title) {
    printf("\n== %s ==\n", title);
}

void subheader(const char* title) {
    printf("-- %s --\n", title);
}

void scanline_test(void) {
    header("Scan Line Test");
    with_stream("9 \"word\" do \"hello\" 5", {
        test("Scanline", "%d", 2, scan_line(stream, KEYWORD));
        test("Scanline Content", "%d", 3, scan_line_content(
                stream, TEXT, (LexemeContent) {.string = "hello"}));
    });
}

void atom_test(void) {
    header("Atom Test");
    with_stream("9", {
        st_test("Number", "NUMBER(9.0..)", atom(stream));
    });
    with_stream("\"WORD\"", {
        st_test("Text", "TEXT(WORD)", atom(stream));
    });
    with_stream("ButtonA", {
        st_test("Magic", "MAGIC(ButtonA)", atom(stream));
    });
    with_stream("\"weiRD\\\"t Ex7\" do 5", {
        st_test("Text Extended", "TEXT(weiRD\\\"t Ex7)", atom(stream));
    });
    with_stream("wootwoot Ex do 5", {
        st_test("Identifier Extended", "IDENTIFIER(wootwoot)", atom(stream));
    });
}

void call_test(void) {
    header("Call Test");
    with_stream("function argument", {
        st_test("1 argument simple call",
                "CALL(function=IDENTIFIER(function), 1 argument(s), arg0=IDENTIFIER(argument))",
                function_call(stream));
    });
    with_stream("function fizz 65 \"hello\" World", {
        st_test("4 argument simple call",
                "CALL(function=IDENTIFIER(function), 1 argument(s), arg0=IDENTIFIER(argument))",
                function_call(stream));
    });
}

void op_test(void) {
    header("Op Test");
    with_stream("left + right", {
        st_test("Simple addition",
                "CALL(function=IDENTIFIER(+), 2 argument(s), arg0=CALL(function=IDENTIFIER(left), 0 argument(s)), arg1=CALL(function=IDENTIFIER(right), 0 argument(s)))",
                infix_operator(stream));
    });
    with_stream("left == right", {
        st_test("Simple equals",
                "CALL(function=IDENTIFIER(==), 2 argument(s), arg0=CALL(function=IDENTIFIER(left), 0 argument(s)), arg1=CALL(function=IDENTIFIER(right), 0 argument(s)))",
                infix_operator(stream));
    });
    with_stream("left / 1 * 2", {
        st_test("Multiply and Divide",
                "CALL(function=IDENTIFIER(function), 1 argument(s), arg0=IDENTIFIER(argument))",
                infix_operator(stream));
    });
    with_stream("left * 1 / 2 == right + 5", {
        st_test("Complex",
                "CALL(function=IDENTIFIER(function), 1 argument(s), arg0=IDENTIFIER(argument))",
                infix_operator(stream));
    });
}

void block_test(void) {
    header("Block Test");
    with_stream("\n  print 5\n  gogogo 1\n  go 5\nx y z", {
        st_test("First block test",
                "???",
                block(ls_tail(stream, 1)));
        ls_print(ls_new_section(stream, 1, 12));
        puts(";");
    });
}

void function_assignment_test(void) {
    header("Function Assignment Test");
    with_stream("let bloop x do\n"
                "  noop \"hoop\"\n"
                "  gogogo woop\n"
                "  go Zoop\n"
                "x y z\n", {
                    st_test("Function assignment declaration test",
                            "ASSIGNMENT(target=DECLARATION(name=IDENTIFIER(bloop)), expr=FUNCTION(arg0=IDENTIFIER(x), block=\n"
                            "line 0: CALL(function=IDENTIFIER(noop), 1 argument(s), arg0=TEXT(hoop))\n"
                            "line 1: CALL(function=IDENTIFIER(gogogo), 1 argument(s), arg0=IDENTIFIER(woop))\n"
                            "line 2: CALL(function=IDENTIFIER(go), 1 argument(s), arg0=MAGIC(Zoop))\n"
                            "))",
                            function_assignment(ls_copy(stream)));
                    ls_print(ls_new_section(stream, 0, 16));
                    puts(";");
                });
}

void parse_test(void) {
    header("Parse Test");
    with_stream("let bloop x y do\n"
                "  noop \"hoop\"\n"
                "  gogogo woop\n"
                "  go Zoop\n"
                "x y z\n"
                "press ButtonA do\n"
                "  shine LightA\n"
                "  shine LightC\n", {
<<<<<<< HEAD:extension/yeet_lang/language_tests/parser.c
                    prog_test("Programme parse test",
                              "???",
                              parse(stream));
                    ls_print(ls_new_section(stream, 0, 16));
                    puts(";");
                });
=======
        prog_test("Programme parse test",
                  "???",
                  parse(stream));
        ls_print(ls_new_section(stream, 0, 16));
        puts(";");
    });
>>>>>>> yeet-lang/master:test/parser.c
}

void test_parse_many_statements(void) {
    header("Parse Many Statements Test");
    with_stream("noop \"hoop\"\n"
                "gogogo woop\n"
                "go Zoop\n"
                "x y z\n"
                "shine LightA\n"
                "shine LightC\n"
                "walk home\n"
                "extra func\n"
                "lorem ipsum\n"
                "dolor sit\n"
                "amet consectetur\n"
                "adipiscing elit\n"
                "integer nec\n"
                "odio praesent\n"
                "libero sed\n"
                "cursus ante\n"
                "dapibus diam\n"
                "sed nisi\n"
                "nulla quis\n"
                "sem at\n"
                "nibh elementum\n"
                "imperdiet duis\n"
                "sagittis ipsum\n"
                "praesent mauris\n"
                "fusce nec\n"
                "telus sed\n"
                "augue semper", {
        prog_test("Programme parse test",
                  "???",
                  parse(stream));
        ls_print(ls_new_section(stream, 0, 16));
        puts(";");
    });
}

// Needs a look at. "noop" is not identified properly. ��Hx�U appears instead
void test_parse_large_block(void) {
    header("Parse Large Block Test");
    with_stream("let bloop x do\n"
                "  noop \"hoop\"\n"
                "  gogogo woop\n"
                "  go Zoop\n"
                "  x y z\n"
                "  shine LightA\n"
                "  shine LightC\n"
                "  walk home\n"
                "  extra func\n"
                "  lorem ipsum\n"
                "  dolor sit\n"
                "  amet consectetur\n"
                "  adipiscing elit\n"
                "  integer nec\n"
                "  odio praesent\n"
                "  libero sed\n"
                "  cursus ante\n"
                "  dapibus diam\n"
                "  sed nisi\n"
                "  nulla quis\n"
                "  sem at\n"
                "  nibh elementum\n"
                "  imperdiet duis\n"
                "  sagittis ipsum\n"
                "  praesent mauris\n"
                "  fusce nec\n"
                "  telus sed\n"
                "  augue semper\n", {
                    prog_test("Programme parse test",
                              "???",
                              parse(stream));
                    ls_print(ls_new_section(stream, 0, 16));
                    puts(";");
                });
}

void test_parse_subexpression() {
    header("Parse Subexpression Test");
    with_stream("say (fact 5)\n"
                "let r = fib 10\n", {
        prog_test("Programme parse test",
                  "???",
                  parse(stream));
        puts(";");
    });
}

void test_parse_control_flow() {
    header("Parse Control Flow Test");
    with_stream("say (fact 5)\n"
                "if x == 2\n"
                "    say hello\n"
                "while x /= 2\n"
                "    say 1023\n", {
        prog_test("Programme parse test",
                  "???",
                  parse(stream));
        puts(";");
    });
}

/* Tests that are supposed to and do indeed fail:
 * "boop woop = 1"
 *
 * "boop = let woop = 1"
 *
 * "boop = let foo x do\n",
 * "  thing"
 *
 */
void test_parse_variable_assignment() {
    header("Parse Variable Assignment Test");
    with_stream("let boop = 1\n"
                "boop = 3.14159\n"
                "woop = \"unknown\"\n"
                "let foo x do\n"
                "  thing\n"
                "woop = foo\n"
                "doop = foo = 1\n", {
                    prog_test("Programme parse test",
                              "???",
                              parse(stream));
                    puts(";");
                });
}

int main(int arg_c, char** arg_v) {
//    scanline_test();
//    atom_test();
//    call_test();
//    block_test();
//    function_assignment_test();
//    parse_test();
    //test_parse_many_statements();
    //test_parse_large_block();
//    test_parse_subexpression();
    //test_parse_variable_assignment();
    //op_test();
    test_parse_control_flow();
    return EXIT_SUCCESS;
}