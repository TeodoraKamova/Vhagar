int puts(char *); // adds a trailing \n to any output

int main(int argc, char **argv)
{
    char *msg;
    msg = "print me maybe!";

    return (argc == 1) ? 67 : puts(msg);
}

