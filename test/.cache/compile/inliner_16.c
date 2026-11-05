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

int f(int x) { return x; }

int main(int argc, char **argv)
{
    goto FUTURE;

PAST:

    return f(argc - 1);

FUTURE:

    goto PAST;
}
