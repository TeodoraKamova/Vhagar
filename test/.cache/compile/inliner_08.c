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

int f(int a)
{
    int x;
    x = a;

    while (1)
    {
        if (x == 1)
        {
            return a;
        }
        else if (x & 1)
        {
            x = 3 * x + 1;
        }
        else
        {
            x = x / 2;
        }
    }
}

int main()
{
    return f(37);
}

