char readchar();

void process_next_char();

int main()
{
    char ch;
    while ((ch = readchar()) != -1)
    {
        process_next_char();
    }

    return 0;
}
