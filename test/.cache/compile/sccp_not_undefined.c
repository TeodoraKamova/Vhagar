void foo(int* a) { *a = 0; }
void boo(int* a) { *a = 1; }

int main()
{
    void (*fun)(int*);
    fun = boo;
    int res;
    fun(&res); // res = 1
    fun = foo;
    fun(&res); // res = 0
    return res;
}
