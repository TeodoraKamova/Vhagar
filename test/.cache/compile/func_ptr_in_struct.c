struct FP
{
    int (*fp)(int, int);
};

int foo(int a, int b)
{
    return a + b;
}

int main()
{
    struct FP s;

    s.fp = &foo;

    struct FP *p;

    p = &s;

    return s.fp(2, 9) + p->fp(9, 2);
}

