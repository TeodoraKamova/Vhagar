
int f(int a, int b, int n)
{
    &n; // budget noinline

    int s; s = 0;

    int i;
    for (i = 0; i < n; ++i)
        s += i * (a - b);

    return s;
}

int main()
{
    return f(2, 3, 4) % 256;
}

