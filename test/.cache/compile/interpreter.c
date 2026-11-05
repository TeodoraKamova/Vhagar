
int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int div(int a, int b) { return b == 0 ? 0 : a / b; }

int apply
(
    int (*fp)(int, int),
    int a,
    int b
)
{
    return fp(a, b);
}

int (*dispatch(char op))(int a, int b)
{
    if (op == '+') return add;
    if (op == '-') return sub;
    if (op == '*') return mul;
    if (op == '/') return div;

    return 0;
}

int main()
{
    int (*var)(int, int);

    var = dispatch('+');

    return apply(var, 9, 2);
}

