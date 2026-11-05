void *c4_malloc(int);
void c4_free(void *);

struct S
{
    int x;
};

int test_arr(struct S **arr, int i)
{
    return arr[i]->x + (*(arr+i))->x + (*&arr[i])->x;
}

int main()
{
    struct S **arr;

    arr = c4_malloc(3 * sizeof (struct S*));

    struct S s0;
    struct S s1;
    struct S s2;

    s0.x = 19;
    s1.x = 20;
    s2.x = 21;

    arr[0] = &s0;
    arr[1] = &s1;
    arr[2] = &s2;

    int res;

    res = test_arr(arr, 1);

    c4_free(arr);

    if (res != 3 * 20)
        return 1;

    return 0;
}

