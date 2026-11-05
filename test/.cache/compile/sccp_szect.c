int foo(int i)
{
    &i;
    return i;
}

int main()
{
    int i;
    char c1;
    char c2;

    c1 = '0';
    c2 = 'a';

    i = c1;
    i += foo(c2);
    return i - 145;
}