int f(int x) { return x; }

int main(int argc, char **argv)
{
    return f(argc) ? f(argc) - 1 : f(argc * 1);
}
