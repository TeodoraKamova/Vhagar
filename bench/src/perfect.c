
// stolen from hacker's delight
int isqrt(int x)
{
    if (x < 0)
        return 0 - 1;

    int a;
    int b;
    int m; // limits and midpoint

    a = 1;
    b = (x / 32) + 8; // first guess at an upper limit
    if (b > 65535)
        b = 65535;

    m = (a + b) / 2;
    if (m * m > x)
        b = m - 1;
    else
        a = m + 1;

    while (b >= a)
    {
        m = (a + b) / 2;
        if (m * m > x)
            b = m - 1;
        else
            a = m + 1;
    }

    return a - 1;
}

// stolen from rosetta code
int perfect(int n)
{
    int max;
    max = isqrt(n) + 1;
    int total;
    int i;

    total = 1;
    i = 2;
    while (i < max)
    {
        if ((n % i) == 0)
        {
            total = total + i;

            int q;
            q = n / i;

            if (q > i)
                total += q;
        }

        i = i + 1;
    }

    return total == n;
}
