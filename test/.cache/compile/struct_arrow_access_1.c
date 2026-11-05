struct S
{
    int x;
};

int f(struct S *p)
{
    return p->x; // (*p).x
}

int main()
{
    struct S s;
    s.x = 99;

    return f(&s);
}


