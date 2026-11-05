struct T
{
    char b;
    int c;
};

struct S
{
    void *a;
    struct T t;
};

int main()
{
    if (sizeof (int) != 4)
        return 1;

    if (sizeof (struct S) != 16)
        return 2;

    struct S s;
    if (sizeof s != 16)
        return 3;

    struct S *p;

    if (sizeof p != 8)
        return 4;

    if (sizeof *p != 16)
        return 5;

    if (sizeof "abcd" != 5)
        return 6;

    if (sizeof (char) != 1)
        return 7;

    if (sizeof (void *) != 8)
        return 8;

    if (sizeof (struct S *) != 8)
        return 9;

    if (sizeof &main != 8)
        return 11;

    if (sizeof sizeof (int *) != 4)
        return 12;

    char c;
    int i;

    if (sizeof (c + i) != 4)
        return 13;

    if (sizeof (c + c) != 4)
        return 14;

    if (sizeof (&i + 1) != 8)
        return 15;

    if (sizeof (s.t.c) != 4)
        return 16;

    if (sizeof(int* (*(*)(char*))(void*)) != 8)
        return 17;

    if (sizeof 'x' != 4)
        return 18;

    return 0;
}
