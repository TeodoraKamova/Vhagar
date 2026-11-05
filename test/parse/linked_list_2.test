/// code from https://www.geeksforgeeks.org/how-to-create-a-linked-list-in-c/, slightly modified to
/// fit in our subset of c

// C Program to create a Linked List
void* malloc(int);
void puts(char*);
void c4_print_int(int);

// Define the structure of Node
struct Node
{
    // Data field. Can add more data according to our need
    int data;

    // Pointer to the next node
    struct Node* next;
};

int main()
{

    // Create the First Node of the Linked List
    // This will serve as the head of the list
    struct Node* first;
    first = malloc(sizeof(struct Node));

    // Assigning data
    first->data = 10;

    // Creating the second struct Node in the list
    struct Node* second;
    second = (struct Node*)malloc(sizeof(struct Node));

    // Assigning data
    second->data = 20;

    // Creating the third struct Node
    struct Node* third;
    third = (struct Node*)malloc(sizeof(struct Node));

    // Assigning data
    third->data = 30;

    // Linking the nodes
    first->next = second; // This will create: 10 -> 20
    second->next = third; // This will create: 10 -> 20 -> 30
    third->next = 0;      // This will create: 10 -> 20 -> 30 -> NULL

    puts("Linked List: ");
    struct Node* temp;
    temp = first;
    while (temp)
    {
        c4_print_int(temp->data);
        temp = temp->next;
    }

    return 0;
}