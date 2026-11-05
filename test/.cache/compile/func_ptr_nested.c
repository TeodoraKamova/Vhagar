int inner(int a, int b)
{
    return 11;
}

int (*outer(void))(int, int)
{
    return &inner;
}

int main()
{

    return outer()(2, 9);
}


