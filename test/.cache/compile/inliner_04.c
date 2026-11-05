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

int f(int x) {

    if (x >= 0)
    {
        if (x % 2 == 0)
        {
            return x;
        }
        else
        {
            return -x;
        }
    }
    else
    {
        if (x == -5)
        {
            return 14;
        }
        else
        {
            return 57;
        }
    }
}

int main()
{
    return f(0);
}
