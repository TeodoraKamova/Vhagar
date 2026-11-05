struct Point
{
    int x;
    int y;
};

struct Rectangle
{
    struct Point top_left;
    struct Point bottom_right;
};

struct Rectangle create_rectangle(int x1, int y1, int x2, int y2)
{
    struct Rectangle rect;
    rect.top_left.x = x1;
    rect.top_left.y = y1;
    rect.bottom_right.x = x2;
    rect.bottom_right.y = y2;
    return rect;
}

int main()
{
    struct Rectangle rect = create_rectangle(0, 0, 10, 10);
    return 0;
}
