struct S
{
    int a;
    struct T
    {
        int b;
        int c;
    } t;
} s;


int main()
{
    s.t.b = 9;
    s.a = 4;
    s.t.c = 6;

    return s.a + s.t.b + s.t.c; // 19
}

