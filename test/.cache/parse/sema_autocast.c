void* malloc(int size);

int main()
{
    int* ia;
    ia = malloc(20 * sizeof(int));
    for (int i = 0; i < 20; ++i)
        ia[i] = i;
    return ia[5]; // 5
}
