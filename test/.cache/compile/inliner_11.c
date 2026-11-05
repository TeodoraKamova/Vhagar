/*

IDEAS FOR INLINER TESTS:

- inner control flow
    - if-else
    - goto
    - canonical loops
    - irreducible loops
- outer control flow
    - ternary
    - loop condition
    - if branch
    - behind label
    - unreachable
- void return type
- struct return type
- varargs
- tail call
- all instructions

*/

void c4_print_str(char *);
void* c4_malloc(int);
void c4_free(void *);
int c4_strlen(char *);

void budget_memcpy(char *dst, char *src, int len) {
    for (; len > 0; --len)
    {
        *dst = *src; ++dst; ++src;
    }
}

int main(int argc, char **argv)
{
    char * cp1;

    int len; len = c4_strlen(argv[0]) + 1;

    cp1 = c4_malloc(len);

    budget_memcpy(cp1, argv[0], len);

    c4_print_str(cp1);
    c4_print_str("\n");

    c4_free(cp1);

    return 0;
}

