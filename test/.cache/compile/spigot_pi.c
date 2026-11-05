// from https://github.com/victormeloasm/SpigotC/blob/main/main.c
// sligtly adjusted to fit into c4 subset

void* c4_malloc(int sz);
void c4_free(void* p);
void c4_print_str(char* str);
void c4_print_int(int x);

void spigot(int n)
{
    int ft;
    int len;
    int* digits;
    ft = 0;
    len = (10 * n / 3) + 2; // Corrected formula
    digits = (int*)c4_malloc(sizeof(int) * len);

    if (digits == 0)
    {
        c4_print_str("Memory allocation failed.\n");
        return;
    }

    int i;
    for (i = 0; i < len; ++i)
        digits[i] = 2;

    for (i = 0; i < n; ++i)
    {
        int carry;
        carry = 0;
        int j;
        for (j = len - 1; j > 0; --j)
        {
            digits[j] = digits[j] * 10 + carry;
            int temp;
            temp = digits[j] / (j * 2 + 1);
            digits[j] = digits[j] % (j * 2 + 1);
            carry = temp * j;
        }

        digits[0] = digits[0] * 10 + carry;
        int q;
        q = digits[0] / 10;
        digits[0] = digits[0] % 10;

        c4_print_int(q);
        if (ft == 0)
        {
            c4_print_str(".");
            ++ft;
        }
    }

    c4_free(digits);
}

int main()
{
    int n;
    n = 200;

    spigot(n);

    c4_print_str("\n");
    return 0;
}

