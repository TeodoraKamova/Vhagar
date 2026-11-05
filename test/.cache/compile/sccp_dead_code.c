int foo(int a, int b, int c)
{
    (void*)&a; // to prevent inlining
    return a + b + c;
}

int main()
{
    int a;
    int b;
    int c;
    int d;
    int undef;

    a = 1;     // 1
    b = 1;     // 1
    c = a + b; // 2
    d = c + 1; // 3 (also dead, kinda)
    b = d;     // 3

    int dead;
    dead = a + b * c - d; // 0
    while (undef)
        ++dead;
    ++dead;
    ++dead;
    dead = 48 * dead * 2 + 4;

    return foo(a, b, c);
}
