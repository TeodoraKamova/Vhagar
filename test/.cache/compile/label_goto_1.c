int main(int argc, char **argv)
{
    int x;
    x = 0;

LABEL:

    if (x >= argc)
        return argc;

    x = x + 1;

    goto LABEL;
}

