int g;
void foo(int a) { g = a; }

int main()
{
    g = 1;
    void (*c4_print_int)(int);
    c4_print_int = foo;
    c4_print_int(0);
    return g;
}