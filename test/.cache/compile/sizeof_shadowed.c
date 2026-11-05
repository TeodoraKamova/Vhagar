void c4_print_int(int);
void c4_print_str(char*);

int main()
{
    int i;
    int res;
    i = res = 0;
    while (i < 10)
    {
        char res;
        {
            res = 4;
            i += sizeof res;
            c4_print_int(i);
            c4_print_str(" ");
        }
    }

    return res;
}

