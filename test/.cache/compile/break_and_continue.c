int main()
{
    int i;
    for (i = 0; i < 20; ++i)
    {
        if (i >= 10)
            break;
    }

    if (i != 10)
        return 1;

    while (i > 0)
    {
        --i;
        if (i < 50)
            continue;
        return 2;
    }

    return i;
}