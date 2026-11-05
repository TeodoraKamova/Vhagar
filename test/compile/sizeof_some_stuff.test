int main()
{
    struct A
    {
        char a;
        int b;
        char c;
        char d;
        char e;
    };

    if (sizeof(struct A) != 12)
        return 1;

    struct B
    {
        int* a;
    };

    if (sizeof(struct B) != 8)
        return 2;

    struct C
    {
        struct A a;
        struct B b;
    };

    if (sizeof(struct C) != 24)
        return 3;

    struct D
    {
        struct A a;
        char c;
        int d;
        struct B b;
    };

    if (sizeof(struct D) != 32)
        return 4;

    struct E
    {
        struct A a;
        char c;
        char d;
        struct B b;
    };

    if (sizeof(struct E) != 24)
        return 5;
}
