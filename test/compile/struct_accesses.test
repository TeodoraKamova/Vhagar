struct T
{
    int x;
};

struct ST
{
    struct T t;
};

struct SP
{
    struct T *t;
};

int test_st(struct ST *sp)
{
    struct ST s;
    s = *sp;

    return sp->t.x + s.t.x + (*sp).t.x + (&s.t)->x;
}

int test_sp(struct SP *sp)
{
    struct SP s;
    s = *sp;

    return sp->t->x + s.t->x + (*sp).t->x + (*sp->t).x + (*s.t).x;
}

int main()
{
    struct ST st;
    struct SP sp;

    st.t.x = 11;
    sp.t = &st.t;

    if (test_st(&st) != 4 * 11)
        return 1;

    if (test_sp(&sp) != 5 * 11)
        return 2;

    return 0;
}
