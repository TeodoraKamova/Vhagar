int f(char *p)
{
    return &p[100] - p;
}

int main()
{
    char *p;

    return f(p);
}
