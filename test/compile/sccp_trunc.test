// no inline, so we don't imm ret 0
int foo(char* c) { return c[0]; }

int main()
{
    int a;
    char b;
    a = 252645197;
    b = a;
    return foo(&b) - 77;
}
