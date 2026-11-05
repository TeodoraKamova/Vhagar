int main()
{
    int a;
    a = 1024;

    int b;
    int c;
    b = a << 2;
start:
    c = 0;

    while (a != b)
    {
        a += 2;
        goto lab1;
    }
    return c;

lab1:
    a = b;
    goto start;

    return 1;
}