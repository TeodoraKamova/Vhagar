// i know it makes much more sense to swap the data
// but whats the joy in that?
// we wanna benchmark the compilers pointer tango skills

struct Node
{
    struct Node *next;
    struct Node *prev;
    int data;
};

struct List
{
    struct Node *head;
    struct Node *tail;
};

void swap(struct Node *lo, struct Node *hi)
{
    struct Node *lo_prev;
    struct Node *hi_next;

    lo_prev = lo->prev;
    hi_next = hi->next;

    lo->prev = hi;
    hi->next = lo;

    lo_prev->next = hi;
    hi_next->prev = lo;

    lo->next = hi_next;
    hi->prev = lo_prev;
}
