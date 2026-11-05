struct S
{
    int (*f)(int);
};

int f(struct S *s)
{
    int x;
    int y;

    x = s->f(2);

    struct S p;

    p.f = s->f;

    y = p.f(2);

    return x;
}
