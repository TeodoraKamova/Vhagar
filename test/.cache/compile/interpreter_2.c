
void *c4_malloc(int);
void c4_free(void *);
int c4_atoi(char *);
int c4_strlen(char *);

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int div(int a, int b) { return b == 0 ? 0 : a / b; }

int (*dispatch(char op))(int a, int b)
{
    if (op == '+') return add;
    if (op == '-') return sub;
    if (op == '*') return mul;
    if (op == '/') return div;

    return 0;
}

int apply
(
    int (*fp)(int, int),
    int a,
    int b
)
{
    return fp(a, b);
}

int interpret(char *code, int size)
{
    // this stack contains the functions to evaluate next
    int *stack;
    int sp; sp = 0;

    stack = c4_malloc(size * sizeof(int));

    int i; i = 0;
    while (i < size)
    {
        char ch;
        ch = code[i];

        int (*tmp)(int, int);

        if ((tmp = dispatch(ch)) != 0)
        {
            // recognized that its a function function
            // pop two values from stack and put the result on top
            int a; int b;

            a = stack[sp - 2];
            b = stack[sp - 1];

            stack[sp - 2] = apply(tmp, a, b);

            sp = sp - 1;
        }
        else if (ch == ' ')
        {
            // do nothing
        }
        else
        {
            stack[sp] = c4_atoi(&code[i]);
            sp = sp + 1;
        }

        i = i + 1;
    }

    int result; result = stack[sp - 1];

    c4_free(stack);

    return result;
}


int main(int argc, char **argv)
{
    if (argc == 1)
        return 1;

    return interpret(argv[1], c4_strlen(argv[1]));
}

