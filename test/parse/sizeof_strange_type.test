struct B;
int foo();
int main()
{
    //
    int a;
    a = sizeof(void (*)(int a));
    a = sizeof(char (*)(int a));
    a = sizeof(struct A { int a; });
    a = sizeof(struct A { struct B b; });
    a = sizeof(sizeof(struct A { struct B b; }(**)(void)));
    a = sizeof(1 == 2);
    a = sizeof(1 && 2);
    a = sizeof(1 || 2);
    a = sizeof(1 | 2);
    a = sizeof(1 ^ 2);
    a = sizeof(~1);
    a = sizeof !0;
    a = sizeof(foo());
    a = sizeof(int(char));
    a = sizeof(void*);
    a = sizeof(a);
    a = sizeof(&a);
    a = sizeof(*&a);
    a = sizeof(*(&*&*&a + 1));
    a = sizeof((&a + 1));
    // a = sizeof(int(*f)()); // quote: "geht in die Richtung"

    struct A
    {
        int a;
    } strA;

    struct He;
    struct He* he;

    sizeof(strA.a = 0);
    sizeof(&strA);
    sizeof(&strA.a);
    sizeof(he);

    struct He
    {
        char c;
    };
    sizeof(*he);
    sizeof(*&he);

    int* arr; // ik it should be int arr[N];
    a = sizeof(arr) / sizeof(arr[0]);

    return a;
}

struct B
{
    int a;
};

int foo()
{
    int a;
    a = sizeof(struct B);
    a = sizeof(struct A { struct B b; }(**)(void));
    return a;
}
