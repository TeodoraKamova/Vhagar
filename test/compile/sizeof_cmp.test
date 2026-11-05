int a;
int b;

int main()
{
    a = 2;
    b = 2;
    a = b * 3;

    if (sizeof(a == b) != 4)
        return 1;

    if (sizeof(a != b) != 4)
        return 2;

    if (sizeof(!a) != 4)
        return 3;

    if (sizeof(a < b) != 4)
        return 4;

    a = 0;
    if (sizeof(b / a) != 4)
        return 5;
}