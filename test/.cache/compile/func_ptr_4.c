int foo(int a, int b)
{
    return a + b;
}

int main()
{
    int (*p1)(int, int);
    int (**p2)(int, int);

    p1 = &foo;
    p2 = &p1;

    return (*p2)(2, 9);
}

