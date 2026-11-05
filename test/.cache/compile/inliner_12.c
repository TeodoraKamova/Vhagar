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

void do_nothing(int x)
{
    if (x == 5)
    {
        return;
    }
    else
    {
        return;
    }

    return;
}

int main()
{
    do_nothing(5);

    return 0;
}
