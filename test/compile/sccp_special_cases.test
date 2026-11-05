int sc1(int a) { return a / a; }
int no1(int a, int b) { return a / b; }
int sc2(int a) { return 0 / a; }
int sc3(int a) { return a / 0; }

int sc4(int a) { return 0 * a; }
int sc5(int a) { return a * 0; }

int sc6(int a) { return a % a; }
int no2(int a, int b) { return a % b; }
int sc7(int a) { return a % 1; }

int sc8(int a) { return a & 0; }
int sc9(int a) { return 0 & a; }

int scA(int a) { return a - a; }
int no3(int a) { return a + a; }

int scB(int a)
{
    int tmp;
    tmp = a | -1;
    return a + tmp;
}

int scC(int a)
{
    char c;
    c = a;
    int tmp;
    c = -1;
    tmp = a | c;
    return a + tmp;
}

int main()
{
    int a; // undef
    int b;
    b = a / a; // is this undef? we interperate it as 1 (which is correct either way)
    return 0;
}