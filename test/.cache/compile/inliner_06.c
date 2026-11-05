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

int f(int x)
{
    goto FUTURE;

PAST:

    return x;

FUTURE:

    goto PAST;
}

int main()
{
    return f(0);
}
