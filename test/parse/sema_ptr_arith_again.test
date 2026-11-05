int main()
{
    struct S
    {
        int a;
        int b;
        int c;
    }* s;

    struct S* ptr1;
    ptr1 = 0;
    struct S* ptr2;
    ptr2 = ptr1 + 7;

    if ((char*)ptr2 != (char*)(sizeof(struct S) * 7))
        return 1;

    if (ptr2 - ptr1 != 7)
        return 2;

    if (ptr1 - ptr2 != -7)
        return 3;
}