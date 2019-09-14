#include "interpreter.h"

#define data_test(name, expected, test) ({\
Data* result = (test);\
printf(name ": %s ", result ? "evaluated" : "not evaluated");\
if(result) print_data(result);\
printf(" (expected: %s)\n", expected);\
})

#define with_stream(string, code) ({\
LexemeStream* stream = lex(string);\
printf("Lexemes: "); ls_print(stream); puts("");\
code \
/*ls_cleanup(stream); FIXME requires more work */ \
})

void header(const char* title) {
    printf("\n== %s ==\n", title);
}

void subheader(const char* title) {
    printf("-- %s --\n", title);
}

void values_test(void) {
    header("Values Test");
    ASTNode* node;
    with_stream("9.5", {
        node = atom(stream).tree;
        data_test("Number", "9.5", evaluate(NULL, node));
    });
    with_stream("\"Hello\"", {
        node = atom(stream).tree;
        data_test("String", "\"Hello\"", evaluate(NULL, node));
    });
    with_stream("ButtonA", {
        node = atom(stream).tree;
        data_test("Device", "Device<ButtonA>", evaluate(NULL, node));
    });
}

void programme_test(void) {
    header("Programme Test");
    subheader("First Programme");
    with_stream(
            "let left first second do\n"
            "    first\n"
            "\n"
            "left 5 2\n", {
                Programme* programme = parse(stream);
                Stack* stack = run(programme);
                print_stack(stack);
            });
    subheader("Second Programme: Double Definitions");
    with_stream(
            "let left first second do\n"
            "    first\n"
            "left first 1 do\n"
            "    1\n"
            "\n"
            "left 5 1\n", {
                Programme* programme = parse(stream);
                Stack* stack = run(programme);
                print_stack(stack);
            });
    subheader("Third Programme: Double Definitions");
    with_stream(
            "let left first second do\n"
            "    say first\n"
            "    first\n"
            "left first 1 do\n"
            "    say \"Bloog\"\n"
            "    1\n"
            "\n"
            "left 5 1\n", {
                Programme* programme = parse(stream);
                Stack* stack = run(programme);
                print_stack(stack);
            });
    subheader("Fourth Programme: Variable Assignments");
    with_stream(
            "let x = 0\n"
            "let left first second do\n"
            "    let y = first\n"
            "    x = second\n"
            "    say first\n"
            "    y\n"
            "left first 1 do\n"
            "    let x = 1\n"
            "    say \"Bloog\"\n"
            "    x\n"
            "let getF do\n"
            "    let a = ButtonA\n"
            "    let f do\n"
            "        say a\n"
            "        a\n"
            "    f\n"
            "let r = left 5 2\n" // Originally 5 1
            "let s = left 5 1\n"
            "say x\n"
            "let f' = getF\n"
            "say r\n"
            "say s\n"
            "say f'\n"
            "left getF 1\n", {
                Programme* programme = parse(stream);
                Stack* stack = run(programme);
                print_stack(stack);
            });
}

int main(int arg_c, char** arg_v) {
    values_test();
    programme_test();
    return EXIT_SUCCESS;
}