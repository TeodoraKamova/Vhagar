struct T
{
    int a;
    int b;
};

struct T* id(struct T* t)
{
    return t;
}

int main()
{
    struct T t;

    t.a = 2;
    t.b = 9;

    return id(&t)->a + id(&t)->b;
}

