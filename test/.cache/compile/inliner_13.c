/*

IDEAS FOR INLINER TESTS:

- inner control flow
    - if-else
    - goto
    - canonical loops
    - irreducible loops
- outer control flow
    - ternary
    - loop condition
    - if branch
    - behind label
    - unreachable
- void return type
- struct return type
- varargs
- tail call
- all instructions

*/

void c4_print_int(int);

void call_me_maybe()
{
    c4_print_int(0);
    return;
}

int main(int argc, char *argv)
{

    if (argc == 1)
        call_me_maybe();

    return 0;
}

