
int add(int a, int b) { return a + b; }

int multiply(int a, int b) { return a * b; }

int foo(int a) { return a; }

int(*just_a_ptr);
int (*vanilla_fun_ptr)(int);
int* (**cinnamon_fun_ptr)(int);

void (*convoluted(int, void (*fp)(int)))(int);

int (*plottwist(int a, int b))(int) { return foo; }

// Function that returns a function pointer
int (**(*get_function_pointer(char op))(int, int))(int)
{
    // if (op == '+')
    //     return add; // sema error -> incorrect return type

    // else if (op == '*')
    //     return multiply; // sema error -> incorrect return type

    int (*(*fptr)(int, int))(int);
    fptr = plottwist;
    int (*p0)(int, int(int)); // same as line below!
    int (*p1)(int, int (*)(int));

    // kyword int
    return 0;
}

void nothing(void) {}

void what_is_even_happening(void)
{
    void (*func_ptr)();
    func_ptr = nothing; // Pointer to function
    void (**ptr_to_func_ptr)();
    ptr_to_func_ptr = &func_ptr; // Pointer to pointer to function

    (**ptr_to_func_ptr)(); // Call the function through the pointer to pointer
}

struct Vec3
{
    int x;
    int y;
    int z;
};

struct Vec3* scale(struct Vec3* base, int magnitude)
{
    base->x = base->x * magnitude;
    base->y = base->y * magnitude;
    base->z = base->z * magnitude;
    return base;
}

int main()
{
    int (*func_ptr1)(int, int);
    func_ptr1 = add;
    if (func_ptr1(1, -1) != 0)
        return func_ptr1(1, 2);

    return 0;
}
