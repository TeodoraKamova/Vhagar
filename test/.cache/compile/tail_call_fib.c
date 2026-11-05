int fib_helper(int n, int a, int b)
{
    if (n == 0)
        return a;
    if (n == 1)
        return b;
    return fib_helper(n - 1, b, a + b);
}

int fib(int n) { return fib_helper(n, 0, 1); }

int main()
{
    //
    return fib(40);
}

