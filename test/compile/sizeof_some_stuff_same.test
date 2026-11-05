struct A
{
    char a;
    int b;
    char c;
    char d;
    char e;
};

struct B
{
    int* a;
};

struct C
{
    struct A a;
    struct B b;
};

struct D
{
    struct A a;
    char c;
    int d;
    struct B b;
};

struct E
{
    struct A a;
    char c;
    char d;
    struct B b;
};

int main()
{
    if (sizeof(struct A) != 12)
        return 1;

    if (sizeof(struct B) != 8)
        return 2;

    if (sizeof(struct C) != 24)
        return 3;

    if (sizeof(struct D) != 32)
        return 4;

    if (sizeof(struct E) != 24)
        return 5;
}
