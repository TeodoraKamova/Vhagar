void c4_print_int(int);
void c4_print_str(char *);

struct S
{
    int x;
    char y;
};

struct T
{
    struct S s;
    char z;
};

int main()
{

    c4_print_int(sizeof (int));
    c4_print_str(" ");

    c4_print_int(sizeof (char));
    c4_print_str(" ");

    c4_print_int(sizeof (void *));
    c4_print_str(" ");

    c4_print_int(sizeof (struct S));
    c4_print_str(" ");

    c4_print_int(sizeof (struct T));
    c4_print_str(" ");

    c4_print_int(sizeof (&main));
    c4_print_str("\n");

    return 0;
}

