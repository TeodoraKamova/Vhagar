int c4_atoi(char*);
void c4_print_int(int);
void c4_print_str(char*);

int foo(int n) { return n * 289; }

int main(int argc, char** argv)
{
    int i;
    int d;
    for (i = 1; i < argc; ++i)
    {
        d = c4_atoi(argv[i]);
        d = foo(d);
        c4_print_int(d);
        c4_print_str(" ");
    }
    return 0;
}

