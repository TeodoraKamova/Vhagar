int f(int (*h)(int))
{

}

int g()
{
    int (*h)(int);

    f(h);

    return 0;
}
