void* c4_malloc(int);

struct Node
{
    struct Node* next;
    char* data;
};

struct Node* create(char* data, struct Node* node)
{
    struct Node* new_node;
    new_node = c4_malloc(sizeof(struct Node));

    new_node->data = data;
    new_node->next = node;

    return new_node;
}

int main()
{
    create("b", 0);

    return 0;
}


