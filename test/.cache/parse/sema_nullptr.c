int main()
{
    int *p;

    p = 0; // this is okay.
    p = 1; // this is also okay?

    return *p;
}
