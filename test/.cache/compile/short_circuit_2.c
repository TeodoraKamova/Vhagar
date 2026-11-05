int puts(char *);

int main()
{
    int x;
    x = 1;

    char *msg;
    msg = "this should be printed!";

    if (1 == 1 && puts(msg) >= 0)
        x = 98;

    if (0 == 1 || puts(msg) >= 0)
        x = 99;

    return x;
}

