int g;

int foo()
{
    int g;
    g = 0;
}

int bar() { g = 0; }

int main()
{
    g = 1;
    foo();
    if (g != 1)
        return 1;

    bar();
    return g;
}