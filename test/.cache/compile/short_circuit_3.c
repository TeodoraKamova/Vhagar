int puts(char *);

int main()
{
    int x;
    int y;

    x = 1 && puts("print me! #1");
    y = 0 || puts("print me! #2");

    x = 0 && puts("DO NOT print me! #1");
    y = 1 || puts("DO NOT print me! #2");

    x = 1 && 1;
    y = 0 || 0;

    if (x != y)
        return 99;

    return 1;
}

