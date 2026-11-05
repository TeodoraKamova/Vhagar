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

int fac(int x)
{
    int s;
    s = 1;

    int i;

    if (x > 4)
    {
        s = 24;
        i = 5;
        goto SHORTCUT;
    }

    for (i = 1; i <= x; ++i)
    {
SHORTCUT:
        s *= i;
    }

    return s;
}

int main()
{
    return fac(5);
}

