struct T
{
    int a;
    int b;
};

int test1(struct T *t)
{
    return t->a + (*t).b;
}


int main()
{
    struct T t;

    t.a = 2;
    t.b = 9;

    return test1(&t);
}

