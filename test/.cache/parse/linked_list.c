void* malloc(int size) { return 0; }

struct Data
{
    int x;
    int y;
    struct Data* (*getDataPtr)(int, struct Data);
};

struct Node
{
    struct Data data;
    struct Node* next;
};

void append(struct Node** head_ref, struct Data new_data)
{
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node)); // TODO
    struct Node* last = *head_ref;
    new_node->data = new_data;
    new_node->next = 0;

    if (*head_ref == 0)
    {
        *head_ref = new_node;
        return;
    }

    while (last->next != 0)
        last = last->next;

    last->next = new_node;
}

int main()
{
    struct Node* head = 0;
    struct Data exampl_data;
    exampl_data.x = 7;
    exampl_data.y = exampl_data.x;
    exampl_data.getDataPtr = 0;

    append(&head, exampl_data);
    append(&head, exampl_data);
    append(&head, exampl_data);
    return head->next->data.x;
}