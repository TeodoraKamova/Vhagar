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
    while (x)
    {
        --x;
    }

    return x;
}

int main()
{
    return f(16);
}
