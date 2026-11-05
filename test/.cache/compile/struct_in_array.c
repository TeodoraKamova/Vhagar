void *c4_malloc(int);
void c4_free(void *);

struct S
{
    int x;
};

int test_arr(struct S *arr, int i)
{
    return arr[i].x + (arr+i)->x + (&arr[i])->x + (*&arr[i]).x;
}

int main()
{
    struct S *arr;

    arr = c4_malloc(3 * sizeof (struct S));

    arr[0].x = 19;
    arr[1].x = 20;
    arr[2].x = 21;

    int res;

    res = test_arr(arr, 1);

    c4_free(arr);

    if (res != 4 * 20)
        return 1;

    return 0;
}

