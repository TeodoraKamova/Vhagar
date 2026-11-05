int main()
{
    if (sizeof(int) != 4)
        return 1;

    if (sizeof(char) != 1)
        return 1;

    int* a = 0;
    char* b = 0;

    if ((char*)(a + 1) != (b + 4))
        return 1;

    return 0;
}
