int main(int argc, char **argv)
{
    if (argc == 1)
        goto A;
    else
        goto B;

A:
B:
    return 99;
}

