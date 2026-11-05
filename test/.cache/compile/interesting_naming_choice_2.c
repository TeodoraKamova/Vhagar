int foo(int i) { return i; }

int main()
{
    int (*main)(int);
    main = foo;
    return main(0);
}