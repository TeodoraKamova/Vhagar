int atoi(char *);

int ack(int m, int n)
{
    if (m == 0)
    {
        return n + 1;
    }
    else
    {
        if (n == 0)
        {
            return ack(m - 1, 1);
        }
        else
        {
            int tmp;
            tmp = ack(m, n - 1);
            return ack(m - 1, tmp);
        }
    }
}

int main(int argc, char **argv)
{
    int m;
    int n;

    if (argc != 3)
        return 1;

    m = atoi(argv[1]);
    n = atoi(argv[2]);

    return ack(m, n);
}

