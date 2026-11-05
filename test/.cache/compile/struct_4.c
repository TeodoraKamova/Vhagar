
struct T
{
    int a;
    int b;
};

struct S
{
    struct T *t;
};

int test2(struct S *s)
{
    (*(*s).t).a = 2;
    s->t->b = 9;

    return s->t->a + (*s->t).b;
}

int main()
{
    struct T t;
    struct S s;

    s.t = &t;

    return test2(&s);
}

