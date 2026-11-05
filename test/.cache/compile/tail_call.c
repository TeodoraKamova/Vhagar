int fac_helper(int n, int acc)
{
    if (n <= 1)
        return acc;
    return fac_helper(n - 1, n * acc);
}

int fac(int n) { return fac_helper(n, 1); }

int main()
{
    int n;
    n = 6;
    return fac(n);
}

