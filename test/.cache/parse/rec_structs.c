struct Node
{
    int data;
    struct Node* next;
};

void print_list(struct Node* node)
{
    if (node == 0)
        return;
    // printf("%d ", node->data); // TODO: implement printf xD
    print_list(node->next);
}

struct Node make_node(int data, struct Node* next)
{
    struct Node node;
    node.data = data;
    node.next = next;
    return node;
}

int main()
{
    struct Node n1 = make_node(1, 0);
    struct Node n2 = make_node(2, &n1);
    print_list(&n2);
    return 0;
}
