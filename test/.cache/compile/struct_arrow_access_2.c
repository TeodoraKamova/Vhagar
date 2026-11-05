struct S
{
    int x;
    int y;
};

int main()
{
    struct S s;
    struct S *p;

    p = &s;

    s.y = 99;

    return p->y; // 99
}

