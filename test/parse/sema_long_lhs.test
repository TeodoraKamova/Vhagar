struct
{
    struct
    {
        struct
        {
            struct
            {
                int* (*finally)();
            } l3;
        } l2;
    } l1;

} g_data;

int main(void)
{
    g_data.l1.l2.l3.finally = 0;
    return *g_data.l1.l2.l3.finally();
}