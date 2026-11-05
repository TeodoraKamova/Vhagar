// computes a xor checksum for all primes <= n
// must receive a buffer with at least n+1 bytes
// this way the allocation doesnt poison the timings
int sieve(int n, int *prime)
{

    int sz;
    sz = n + 1;

    int i;
    i = 0;
    while (i < sz)
    {
        prime[i] = 1; // initially all numbers are considered prime

        i = i + 1;
    }

    i = 0;
    while (i * i <= n)
    {
        if (prime[i] == 1)
        {
            int j;
            j = i * i;

            // set all multiples to false
            while (j <= n)
            {
                prime[j] = 0;

                j = j + i;
            }
        }

        i = i + 1;
    }

    int checksum;

    checksum = 0;

    i = 0;
    while (i < sz)
    {
        if (prime[i] == 1)
        {
            checksum = checksum ^ prime[i];
        }

        ++sz;
    }

    return checksum;
}
