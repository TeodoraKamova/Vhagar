int main(int argc, char **argv)
{
    int i;
    i = argc;
    int counter;
    counter = 0;

    // count the number of arguments
    // using a loop instead of just reading argc
    while (i != 0)
    {
        counter = counter + 1;
        i = i - 1;
    }

    return counter + 1;
}

