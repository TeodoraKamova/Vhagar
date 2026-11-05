int main()
{
    if (sizeof(sizeof(sizeof(sizeof(int* (*)(int, int))))) != 4)
        return 1;

    if (sizeof(sizeof(int)) != sizeof(sizeof(int*)))
        return 2;

    int (*m)(void);
    m = main;
    if (sizeof(m) != 8)
        return 3;
    if (sizeof(&main) != 8)
        return 4;

    if (sizeof(*&*&*&*&*&*&*&*&*&m) != 8)
        return 5;

    char* c;
    if (sizeof(*&*&*&*&*&*&*&*&*c) != 1)
        return 6;

    if (sizeof(&*&*&*&*&*&*&*&*c) != 8)
        return 7;

    struct S
    {
        int a;
        int b;
        int c;
    }* s;

    if (sizeof(s) != 8)
        return 8;

    if (sizeof(*s) != 12)
        return 9;

    struct S s2;

    if (sizeof(s2) != 12)
        return 10;

    if (sizeof(sizeof("Hello World")) != 4)
        return 11;

    if (sizeof("Hello World") != 12)
        return 12;
}