
struct T
{
    int a;
    int b;
};

struct T f1(struct T *t)
{
    return *t;
}

struct T *f2(struct T *t)
{
    return t;
}

int main()
{
    struct T t;

    t.a = 2;
    t.b = 9;

    return f1(&t).a + f2(&t)->b;
}

