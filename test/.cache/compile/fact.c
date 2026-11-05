int factorial_recursive(int n)
{
    if (n == 0 || n == 1)
        return 1;

    return n * factorial_recursive(n - 1);
}

int factorial_iterative(int n)
{
    int result;
    result = 1;
    int i;
    for (i = 2; i <= n; ++i)
        result *= i;

    return result;
}

int factorial_recursive_ptr(int n, void* fun)
{
    if (n == 0 || n == 1)
        return 1;

    return n * ((int (*)(int, void*))(fun))(n - 1, fun);
}

int main()
{
    int fac9;
    fac9 = factorial_iterative(9);

    if (fac9 != 362880)
        return 1;

    if (factorial_recursive(9) != fac9)
        return 2;

    if (factorial_recursive_ptr(9, (void*)(factorial_recursive_ptr)) != fac9)
        return 3;

    // implicit cast
    if (factorial_recursive_ptr(9, factorial_recursive_ptr) != fac9)
        return 4;

    return 0;
}