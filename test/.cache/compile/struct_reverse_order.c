void c4_print_int(int);

struct S
{
    char x;
    int y;
} s;

int main()
{
    s.y = 4;
    s.x = 2;

    c4_print_int(s.y);
    c4_print_int(s.x); // implicit conversion

    return 0;
}

