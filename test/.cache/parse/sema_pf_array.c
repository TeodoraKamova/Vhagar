void func1();
void func2();
void func3();

int main()
{
    void (**funcs)() = (void (**)())(0);
    funcs[0] = func1;
    funcs[1] = func2;
    funcs[2] = func3;

    int i = 0;
    while (i < 3)
    {
        funcs[i](); // Call each function through the array
        i = i + 1;
    }
    return 0;
}

int glob = 1;

void func1() { glob = glob * glob + glob * glob * glob - 16; }
void func2() { glob = glob + 4; }
void func3() { glob = 2; }