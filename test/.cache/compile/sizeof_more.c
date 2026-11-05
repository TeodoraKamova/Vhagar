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

    if (sizeof((int*)0 - (int*)0) != 4)
        return 13;

    int a;
    int b;
    if (sizeof(&a - &b) != 4)
        return 14;

    if (sizeof(&a - b) != 8)
        return 15;

    if (sizeof(((struct S*)&a)[5].b) != 4)
        return 16;

    if (sizeof(&*&((struct S*)&a)[-1].b) != 8)
        return 17;

    if (sizeof(((struct S*)&a)[-1]) != sizeof(struct S))
        return 18;

    if (sizeof(void*) != 8)
        return 19;

    if (sizeof(&a - 0) != 8)
        return 20;

    struct S* ptr1;
    ptr1 = 0;
    struct S* ptr2;
    ptr2 = ptr1 + 7;
    if ((char*)ptr2 != (char*)(sizeof(struct S) * 7))
        return 21;

    if (ptr2 - ptr1 != 7)
        return 22;

    if (ptr1 - ptr2 != -7)
        return 23;
}