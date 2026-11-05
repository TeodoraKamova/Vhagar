struct S
{
    int x;
};

int main()
{
    struct S s;

    void *p;
    p = &s;

    ((struct S*)p)->x = 9;

    return s.x + ((struct S*)&s)->x - 18;
}
