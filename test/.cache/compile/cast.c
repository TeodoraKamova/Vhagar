int main()
{
    char c;
    int i;
    void *p;

    i = (int)c;
    c = (char)i;
    p = (void *)i;
    i = (int)p;

    c = (char)c;
    i = (int)i;
    p = (void *)((char *)p);

    return 0;
}
