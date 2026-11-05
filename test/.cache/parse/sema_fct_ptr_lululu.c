int f(int (*h)(int)) {
    return 0;
}

int g()
{
    int x;
    int (*k)(int);

    x = 0;

    f(x != 42 ? k : k);

    return 0;
}
