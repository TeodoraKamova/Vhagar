int (*f)(int, int);

int foo(int a, int b)
{
    return a + b;
}

int main()
{
    f = &foo;

    return f(2, 9);
}

